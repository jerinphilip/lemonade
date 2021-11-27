import os
import sys
import argparse
from collections import Counter
from collections import defaultdict
from argparse import ArgumentParser

# Temporary, should eventually have proper packaging.
sys.path.insert(0, "../build")

import pybergamot
from pybergamot import Service, ResponseOptions, ServiceConfig


if __name__ == '__main__':
    parser = ArgumentParser("Translate a sample blob of text in python")
    parser.add_argument('--model-config', type=str, help="Path to model file to use in tag-transfer translation", required=True)
    parser.add_argument('--num-workers', type=int, help="Number of worker threads to use to translate", default=4)
    parser.add_argument('--cache-size', type=int, help="How many sentences to hold in cache", default=2000)
    parser.add_argument('--cache-mutex-buckets', type=int, help="How many mutex buckets to use to reduce contention in cache among workers", default=20)

    args = parser.parse_args()

    # Create config
    config = ServiceConfig()
    config.numWorkers = args.num_workers;
    config.cacheSize = args.cache_size;
    config.cacheMutexBuckets = args.cache_mutex_buckets;
    
    # Build service
    service = Service(config)

    # Work with one model, loaded from config file
    model = service.modelFromConfigPath(args.model_config)

    # Configure a few options which require how a Response is constructed
    options = ResponseOptions();
    options.alignment = True
    options.qualityScores = True

    inputs = [
        "Hello World!",
        "Goodbye World!"
    ]

    responses = service.translate(model, inputs, options)

    for response in responses:
        print('[src] > ', response.source.text)
        print('[tgt] > ', response.target.text)

