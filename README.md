# lemonade

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) 
[![build/ubuntu-latest](https://github.com/jerinphilip/lemonade/actions/workflows/main.yml/badge.svg)](./.github/workflows/main.yml)

**Disclaimer** This repository is experimental and there is currently no
promise of stability on API. 

Lemonade showcases applications and extensions to
[bergamot-translator](https://github.com/browsermt/bergamot-translator) outside
the [official deliverables](https://browser.mt/deliverables). Some of these
undertakings outside official commitments have found eventual application 
as development or evaluation aides - take a look at
https://github.com/jerinphilip/tagtransfer for example.

If you're interested in using a more stable C++ library which is
cross-platfrom, head over-to [bergamot-translator](https://github.com/browsermt/bergamot-translator). If
you're interested a GUI local translator based on bergamot-translator, please
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

Python bindings have been merged to
[bergamot-translator](https://github.com/browsermt/bergamot-translator/pull/310).


### iBus Engine

[iBus](https://en.wikipedia.org/wiki/Intelligent_Input_Bus) provides a method
to hijack the keyboard-input and insert modified text into any graphical
application. The author believes doing this will be superior to a web-extension for 
outbound translation due to:

1. minimal interference with page HTML unlike a web-extension and
2. wider-coverage across all GUI applications (any box where text can be entered).

Find a demonstration of iBus in action on LibreOffice below:

<img src="https://user-images.githubusercontent.com/727292/147887982-690f5a65-ad8c-4743-8035-56f7e4f4a6b2.gif" width=720 alt="iBus translation in action"/>

## Acknowledgements

This project was made possible through the combined effort of all researchers
and [partners](https://browser.mt/partners/) in the Bergamot project. The
[translation models](https://github.com/browsermt/students) are prepared as
part of the Bergamot project. The translation engine used is
[bergamot-translator](https://github.com/browsermt/bergamot-translator) which
is based on [marian](https://github.com/marian-nmt/marian-dev).

