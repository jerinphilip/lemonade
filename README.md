# lemonade

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) 
[![build/ubuntu-latest](https://github.com/jerinphilip/lemonade/actions/workflows/main.yml/badge.svg)](./.github/workflows/main.yml)

**Disclaimer** This repository is experimental and there is currently no
promise of stability on API. 

Lemonade showcases applications and extensions to
[bergamot-translator](https://github.com/browsermt/bergamot-translator) outside
the [official deliverables](https://browser.mt/deliverables). 

For using a C++ library which is cross-platfrom, by
[bergamot-translator](https://github.com/browsermt/bergamot-translator).

If you're interested a GUI local translator with more stability, Please
checkout [translateLocally](https://github.com/XapaJIaMnu/translateLocally).



## Dependencies

- Qt: Required for liblemonade (CPP) to identify standard-folders on \*nix.
- libarchive: Required for downloading and extracting archives via liblemonade.
- pybind11: Required for pybindings.

Many of the above dependencies can be isolated to pybindings or iBus engine or
liblemonade to minimize dependencies for a particular component. Contributions
are welcome.

## Available features

### Python Bindings

You can find the minimal python-bindings to create a local-translation service
in [bindings/python](./bindings/python) folder. 
It is recommended to use a virtual-environment for installation. To install:

```bash
python3 setup.py bdist_wheel
python3 -m pip -U install --force-reinstall dist/bergamot-*.whl
```

In addition to bindings to the library which you may use for-your use-cases, a
command-line interface is provided along with an entry-point which you may use
to download models, and translate input text.

```bash
$ bergamot download # download models from the web.

# translate from stdin
$ echo "Hello World." | bergamot translate -m en-de-tiny  
Hallo Welt.

```

It is expected that pybindings will be merged into bergamot-translator
eventually when mature.

#### Python bindings on macOS
Assuming you have Qt installed through the official Qt distribution, it would
be something this, where you might need to change the path to your version of
Qt.

```sh
CMAKE_ARGS="-DCMAKE_PREFIX_PATH=$HOME/Qt/6.2.2/macos;$(brew --prefix pybind11)" pip3 install path/to/lemonade
```

### iBus Engine

[iBus](https://en.wikipedia.org/wiki/Intelligent_Input_Bus) provides a method
to hijack the keyboard-input and insert modified text into any graphical
application. The author believes doing this will be superior to a web-extension
due to:

1. minimal interference with page HTML unlike a web-extension and
2. wider-coverage across all GUI applications (any box where text can be entered).

Find a demonstration of iBus in action on LibreOffice below:

![iBus](https://user-images.githubusercontent.com/727292/147791520-b3732b87-a142-4f95-b1a9-75e49e9488df.gif)

## Acknowledgements

This project was made possible through the combined effort of all researchers
and [partners](https://browser.mt/partners/) in the Bergamot project . The
[translation models](https://github.com/browsermt/students) are prepared as
part of the Bergamot project. The translation engine used is
[bergamot-translator](https://github.com/browsermt/bergamot-translator) which
is based on [marian](https://github.com/marian-nmt/marian-dev).

