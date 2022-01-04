from appdirs import AppDirs
import json
import yaml
import os
import typing as t
from abc import abstractmethod, ABC
from functools import partial
from urllib.parse import urlparse
import requests
import tarfile


def download_resource(url, save_location, force_download=False):
    if force_download or not os.path.exists(save_location):
        response = requests.get(url, stream=True)
        # Throw an error for bad status codes
        response.raise_for_status()
        with open(save_location, "wb") as handle:
            for block in response.iter_content(1024):
                handle.write(block)


def patch_marian_for_bergamot(fpath, output_path, quality=False):
    data = None
    with open(fpath) as fp:
        data = yaml.load(fp, Loader=yaml.FullLoader)

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
        data.update({"quality": args.quality, "skip-cost": False})

    with open(output_path, "w") as output_file:
        print(yaml.dump(data, sort_keys=False), file=output_file)


class Repository(ABC):
    """
    An interface for several repositories. We support translateLocally and
    Mozilla repositories for usage through python.
    """

    @abstractmethod
    def refresh(self):
        """Refreshes the repository"""
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
    def __init__(self, appDir):
        self.repository = "translateLocally"
        self.dirs = {
            "cache": os.path.join(appDir.user_cache_dir, self.repository),
            "config": os.path.join(appDir.user_config_dir, self.repository),
            "data": os.path.join(appDir.user_data_dir, self.repository),
            "archive": os.path.join(appDir.user_data_dir, self.repository, "archives"),
            "models": os.path.join(appDir.user_data_dir, self.repository, "models"),
        }

        for directory in self.dirs.values():
            os.makedirs(directory, exist_ok=True)

        self.models_file = os.path.join(self.dirs["config"], "models.json")
        self.data = self.refresh(force_download=True)

        self.data_by_code = {}
        for model in self.data["models"]:
            self.data_by_code[model["code"]] = model

    def refresh(self, force_download: bool = False) -> None:
        def get_inventory_fresh(url):
            response = requests.get(url)
            return response.text

        def write_inventory_to_file(inventory, save_location):
            with open(save_location, "w+") as models_file:
                models_file.write(inventory)

        url = "https://translatelocally.com/models.json"
        if force_download or not os.path.exists(self.models_file):
            inventory = get_inventory_fresh(url)
            write_inventory_to_file(inventory, self.models_file)
            return json.loads(inventory)
        else:
            with open(save_location) as models_file:
                data = json.load(models_file)
                return data

    def models(self, filter_downloaded: bool = True) -> t.List[str]:
        # data = get_inventory(config.url, config.models_file)
        # now self.data
        codes = []
        for model in self.data["models"]:
            if filter_downloaded:
                fprefix = self.hardCodeFpaths(model["url"])
                model_dir = os.path.join(self.dirs["models"], fprefix)
                if os.path.exists(model_dir):
                    codes.append(model["code"])
            else:
                codes.append(model["code"])
        return codes

    def modelConfigPath(self, model_identifier: str) -> str:
        model = self.modelEntry(model_identifier)
        fprefix = self.hardCodeFpaths(model["url"])
        model_dir = os.path.join(self.dirs["models"], fprefix)
        return os.path.join(model_dir, "config.bergamot.yml")

    def modelEntry(self, model_identifier: str) -> t.Any:
        return self.data_by_code[model_identifier]

    def hardCodeFpaths(self, url):
        o = urlparse(url)
        fname = os.path.basename(o.path)  # something tar.gz.
        fname_without_extension = fname.replace(".tar.gz", "")
        return fname_without_extension

    def download(self, model_identifier: str):
        # Download path
        model = self.modelEntry(model_identifier)
        model_archive = "{}.tar.gz".format(model["shortName"])
        save_location = os.path.join(self.dirs["archive"], model_archive)
        download_resource(model["url"], save_location)

        with tarfile.open(save_location) as model_archive:
            model_archive.extractall(self.dirs["models"])
            fprefix = self.hardCodeFpaths(model["url"])
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


APP = "lemonade"
appDir = AppDirs("lemonade")
repository = TranslateLocally(appDir)
