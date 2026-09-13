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
LETOS_ROOT="$ROOT_DIR/LetosRemoteProject"
AMALG_DIR="$LETOS_ROOT/sqlite3mc/amalgamation"
SQLITE3MC_DIR="$LETOS_ROOT/sqlite3mc"
OUT_DIR="$LETOS_ROOT/sqlite3mc/lib"

if [ ! -d "$AMALG_DIR" ]; then
  echo "Amalgamation directory not found: $AMALG_DIR" >&2
  echo "Run ./update_mc.sh first." >&2
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
CXX_STDLIB_FLAGS="-static-libstdc++"
ANDROID_LIBS="-llog"

STRIP="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip"
for arch in "${ARCHS[@]}"; do
  echo "Building for arch: $arch"
  case "$arch" in
    armeabi-v7a)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi${API_ARM}-clang"
      CXX="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/armv7a-linux-androideabi${API_ARM}-clang++"
      ;;
    arm64-v8a)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android${API_ARM64}-clang"
      CXX="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android${API_ARM64}-clang++"
      ;;
    x86)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android${API_X86}-clang"
      CXX="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/i686-linux-android${API_X86}-clang++"
      ;;
    x86_64)
      CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android${API_X86_64}-clang"
      CXX="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/x86_64-linux-android${API_X86_64}-clang++"
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
  if [ ! -x "$CXX" ]; then
    echo "C++ compiler not found or not executable: $CXX" >&2
    exit 6
  fi

  BUILD_DIR="$(mktemp -d)"
  cp -R "$SQLITE3MC_DIR"/. "$BUILD_DIR"/
  pushd "$BUILD_DIR" >/dev/null
  INCLUDE_FLAGS=(
    -I"$BUILD_DIR"
    -I"$BUILD_DIR/amalgamation"
    -I"$BUILD_DIR/amalgamation/nativehelper"
  )

  mapfile -t source_files < <(
    find amalgamation -type f \
      \( -name '*.c' -o -name '*.cpp' \) \
      ! -name 'shell3mc_amalgamation.c' \
      ! -name 'sqlite3.c' \
      | sort
  )
  if [ "${#source_files[@]}" -eq 0 ]; then
    echo "No C/C++ sources found in $AMALG_DIR" >&2
    exit 7
  fi

  object_files=()
  for source_file in "${source_files[@]}"; do
    object_file="$(basename "${source_file%.*}").o"
    compile_flags=("${INCLUDE_FLAGS[@]}")
    case "$source_file" in
      *.cpp)
        if [ "$(basename "$source_file")" = "JNIHelp.cpp" ]; then
          compile_flags+=(-D__GLIBC__=1)
        fi
        echo "Compiling $source_file -> $object_file"
        "$CXX" $CFLAGS "${compile_flags[@]}" -c "$source_file" -o "$object_file"
        ;;
      *)
        echo "Compiling $source_file -> $object_file"
        "$CC" $CFLAGS "${compile_flags[@]}" -c "$source_file" -o "$object_file"
        ;;
    esac
    object_files+=("$object_file")
  done

  echo "Linking libsqliteX.so"
  "$CXX" $LDFLAGS $CXX_STDLIB_FLAGS "${object_files[@]}" $ANDROID_LIBS -o libsqliteX.so
  "$STRIP" libsqliteX.so

  mkdir -p "$OUT_DIR/$arch"
  mv libsqliteX.so "$OUT_DIR/$arch/"

  popd >/dev/null
  rm -rf "$BUILD_DIR"
  echo "Built $OUT_DIR/$arch/libsqliteX.so"
done

echo "All builds finished. Libraries placed in $OUT_DIR"