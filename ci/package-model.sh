#!/usr/bin/env bash
# Package an installed simulator for a matching Ubuntu x86-64 host.
set -euo pipefail

install_dir=${1:?usage: package-model.sh SDE_INSTALL OUTPUT_DIR}
output=$(realpath -m "${2:?missing output directory}")
repo=$(cd "$(dirname "$0")/.." && pwd)
version=$(cat "$repo/Version.txt")
name="tofino-model-v${version}-ubuntu24.04-amd64"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

mkdir -p "$work/$name"/{bin,lib} "$output"
install -m 755 "$install_dir/bin/tofino-model" "$work/$name/bin/"
install -m 644 "$install_dir/lib/libcli.so.1.10" "$work/$name/lib/"
patchelf --set-rpath "\$ORIGIN/../lib" "$work/$name/bin/tofino-model"
cp "$repo"/{LICENSE,ThirdPartyNotice.txt,Version.txt} "$work/$name/"
tar -C "$work" -czf "$output/$name.tar.gz" "$name"
(cd "$output" && sha256sum "$name.tar.gz" > "$name.tar.gz.sha256")
