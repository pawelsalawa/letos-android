#!/usr/bin/env bash
set -euo pipefail

# Usage: ./compile_mc.sh [arch1,arch2,...]
# Default: armeabi-v7a,arm64-v8a,x86,x86_64
# Requires ANDROID_NDK_ROOT or ANDROID_NDK_HOME to be set and a working NDK with clang toolchain

LOCAL_ANDROID_NDK_ROOT="$HOME/Android/Sdk/ndk/30.0.16138531"

NDK="${ANDROID_NDK_ROOT:-${ANDROID_NDK_HOME:-$LOCAL_ANDROID_NDK_ROOT}}"
if [ -z "$NDK" ]; then
  echo "Please set ANDROID_NDK_ROOT or ANDROID_NDK_HOME to your Android NDK path." >&2
  exit 2
fi

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
AMALG_DIR="$ROOT_DIR/sqlite3mc/amalgamation"
OUT_DIR="$ROOT_DIR/sqlite3mc/lib"

if [ ! -d "$AMALG_DIR" ]; then
  echo "Amalgamation directory not found: $AMALG_DIR" >&2
  echo "Run ./update_sqlite3mc.sh first." >&2
  exit 3
fi

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

DEFAULT_ARCHS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")
if [ -n "${1:-}" ]; then
  IFS=',' read -r -a ARCHS <<< "$1"
else
  ARCHS=("${DEFAULT_ARCHS[@]}")
fi

# API levels: choose reasonable minimums
API_ARM=30
API_ARM64=30
API_X86=30
API_X86_64=30

# Common compile flags (tweak as needed)
CFLAGS="-O2 \
        -fPIC \
        -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT \
        -DSQLITE_ENABLE_DBSTAT_VTAB \
        -DSQLITE_ENABLE_BYTECODE_VTAB \
        -DSQLITE_ENABLE_COLUMN_METADATA \
        -DSQLITE_ENABLE_EXPLAIN_COMMENTS \
        -DSQLITE_ENABLE_FTS3 \
        -DSQLITE_ENABLE_FTS4 \
        -DSQLITE_ENABLE_FTS5 \
        -DSQLITE_ENABLE_GEOPOLY \
        -DSQLITE_ENABLE_JSON1 \
        -DSQLITE_ENABLE_RTREE \
        -DSQLITE_ENABLE_MATH_FUNCTIONS \
        -DSQLITE_ENABLE_PERCENTILE \
        -DSQLITE_ENABLE_ORDERED_SET_AGGREGATES \
        -DSQLITE_DQS=1 \
        -DSQLITE_THREADSAFE=1"
LDFLAGS="-shared -Wl,-soname,libsqliteX.so"

STRIP="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip"
for arch in "${ARCHS[@]}"; do
  echo "Building for arch: $arch"
  case "$arch" in
    armeabi-v7a)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi${API_ARM}-clang"
      ;;
    arm64-v8a)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android${API_ARM64}-clang"
      ;;
    x86)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android${API_X86}-clang"
      ;;
    x86_64)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android${API_X86_64}-clang"
      ;;
    *)
      echo "Unsupported arch: $arch" >&2
      exit 4
      ;;
  esac

  if [ ! -x "$CC" ]; then
    echo "Compiler not found or not executable: $CC" >&2
    exit 5
  fi

  BUILD_DIR="$(mktemp -d)"
  cp -r "$AMALG_DIR"/* "$BUILD_DIR"/
  pushd "$BUILD_DIR" >/dev/null

  echo "Compiling sqlite3.c -> sqlite3.o"
  "$CC" $CFLAGS -I. -c sqlite3mc_amalgamation.c -o sqlite3.o

  echo "Linking libsqliteX.so"
  "$CC" $LDFLAGS sqlite3.o -o libsqliteX.so
  "$STRIP" libsqliteX.so

  mkdir -p "$OUT_DIR/$arch"
  mv libsqliteX.so "$OUT_DIR/$arch/"

  popd >/dev/null
  rm -rf "$BUILD_DIR"
  echo "Built $OUT_DIR/$arch/libsqliteX.so"
done

echo "All builds finished. Libraries placed in $OUT_DIR"

# If an AAR was downloaded by update_mc.sh, replace its JNI libs with the freshly built ones
AAR_GLOB=("$ROOT_DIR/sqlite3mc"/*.aar)
if [ -e "${AAR_GLOB[0]:-}" ]; then
  AAR_PATH="${AAR_GLOB[0]}"
  echo "Found AAR to patch: $AAR_PATH"
  PATCH_DIR="$(mktemp -d)"
  unzip -q "$AAR_PATH" -d "$PATCH_DIR"

  for arch in "${ARCHS[@]}"; do
    DEST_DIR="$PATCH_DIR/jni/$arch"
    mkdir -p "$DEST_DIR"
    if [ -f "$OUT_DIR/$arch/libsqliteX.so" ]; then
      echo "Replacing JNI lib for $arch"
      # remove existing .so files for this library name (be conservative: remove existing libs)
      rm -f "$DEST_DIR"/*.so || true
      cp "$OUT_DIR/$arch/libsqliteX.so" "$DEST_DIR/"
    else
      echo "Warning: compiled lib for $arch not found, skipping" >&2
    fi
  done

  # Repack AAR (preserve original name)
  NEW_AAR="${AAR_PATH}.patched"
  (cd "$PATCH_DIR" && zip -q -r "$NEW_AAR" .)
  if [ -f "$NEW_AAR" ]; then
    mv -f "$NEW_AAR" "$AAR_PATH"
    echo "Patched AAR written to $AAR_PATH"
  else
    echo "Failed to create patched AAR" >&2
  fi
  rm -rf "$PATCH_DIR"
else
  echo "No AAR found in $ROOT_DIR/sqlite3mc; skipping AAR patching step"
fi