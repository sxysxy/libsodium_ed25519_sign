#!/bin/sh
set -eu

MODE="${1:-all}"

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
DIST_DIR="$SCRIPT_DIR/dist"
WEB_DIST_DIR="$DIST_DIR/web"

build_native() {
    cc -std=c99 -O2 -Wall -Wextra \
      "$SCRIPT_DIR/sign_file.c" "$SCRIPT_DIR/ed25519_sign.c" \
      -o "$SCRIPT_DIR/sign_file"

    cc -std=c99 -O2 -Wall -Wextra \
      "$SCRIPT_DIR/verify_file.c" "$SCRIPT_DIR/ed25519_sign.c" \
      -o "$SCRIPT_DIR/verify_file"

    echo "native build complete:"
    echo "  $SCRIPT_DIR/sign_file"
    echo "  $SCRIPT_DIR/verify_file"
}

build_wasm() {
    if ! command -v emcc >/dev/null 2>&1; then
        echo "emcc not found, skip wasm build" >&2
        echo "install Emscripten first: https://emscripten.org/docs/getting_started/downloads.html" >&2
        return 1
    fi

    mkdir -p "$WEB_DIST_DIR"

    emcc -O3 -Wall -Wextra \
      "$SCRIPT_DIR/web/ed25519_wasm.c" "$SCRIPT_DIR/ed25519_sign.c" \
      -o "$WEB_DIST_DIR/ed25519_wasm.js" \
      -s WASM=1 \
      -s ALLOW_MEMORY_GROWTH=1 \
      -s ENVIRONMENT=web \
      -s EXPORTED_FUNCTIONS='["_malloc","_free","_wasm_sign_hex","_wasm_verify_hex"]' \
      -s EXPORTED_RUNTIME_METHODS='["UTF8ToString","stringToUTF8","lengthBytesUTF8","HEAPU8"]'

    cp "$SCRIPT_DIR/web/index.html" "$WEB_DIST_DIR/index.html"

    echo "wasm build complete:"
    echo "  $WEB_DIST_DIR/index.html"
    echo "  $WEB_DIST_DIR/ed25519_wasm.js"
    echo "  $WEB_DIST_DIR/ed25519_wasm.wasm"
}

case "$MODE" in
    cc|native)
        build_native
        ;;
    wasm|webasm)
        build_wasm
        ;;
    all)
        build_native
        if ! build_wasm; then
            echo "wasm build skipped" >&2
        fi
        ;;
    *)
        echo "Usage: $0 [cc|wasm|all]" >&2
        exit 1
        ;;
esac
