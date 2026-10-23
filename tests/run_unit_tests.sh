#!/usr/bin/env bash
set -euo pipefail

source "$HOME/zephyr-venv/bin/activate"

PROJECT="$HOME/embedded-health-monitor"
ZEPHYR_WORKSPACE="$HOME/zephyrproject"

echo "=== Building Zephyr unit tests ==="

cd "$ZEPHYR_WORKSPACE"

west build -p always \
    -b native_sim \
    "$PROJECT/tests/unit" \
    -d "$PROJECT/build-unit"

echo "=== Running unit tests ==="

west build -t run -d "$PROJECT/build-unit"

echo "=== Unit-test run completed successfully ==="
