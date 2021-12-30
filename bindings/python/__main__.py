import os
import sys
import argparse
from collections import Counter
from collections import defaultdict
from argparse import ArgumentParser
from ._bergamot import Service, ResponseOptions, ServiceConfig, VectorString
from .config import Config, get_inventory, hardCodeFpaths


if __name__ == "__main__":
    parser = ArgumentParser("Translate a sample blob of text in python")
    parser.add_argument(
        "--model-code",
        type=str,
        help="Path to model file to use in tag-transfer translation",
        required=True,
    )
    parser.add_argument(
        "--num-workers",
        type=int,
        help="Number of worker threads to use to translate",
        default=4,
    )
    parser.add_argument(
        "--cache-size",
        type=int,
        help="How many sentences to hold in cache",
        default=2000,
    )
    parser.add_argument(
        "--cache-mutex-buckets",
        type=int,
        help="How many mutex buckets to use to reduce contention in cache among workers",
        default=20,
    )

    args = parser.parse_args()

    # Create config
    config = ServiceConfig()
    config.numWorkers = args.num_workers
    config.cacheSize = args.cache_size
    config.cacheMutexBuckets = args.cache_mutex_buckets

    # Build service
    service = Service(config)

    # Work with one model, loaded from config file
    appConfig = Config()
    inventory = get_inventory(appConfig.url, appConfig.models_file)
    model_config = {entry["code"]: entry for entry in inventory["models"]}
    model_entry = model_config[args.model_code]
    fprefix = hardCodeFpaths(model_entry["url"])
    model_dir = os.path.join(appConfig.models_dir, fprefix)
    model_config = os.path.join(model_dir, "config.bergamot.yml")

    model = service.modelFromConfigPath(model_config)

    # Configure a few options which require how a Response is constructed
    options = ResponseOptions()
    options.alignment = True
    options.qualityScores = True

    source = sys.stdin.read()
    responses = service.translate(model, VectorString([source]), options)

    for response in responses:
        print(response.target.text)
