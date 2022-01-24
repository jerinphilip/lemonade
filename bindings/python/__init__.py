import typing

try:
    from ._bergamot import *  # type: ignore
    from .repository import Aggregator, TranslateLocallyLike

    repository: typing.Annotated[
        repository,
        "Object aggregating multiple model providers to provide an API to query and get models by (repository-name, model-code)",
    ] = Aggregator(
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
