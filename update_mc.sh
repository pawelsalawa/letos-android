#!/usr/bin/env bash
set -euo pipefail

# Usage: ./update_mc.sh [GIT_REPO]
# Default repo: https://github.com/utelle/SQLite3MultipleCiphers
# Downloads the latest SQLite3MultipleCiphers amalgamation into
# LetosRemoteProject/sqlite3mc/amalgamation, then overlays SQLite Android
# Bindings JNI sources there and:
# - replaces LetosRemoteProject/letosremote/src/main/java/org/sqlite
# - refreshes LetosRemoteProject/sqlite3mc/amalgamation with JNI C/C++/header
#   sources from SQLite Android Bindings (excluding sqlite3.cpp/sqlite3.h)
# - rewrites bundled SQLite symbols to mc_sqlite3* to avoid colliding with
#   Android's libsqlite.so symbols

REPO_ARG="${1:-https://github.com/utelle/SQLite3MultipleCiphers}"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BINDINGS_URL="https://www.sqlite.org/android/zip/SQLite+Android+Bindings.zip?uuid=trunk"
LETOS_ROOT="$ROOT_DIR/LetosRemoteProject"
SQLITE3MC_DIR="$LETOS_ROOT/sqlite3mc"
AMALG_DIR="$SQLITE3MC_DIR/amalgamation"
JAVA_ORG_DIR="$ROOT_DIR/LetosRemoteProject/letosremote/src/main/java/org"
JNI_SOURCE_DIR_REL="sqlite3/src/main/jni/sqlite"
LETOS_NATIVE_C_DIR="$LETOS_ROOT/letosremote/src/main/c"
TMP_DIR="$(mktemp -d)"
RELEASE_JSON="$TMP_DIR/release.json"
trap 'rm -rf "$TMP_DIR"' EXIT

if ! command -v unzip >/dev/null 2>&1; then
  echo "unzip is required but not installed" >&2
  exit 2
fi

# normalize owner/repo
if echo "$REPO_ARG" | grep -qE '^https?://'; then
  OWNER_REPO=$(echo "$REPO_ARG" | sed -E 's#https?://github.com/##; s#\.git$##; s#/$##')
else
  OWNER_REPO="$REPO_ARG"
fi

API_URL="https://api.github.com/repos/$OWNER_REPO/releases/latest"

rewrite_sqlite_symbols() {
  local in_file="$1"
  local out_file
  out_file="$(mktemp "$TMP_DIR/$(basename "$in_file").XXXXXX")"

  perl -pe '
    s/\bsqlite3/mc_sqlite3/g;
    s/mc_sqlite3mc_amalgamation\./sqlite3mc_amalgamation./g;
    s/mc_sqlite3ext\.h/sqlite3ext.h/g;
    s/mc_sqlite3\.h/sqlite3.h/g;
  ' "$in_file" > "$out_file"

  mv "$out_file" "$in_file"
}

echo "Fetching latest release metadata for $OWNER_REPO..."
if [ -n "${GITHUB_TOKEN:-}" ]; then
  AUTH_HEADER=(-H "Authorization: token $GITHUB_TOKEN")
else
  AUTH_HEADER=()
fi

curl -sL --fail "${AUTH_HEADER[@]}" "$API_URL" -o "$RELEASE_JSON"

if command -v jq >/dev/null 2>&1; then
  AMALG_URL=$(jq -r '.assets[] | select(.name | test("amalgamation"; "i")) | .browser_download_url' "$RELEASE_JSON" | head -n1)
else
  AMALG_URL=$(awk 'BEGIN{IGNORECASE=1}
    /"name"/ { name=$0; getline; if(name ~ /amalgamation/) {
      if($0 ~ /browser_download_url/) { gsub(/.*"browser_download_url"\s*:\s*"/, ""); gsub(/".*/, ""); print; exit }
    }}' "$RELEASE_JSON")
fi

if [ -z "${AMALG_URL:-}" ]; then
  echo "No release asset with 'amalgamation' found in latest release for $OWNER_REPO" >&2
  exit 3
fi

echo "Downloading SQLite3MultipleCiphers amalgamation..."
curl -L --fail --retry 3 \
  "$AMALG_URL" \
  -o "$TMP_DIR/amalgamation.zip"

echo "Downloading SQLite Android Bindings..."
curl -L --fail --retry 3 \
  "$BINDINGS_URL" \
  -o "$TMP_DIR/bindings.zip"

rm -rf "$AMALG_DIR"
mkdir -p "$AMALG_DIR"
find "$SQLITE3MC_DIR" -maxdepth 1 -type f ! -name 'README.md' -delete

unzip -q "$TMP_DIR/amalgamation.zip" -d "$TMP_DIR/amalgamation"

shopt -s nullglob
amalg_entries=("$TMP_DIR/amalgamation"/*)
if [ "${#amalg_entries[@]}" -eq 1 ] && [ -d "${amalg_entries[0]}" ]; then
  cp -R "${amalg_entries[0]}"/. "$AMALG_DIR"/
else
  cp -R "$TMP_DIR/amalgamation"/. "$AMALG_DIR"/
fi
unzip -q "$TMP_DIR/bindings.zip" -d "$TMP_DIR"
SRC="$TMP_DIR/SQLite_Android_Bindings"
JAVA_SOURCE_DIR="$SRC/sqlite3/src/main/java/org/sqlite"
JNI_SOURCE_DIR="$SRC/$JNI_SOURCE_DIR_REL"

if [ ! -d "$JAVA_SOURCE_DIR" ]; then
  echo "Java source directory not found: $JAVA_SOURCE_DIR" >&2
  exit 3
fi

if [ ! -d "$JNI_SOURCE_DIR" ]; then
  echo "JNI source directory not found: $JNI_SOURCE_DIR" >&2
  exit 4
fi

if [ ! -d "$JAVA_ORG_DIR" ]; then
  echo "Target Java directory not found: $JAVA_ORG_DIR" >&2
  exit 5
fi

echo "Replacing org/sqlite Java sources..."
rm -rf "$JAVA_ORG_DIR/sqlite"
cp -R "$JAVA_SOURCE_DIR" "$JAVA_ORG_DIR/"

echo "Overlaying SQLite Android Bindings JNI sources into amalgamation..."
while IFS= read -r -d '' source_file; do
  relative_path="${source_file#"$JNI_SOURCE_DIR"/}"
  destination="$AMALG_DIR/$relative_path"

  mkdir -p "$(dirname "$destination")"
  cp "$source_file" "$destination"
done < <(
  find "$JNI_SOURCE_DIR" -type f \
    \( -name '*.c' -o -name '*.cpp' -o -name '*.h' \) \
    ! -name 'sqlite3.c' \
    ! -name 'sqlite3.cpp' \
    ! -name 'sqlite3.h' \
    -print0
)

echo "Rewriting bundled SQLite symbols to mc_sqlite3*..."
transform_files=()
while IFS= read -r -d '' source_file; do
  if LC_ALL=C grep -q '\<sqlite3' "$source_file"; then
    transform_files+=("$source_file")
  fi
done < <(
  find "$AMALG_DIR" "$LETOS_NATIVE_C_DIR" -type f \
    \( -name '*.c' -o -name '*.cpp' -o -name '*.h' \) \
    -print0
)

for source_file in "${transform_files[@]}"; do
  rewrite_sqlite_symbols "$source_file"
done

echo "SQLite Android Bindings installed into:"
echo "  - $JAVA_ORG_DIR/sqlite"
echo "  - $AMALG_DIR"
exit 0
