#!/usr/bin/env python3 
import requests
from argparse import ArgumentParser
import typing
import json
import yaml
from dataclasses import dataclass
import os
import tarfile
from urllib.parse import urlparse

APP = "lemonade"
HOME = os.environ.get("HOME")
CACHE_DIR = os.environ.get("XDG_CACHE_HOME", os.path.join(HOME, ".cache"))
CONFIG_DIR = os.environ.get("XDG_CONFIG_HOME", os.path.join(HOME, ".config"))

@dataclass
class Config:
    url : str = "https://translatelocally.com/models.json"
    cache_dir: str = os.path.join(CACHE_DIR, APP)
    config_dir: str = os.path.join(CONFIG_DIR, APP)
    models_file: str = os.path.join(CONFIG_DIR, APP, "models.json")
    data_dir: str = os.path.join(HOME, ".{}".format(APP))
    archive_dir: str = os.path.join(HOME, ".{}".format(APP), "archives")
    models_dir: str = os.path.join(HOME, ".{}".format(APP), "models")

config = Config()

def create_required_dirs(config):
    os.makedirs(config.cache_dir, exist_ok=True)
    os.makedirs(config.config_dir, exist_ok=True)
    os.makedirs(config.data_dir, exist_ok=True)
    os.makedirs(config.models_dir, exist_ok=True)
    os.makedirs(config.archive_dir, exist_ok=True)

def get_inventory(url, save_location, force_download=False):
    def get_inventory_fresh(url):
        response = requests.get(url)
        return response.text

    def write_inventory_to_file(inventory, save_location):
        with open(save_location, "w+") as models_file:
            models_file.write(inventory)

    if force_download or not os.path.exists(save_location):
        inventory = get_inventory_fresh(url)
        write_inventory_to_file(inventory, save_location)
        return json.loads(inventory)
    else:
        with open(save_location) as models_file:
            data = json.load(models_file)
            return data


def download_archive(url, save_location, force_download=False):
    if force_download or not os.path.exists(save_location):
        response = requests.get(url, stream=True)
        # Throw an error for bad status codes
        response.raise_for_status()
        with open(save_location, 'wb') as handle:
            for block in response.iter_content(1024):
                handle.write(block)


def hardCodeFpaths(url):
    o = urlparse(url)
    fname = os.path.basename(o.path) # something tar.gz.
    fname_without_extension = fname.replace(".tar.gz", "")
    return fname_without_extension


def patch_marian_for_bergamot(fpath, output_path, quality=False):
    data = None
    with open(fpath) as fp:
        data = yaml.load(fp, Loader=yaml.FullLoader)

    data.update({
        'ssplit-prefix-file': '',
        'ssplit-mode': 'sentence',
        'max-length-break': 128,
        'mini-batch-words': 1024,
        'workspace': 128, # shipped models use big workspaces. We'd prefer to keep it low.
    })

    if quality:
        data.update({
            'quality': args.quality, 
            'skip-cost': False
        })

    with open(output_path, 'w') as ofp:
        print(yaml.dump(data, sort_keys=False), file=ofp)



if __name__ == '__main__':
    parser = ArgumentParser("Model manager to download models for bergamot")
    create_required_dirs(config)
    data = get_inventory(config.url, config.models_file)
    for model in data["models"]:
        model_archive ='{}.tar.gz'.format(model["shortName"])
        save_location = os.path.join(config.archive_dir, model_archive)
        download_archive(model["url"], save_location)
        fprefix = hardCodeFpaths(model["url"])
        with tarfile.open(save_location) as model_archive:
            model_archive.extractall(config.models_dir)
            model_dir = os.path.join(config.models_dir, fprefix)
            link = os.path.join(config.models_dir, model["code"])
            if not os.path.exists(link):
                os.symlink(model_dir, link)

            config_path = os.path.join(link, "config.intgemm8bitalpha.yml")
            bergamot_config_path = os.path.join(link, "config.bergamot.yml")
            patch_marian_for_bergamot(config_path, bergamot_config_path)
            print(model, model_dir, link)

