import os
import sys
import argparse
from collections import Counter
from collections import defaultdict
from argparse import ArgumentParser
from ._bergamot import Service, ResponseOptions, ServiceConfig, VectorString
from .config import Config, get_inventory, hardCodeFpaths
from .pkgmgr import download, listModels


def translate_fn(args):
    config = ServiceConfig()
    config.numWorkers = args.num_workers

    # Build service
    service = Service(config)

    # Work with one model, loaded from config file
    appConfig = Config()
    inventory = get_inventory(appConfig.url, appConfig.models_file)
    model_config = {entry["code"]: entry for entry in inventory["models"]}
    model_entry = model_config[args.model]
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
        print(response.target.text, end="")


def main():
    parser = ArgumentParser("bergamot")
    subparsers = parser.add_subparsers(
        title="actions",
        description="The following actions are available through the bergamot package",
        help="To obtain help on how to run these actions supply <cmd> -h.",
        dest="action",
    )

    ls = subparsers.add_parser("ls")

    dl = subparsers.add_parser("download")
    dl.add_argument(
        "-m",
        "--model",
        type=str,
        required=False,
        default=None,
        help="Fetch model with given code. Use ls to list available models",
    )

    dl.add_argument("--all", type=bool, default=True)

    translate = subparsers.add_parser("translate")
    translate.add_argument(
        "-m",
        "--model",
        type=str,
        help="Path to model file to use in tag-transfer translation",
        required=True,
    )
    translate.add_argument(
        "--num-workers",
        type=int,
        help="Number of worker threads to use to translate",
        default=4,
    )

    args = parser.parse_args()
    config = Config()

    if args.action == "download":
        download(config)
    elif args.action == "ls":
        listModels(config)
    elif args.action == "translate":
        translate_fn(args)
    else:
        parser.print_help(sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
