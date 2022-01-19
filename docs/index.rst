.. Bergamot documentation master file, created by
   sphinx-quickstart on Tue Jan 18 17:26:57 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Documentation: bergamot-translator
====================================

.. toctree::
   :maxdepth: 3
   :caption: Contents:


Command Line Interface
----------------------

.. argparse::
   :ref: bergamot.cmds.make_parser
   :prog: bergamot


Module Documentation
--------------------

.. automodule:: bergamot
   :members:
   :undoc-members:
 
C++ Exports
++++++++++++

.. autoclass:: bergamot.ServiceConfig
   :members:
   :undoc-members:

.. autoclass:: bergamot.Service
   :members:
   :undoc-members:


.. autoclass:: bergamot.TranslationModel
   :members:
   :undoc-members:

.. autoclass:: bergamot.ResponseOptions
   :members:
   :undoc-members:

Pure Python 
+++++++++++

.. autoclass:: bergamot.config.Repository
   :members:
   :undoc-members:

.. autoclass:: bergamot.config.TranslateLocally
   :members:
   :undoc-members:

.. autofunction:: bergamot.config.patch_marian_for_bergamot



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
