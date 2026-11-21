# Ubuntu CI caches

`native-v1-` caches contain installed third-party dependencies. They require
an exact key match because their libraries, headers and build tools must agree.
They are saved before Python packages or the SDE are installed.
