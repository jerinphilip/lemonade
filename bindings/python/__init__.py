import typing

try:
    from ._bergamot import *  # type: ignore
    from .repository import Aggregator, TranslateLocallyLike

    REPOSITORY = Aggregator(
        [
            TranslateLocallyLike(
                "browsermt", "https://translatelocally.com/models.json"
            ),
            TranslateLocallyLike(
                "opus", "https://object.pouta.csc.fi/OPUS-MT-models/app/models.json"
            ),
        ]
    )
except ImportError:
    raise
