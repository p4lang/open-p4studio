""" A pluggable packet module

This module dynamically imports definitions from packet manipulation module,
specified in config or provided as an agrument.
The default one is Scapy, but one can develop its own packet manipulation framework and
then, create an implementation of packet module for it (for Scapy it is packet_scapy.py)
"""

from ptf import config

__module = __import__(
    config.get("packet_manipulation_module", "ptf.packet_scapy"), fromlist=["*"]
)
__keys = []

# import logic - everything from __all__ if provided, otherwise everything not starting
# with underscore
print("Using packet manipulation module: %s" % __module.__name__)
if "__all__" in __module.__dict__:
    __keys = __module.__dict__["__all__"]
else:
    __keys = [k for k in __module.__dict__ if not k.startswith("_")]

locals().update({k: getattr(__module, k) for k in __keys})
