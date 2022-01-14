import json
import os
import tarfile
import typing as t
from abc import ABC, abstractmethod
from functools import partial
from urllib.parse import urlparse

import requests
import yaml
from appdirs import AppDirs

from .typing_utils import URL, PathLike


def download_resource(url: URL, save_location: PathLike, force_download=False):
    """
    Downloads a resource from url into save_location, overwrites only if
    force_download is true.
    """
    if force_download or not os.path.exists(save_location):
        response = requests.get(url, stream=True)
        # Throw an error for bad status codes
        response.raise_for_status()
        with open(save_location, "wb") as handle:
            for block in response.iter_content(1024):
                handle.write(block)


def patch_marian_for_bergamot(
    marian_config_path: PathLike, bergamot_config_path: PathLike, quality: bool = False
):
    """
    Accepts path to a config-file from marian-training and followign
    quantization and adjusts parameters for use in bergamot.
    """
    # Load marian_config_path
    data = None
    with open(marian_config_path) as fp:
        data = yaml.load(fp, Loader=yaml.FullLoader)

    # Update a few entries. Things here are hardcode.
    data.update(
        {
            "ssplit-prefix-file": "",
            "ssplit-mode": "paragraph",
            "max-length-break": 128,
            "mini-batch-words": 1024,
            "workspace": 128,  # shipped models use big workspaces. We'd prefer to keep it low.
            "alignment": "soft",
        }
    )

    if quality:
        data.update({"quality": quality, "skip-cost": False})

    # Write-out.
    with open(bergamot_config_path, "w") as output_file:
        print(yaml.dump(data, sort_keys=False), file=output_file)


class Repository(ABC):
    """
    An interface for several repositories. Intended to enable interchangable
    use of translateLocally and Mozilla repositories for usage through python.
    """

    @abstractmethod
    def update(self):
        """Updates the model list"""
        pass

    @abstractmethod
    def models(self) -> t.List[str]:
        """returns identifiers for available models"""
        pass

    @abstractmethod
    def modelConfigPath(self, model_identifier: str) -> str:
        """returns modelConfigPath for for a given model-identifier"""
        pass


class TranslateLocally(Repository):
    """
    This class implements Repository to fetch models from translateLocally.
    AppDirs is used to standardize directories and further specialization
    happens with translateLocally identifier.
    """

    def __init__(self, appDir: AppDirs):
        self.repository = "translateLocally"
        f = os.path.join
        self.dirs = {
            "cache": f(appDir.user_cache_dir),
            "config": f(appDir.user_config_dir),
            "data": f(appDir.user_data_dir),
            "archive": f(appDir.user_data_dir, "archives"),
            "models": f(appDir.user_data_dir, "models"),
        }

        for directory in self.dirs.values():
            os.makedirs(directory, exist_ok=True)

        self.models_file = os.path.join(self.dirs["config"], "models.json")
        self.data = self.update(self.models_file)

        self.data_by_code = {}
        for model in self.data["models"]:
            self.data_by_code[model["code"]] = model

    def update(self, models_file_path: PathLike) -> t.Dict[str, t.Any]:
        url = "https://translatelocally.com/models.json"
        inventory = requests.get(url).text
        with open(models_file_path, "w+") as models_file:
            models_file.write(inventory)
        return json.loads(inventory)

    def models(self, filter_downloaded: bool = True) -> t.List[str]:
        codes = []
        for model in self.data["models"]:
            if filter_downloaded:
                fprefix = self._archive_name_without_extension(model["url"])
                model_dir = os.path.join(self.dirs["models"], fprefix)
                if os.path.exists(model_dir):
                    codes.append(model["code"])
            else:
                codes.append(model["code"])
        return codes

    def modelConfigPath(self, model_identifier: str) -> str:
        model = self.model(model_identifier)
        fprefix = self._archive_name_without_extension(model["url"])
        model_dir = os.path.join(self.dirs["models"], fprefix)
        return os.path.join(model_dir, "config.bergamot.yml")

    def model(self, model_identifier: str) -> t.Any:
        return self.data_by_code[model_identifier]

    def download(self, model_identifier: str):
        # Download path
        model = self.model(model_identifier)
        model_archive = "{}.tar.gz".format(model["shortName"])
        save_location = os.path.join(self.dirs["archive"], model_archive)
        download_resource(model["url"], save_location)

        with tarfile.open(save_location) as model_archive:
            model_archive.extractall(self.dirs["models"])
            fprefix = self._archive_name_without_extension(model["url"])
            model_dir = os.path.join(self.dirs["models"], fprefix)
            symlink = os.path.join(self.dirs["models"], model["code"])

            print(
                "Downloading and extracting {} into ... {}".format(
                    model["code"], model_dir
                ),
                end=" ",
            )

            if not os.path.exists(symlink):
                os.symlink(model_dir, symlink)

            config_path = os.path.join(symlink, "config.intgemm8bitalpha.yml")
            bergamot_config_path = os.path.join(symlink, "config.bergamot.yml")

            # Finally patch so we don't have to reload this again.
            patch_marian_for_bergamot(config_path, bergamot_config_path)

            print("Done.")

    def _archive_name_without_extension(self, url: URL):
        o = urlparse(url)
        fname = os.path.basename(o.path)  # something tar.gz.
        fname_without_extension = fname.replace(".tar.gz", "")
        return fname_without_extension


APP = "lemonade"
appDir = AppDirs("lemonade")
repository = TranslateLocally(appDir)
