import argparse
import sys
from argparse import ArgumentParser

from .cmds import CMDS


def main() -> None:
    parser = ArgumentParser("bergamot")
    subparsers = parser.add_subparsers(
        title="actions",
        description="The following actions are available through the bergamot package",
        help="To obtain help on how to run these actions supply <cmd> -h.",
        dest="action",
    )

    for key, cls in CMDS.items():
        cls.embed_subparser(key, subparsers)

    args = parser.parse_args()

    if args.action in CMDS:
        CMDS[args.action].execute(args)
    else:
        parser.print_help(sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
