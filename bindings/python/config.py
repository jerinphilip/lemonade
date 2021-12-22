from dataclasses import dataclass
from appdirs import AppDirs
import json
import os
from urllib.parse import urlparse

APP = "lemonade"
appDir = AppDirs("lemonade")


@dataclass
class Config:
    url : str = "https://translatelocally.com/models.json"
    cache_dir: str = appDir.user_cache_dir
    config_dir: str = appDir.user_config_dir
    models_file: str = os.path.join(appDir.user_config_dir, "models.json")
    data_dir: str = appDir.user_data_dir
    archive_dir: str = os.path.join(appDir.user_data_dir, "archives")
    models_dir: str = os.path.join(appDir.user_data_dir, "models")

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

def hardCodeFpaths(url):
    o = urlparse(url)
    fname = os.path.basename(o.path) # something tar.gz.
    fname_without_extension = fname.replace(".tar.gz", "")
    return fname_without_extension

def listModels(config):
    data = get_inventory(config.url, config.models_file)
    print("The following models are available:\n")
    counter = 0
    for model in data["models"]:
        fprefix = hardCodeFpaths(model["url"])
        model_dir = os.path.join(config.models_dir, fprefix)
        if (os.path.exists(model_dir)):
            counter += 1
            print(' {}.'.format(str(counter).rjust(4)), model["code"], model["name"])
    print()
