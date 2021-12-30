#!/usr/bin/env python3
import requests
from argparse import ArgumentParser
import typing
import yaml
import os
import tarfile
from .config import (
    Config,
    create_required_dirs,
    get_inventory,
    hardCodeFpaths,
    listModels,
)


def download_archive(url, save_location, force_download=False):
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

    with open(output_path, "w") as ofp:
        print(yaml.dump(data, sort_keys=False), file=ofp)


def download(config):
    create_required_dirs(config)
    print("Getting inventory from {}....".format(config.url), end="")
    data = get_inventory(config.url, config.models_file)
    print("Done.")
    for model in data["models"]:
        model_archive = "{}.tar.gz".format(model["shortName"])
        save_location = os.path.join(config.archive_dir, model_archive)
        download_archive(model["url"], save_location)
        fprefix = hardCodeFpaths(model["url"])
        with tarfile.open(save_location) as model_archive:
            model_archive.extractall(config.models_dir)
            model_dir = os.path.join(config.models_dir, fprefix)
            link = os.path.join(config.models_dir, model["code"])
            print(
                "Downloading and extracting {} into ...{}".format(
                    model["code"], model_dir
                ),
                end=" ",
            )

            if not os.path.exists(link):
                os.symlink(model_dir, link)

            config_path = os.path.join(link, "config.intgemm8bitalpha.yml")
            bergamot_config_path = os.path.join(link, "config.bergamot.yml")
            patch_marian_for_bergamot(config_path, bergamot_config_path)
            print("Done.")

