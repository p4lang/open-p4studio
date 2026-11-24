# Ubuntu CI caches

The compiler cache follows [P4C's policy](https://github.com/p4lang/p4c/tree/bccb23cc10/.github/actions).
Each Ubuntu build restores the latest `main` snapshot for its compiler and
build settings. Pull requests overlay their own changes and save up to 200 MB
of changed entries after a successful build. Only `main` saves a full snapshot
(up to 2 GB per Ubuntu version). Unchanged builds skip the upload, and merge
queue jobs only restore caches.

The daily **Cache maintenance** workflow removes superseded snapshots and
closed-PR entries. It targets 2 GB of PR caches and 9 GB total, counting other
repository caches towards the total but deleting only `ccache-v2-` entries.
Use its manual `dry-run` option to preview deletions.
