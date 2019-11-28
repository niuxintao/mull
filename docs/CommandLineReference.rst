======================
Command Line Reference
======================

.. contents::
   :local:


mull-cxx
========

Cache Control
-------------

In order to speedup execution Mull saves compilation results across subsequent runs.
The following options control the cache behaviour.

``--cache-dir``
^^^^^^^^^^^^^^^

    This option controls where Mull stores cached data such as compiled object files.
    If the directory does not exists, Mull attempts to create one.

    Defaults to ``/tmp/cache``.

    Examples:

    .. code:: bash

        mull-cxx --cache-dir=$PWD/cache ...
        mull-cxx --cache-dir /tmp/cache ...


``--disable-cache``
^^^^^^^^^^^^^^^^^^^

    Tells Mull whether it should use cache or not.

    Enabled by default.

    Examples:

    .. code:: bash

        mull-cxx --disable-cache ...
        mull-cxx --disable-cache=false ...


Compilation Flags
-----------------

``--compdb-path``
^^^^^^^^^^^^^^^^^



``--compilation-flags``
^^^^^^^^^^^^^^^^^^^^^^^

