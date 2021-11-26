import yaml
import os
import argparse
import sys
from collections import Counter
from collections import defaultdict

sys.path.insert(0, "../build")

import pybergamot
from pybergamot import Service, Response, ResponseOptions, ServiceConfig, TranslationModel

def build_model(service, configPath):
    model = service.modelFromConfigPath(configPath)
    return model

def build_service():
    config = ServiceConfig()
    config.numWorkers = 4;
    config.cacheSize = 2000;
    config.cacheMutexBuckets = 18;
    return Service(config);


if __name__ == '__main__':
    ende = "/home/jerin/.local/share/lemonade/models/ende.student.tiny11/config.bergamot.yml"
    service = build_service()

    options = ResponseOptions();
    options.alignment = True
    options.qualityScores = True
    options.HTML = True

    model = build_model(service, ende)
    from examples import EXAMPLES

    for example in EXAMPLES:
        response = service.translate(model, example["input"], options)
        print('[src] > ', response.source.text)
        print('[hyp] > ', response.target.text)
        print('[tgt] > ', example["expectedProjectedString"])

