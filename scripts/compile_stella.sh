#!/usr/bin/env bash

set -euo pipefail

IMAGE="fizruk/stella"
DIGEST="sha256:86d6123aff226787a02b1b8021c7d4f2cd3703586155f5a96d6f0a7c21d805c6"

dir="$1"

shopt -s nullglob
stella_files=("$dir"/*.stella)

if [ ${#stella_files[@]} -eq 0 ]; then
  echo "No .stella files found in $dir"
  exit 0
fi

compile() {
  local src="$1"
  local dst="${src%.stella}.c"

  echo "=== Compiling $(basename "$src") -> $(basename "$dst") ==="

  docker run --rm -i \
    "$IMAGE@$DIGEST" \
    compile /dev/stdin -o /dev/stdout \
    < "$src" > "$dst"
}

for file in "${stella_files[@]}"; do
  compile "$file"
done