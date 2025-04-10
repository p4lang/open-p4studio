#!/usr/bin/env bash

[ -z $1 ] && echo "Error: Path to version file required as first argument" && exit 1

VERSION_FILE=$1

old_version=$( cat  ${VERSION_FILE}) ;
new_version=$$( @srcdir@/tools/build_generated_versions.sh @srcdir@ MODEL --match "model*" ) ; \
if [[ $${new_version} != $${old_version} ]]; then \
  echo Updating $@ ; \
  @srcdir@/tools/build_generated_versions.sh @srcdir@ MODEL --match "model*" > $@ ; \
fi
