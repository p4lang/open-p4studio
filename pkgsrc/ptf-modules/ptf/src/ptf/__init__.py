"""Docstring to silence pylint; ignores --ignore option for __init__.py"""

import sys
import os
import logging

try:
    from ._version import __version__
except ImportError:
    # the generated _version.py file should not be checked-in
    # if it is missing, we set the version string to "unknown"
    __version__ = "unknown"

# Global config dictionary
# Populated by oft.
config = {}

# Global DataPlane instance used by all tests.
# Populated by oft.
dataplane_instance = None


def open_logfile(name):
    """
    (Re)open logfile

    When using a log directory a new logfile is created for each test. The same
    code is used to implement a single logfile in the absence of --log-dir.
    """

    _format = "%(asctime)s.%(msecs)03d  %(name)-10s: %(levelname)-8s: %(message)s"
    _datefmt = "%H:%M:%S"

    if config["log_dir"] != None:
        filename = os.path.join(config["log_dir"], name) + ".log"
    else:
        filename = config["log_file"]

    logger = logging.getLogger()

    # Remove any existing handlers
    for handler in list(logger.handlers):
        logger.removeHandler(handler)
        handler.close()

    formatter = logging.Formatter(_format, _datefmt)

    # Add a new handler
    handler = logging.FileHandler(filename, mode="a")
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    # We log all ERROR and CRITICAL messages to stdout as well as to the
    # logfile.
    stream_handler = logging.StreamHandler()
    stream_handler.setLevel(logging.ERROR)
    stream_handler.setFormatter(formatter)
    logger.addHandler(stream_handler)


def disable_logging():
    """
    Temporarily disable all logging by setting the global log level to
    CRITICAL, which is the highest log level in use.
    """
    logging.disable(logging.CRITICAL)


def enable_logging():
    """
    Turn logging back on after a call to disable_logging().
    """
    logging.disable(logging.NOTSET)
