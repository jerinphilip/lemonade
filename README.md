# lemonade

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html) 
[![build/ubuntu-latest](https://github.com/jerinphilip/lemonade/actions/workflows/main.yml/badge.svg)](./.github/workflows/main.yml)


Abuses [iBus](https://en.wikipedia.org/wiki/Intelligent_Input_Bus) to hijack
text entered into a field by a user, to insert translated text into any
graphical application. 

This allows you to enter text in a language you know, while the field gets the
translated text. Useful when interacting with websites or agents in a foreign
language.

Find a demonstration of iBus in action on LibreOffice below:

<img src="https://user-images.githubusercontent.com/727292/147887982-690f5a65-ad8c-4743-8035-56f7e4f4a6b2.gif" width=720 alt="iBus translation in action"/>

The functionality will work in any GUI application which requests
keyboard-input - browser, text-editors, mail-clients, chat clients etc.

## Setup

Setup currently requires some technical expertise. There are setup-instructions
[here](https://github.com/jerinphilip/lemonade/wiki/Setting-Up-iBus). 

Please contact via email if need further support and/or you can help improve by
contributing.

* jerinphilip [at] live.in


## History 

This repository was formerly a sandbox used to build proof-of-concept
applications of
[bergamot-translator](https://github.com/browsermt/bergamot-translator) outside
the [official deliverables](https://browser.mt/deliverables). Python bindings
have been moved to upstream bergamot-translator. What remains here is
ibus-engine.

See also: 

* [bergamot-translator](https://github.com/browsermt/bergamot-translator)
* [translateLocally](https://github.com/XapaJIaMnu/translateLocally)

## Acknowledgements

This application was made possible through the combined effort of all
researchers and [partners](https://browser.mt/partners/) in the Bergamot
project. The [translation models](https://github.com/browsermt/students) are
prepared as part of the Bergamot project. The translation engine used is
[slimt](https://github.com/jerinphilip/slimt/) which is an inference slice
using only one class of models within is based on
[marian](https://github.com/marian-nmt/marian-dev).

