import argparse
import sys
from collections import Counter, defaultdict

from . import ResponseOptions, Service, ServiceConfig, VectorString
from .config import repository

CMDS = {}


def _register_cmd(cmd: str):
    """
    Convenience decorator function, which populates the dictionary above with
    commands created in a declarative fashion.
    """

    def __inner(cls):
        CMDS[cmd] = cls
        return cls

    return __inner


@_register_cmd("translate")
class Translate:
    @staticmethod
    def embed_subparser(key: str, subparsers: argparse._SubParsersAction):
        translate = subparsers.add_parser(key)
        translate.add_argument(
            "-m",
            "--model",
            type=str,
            nargs="+",
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

    @staticmethod
    def execute(args: argparse.Namespace):
        # Build service

        config = ServiceConfig(numWorkers=args.num_workers)
        service = Service(config)

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


@_register_cmd("download")
class Download:
    @staticmethod
    def embed_subparser(key: str, subparsers: argparse._SubParsersAction):
        download = subparsers.add_parser(key)
        download.add_argument(
            "-m",
            "--model",
            type=str,
            required=False,
            default=None,
            help="Fetch model with given code. Use ls to list available models. Optional, if none supplied all models are downloaded.",
        )

    @staticmethod
    def execute(args: argparse.Namespace):
        if args.model is not None:
            repository.download(args.model)
        else:
            for model in repository.models(filter_downloaded=False):
                repository.download(model)


@_register_cmd("ls")
class List:
    @staticmethod
    def embed_subparser(key: str, subparsers: argparse._SubParsersAction):
        ls = subparsers.add_parser(key)

    @staticmethod
    def execute(args: argparse.Namespace):
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
