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

    model = build_model(service, ende)

    inputs = pybergamot.VectorString()
    inputs.append("Hello World!")
    inputs.append("Goodbye World!")

    responses = service.translate(model, inputs, options)

    for response in responses:
        print('[src] > ', response.source.text)
        print('[tgt] > ', response.target.text)

