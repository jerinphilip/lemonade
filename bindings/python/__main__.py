import argparse
import os
import sys
from argparse import ArgumentParser
from collections import Counter, defaultdict

from ._bergamot import ResponseOptions, Service, ServiceConfig, VectorString
from .config import repository


def translate_fn(args):
    # Build service
    config = ServiceConfig(numWorkers=args.num_workers)
    service = Service(config)

    # Work with one model, loaded from config file
    model_config = repository.modelConfigPath(args.model)
    model = service.modelFromConfigPath(model_config)

    # Configure a few options which require how a Response is constructed
    options = ResponseOptions(
        alignment=args.alignment, qualityScores=args.quality_scores, HTML=args.html
    )

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

    # Tweak response-options for quick HTML in out via commandline
    options = translate.add_argument_group("response-options")
    options.add_argument("--html", type=bool, default=False)
    options.add_argument("--alignment", type=bool, default=False)
    options.add_argument("--quality-scores", type=bool, default=False)

    args = parser.parse_args()

    if args.action == "download":
        for model in repository.models(filter_downloaded=False):
            repository.download(model)
    elif args.action == "ls":
        print("Available models: ")
        for counter, identifier in enumerate(
            repository.models(filter_downloaded=True), 1
        ):
            model = repository.model(identifier)
            print(
                " {}.".format(str(counter).rjust(4)),
                model["code"],
                model["name"],
            )
        print()

    elif args.action == "translate":
        translate_fn(args)
    else:
        parser.print_help(sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
