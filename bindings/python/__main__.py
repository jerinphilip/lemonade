import argparse
import os
import sys
from argparse import ArgumentParser
from collections import Counter, defaultdict

from . import ResponseOptions, Service, ServiceConfig, VectorString
from .config import repository


def translate_fn(args):
    # Build service
    config = ServiceConfig(numWorkers=args.num_workers)
    service = Service(config)

    # Work with one model, loaded from config file

    models = [
        service.modelFromConfigPath(repository.modelConfigPath(model))
        for model in args.model
    ]

    # Configure a few options which require how a Response is constructed
    options = ResponseOptions(
        alignment=args.alignment, qualityScores=args.quality_scores, HTML=args.html
    )

    source = sys.stdin.read()

    responses = None
    if len(models) == 1:
        [model] = models
        responses = service.translate(model, VectorString([source]), options)
    else:
        [first, second] = models
        responses = service.pivot(first, second, VectorString([source]), options)

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
        nargs="+",
        help="Path to at max 2 model files to use in tag-transfer translation. 1 model implies only forward translation. If two models are provided",
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
        if len(args.model) > 2:
            print("Error, more than two models specified.", file=sys.stderr)
            parser.print_help(sys.stderr)
            sys.exit(1)

        translate_fn(args)
    else:
        parser.print_help(sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
