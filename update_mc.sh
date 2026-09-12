#!/usr/bin/env bash
set -euo pipefail

# Usage: ./update_sqlite3mc.sh [GIT_REPO]
# Default repo: https://github.com/utelle/SQLite3MultipleCiphers
# Downloads the latest release asset that contains 'amalgamation' in its name
# and also downloads the first .aar asset from the same release into sqlite3mc/

REPO_ARG="${1:-https://github.com/utelle/SQLite3MultipleCiphers}"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
AMALG_DIR="$ROOT_DIR/sqlite3mc/amalgamation"
AAR_DIR="$ROOT_DIR/sqlite3mc"
TMP_DIR="$(mktemp -d)"
RELEASE_JSON="$TMP_DIR/release.json"

# normalize owner/repo
if echo "$REPO_ARG" | grep -qE '^https?://'; then
  OWNER_REPO=$(echo "$REPO_ARG" | sed -E 's#https?://github.com/##; s#\.git$##; s#/$##')
else
  OWNER_REPO="$REPO_ARG"
fi

API_URL="https://api.github.com/repos/$OWNER_REPO/releases/latest"

echo "Fetching latest release metadata for $OWNER_REPO..."
# Use GITHUB_TOKEN if provided to increase rate limit
if [ -n "${GITHUB_TOKEN:-}" ]; then
  AUTH_HEADER=( -H "Authorization: token $GITHUB_TOKEN" )
else
  AUTH_HEADER=()
fi

if ! curl -sL "${AUTH_HEADER[@]}" "$API_URL" -o "$RELEASE_JSON"; then
  echo "Failed to fetch release metadata from $API_URL" >&2
  rm -rf "$TMP_DIR"
  exit 2
fi

# Find amalgamation asset URL and .aar asset URL (if any)
if command -v jq >/dev/null 2>&1; then
  AMALG_URL=$(jq -r '.assets[] | select(.name | test("amalgamation"; "i")) | .browser_download_url' "$RELEASE_JSON" | head -n1)
  AAR_URL=$(jq -r '.assets[] | select(.name | test("\\.aar$"; "i")) | .browser_download_url' "$RELEASE_JSON" | head -n1)
  AAR_NAME=$(jq -r '.assets[] | select(.name | test("\\.aar$"; "i")) | .name' "$RELEASE_JSON" | head -n1)
else
  # Fallback to awk parsing
  AMALG_URL=$(awk 'BEGIN{IGNORECASE=1}
    /"name"/ { name=$0; getline; if(name ~ /amalgamation/) {
      if($0 ~ /browser_download_url/) { gsub(/.*"browser_download_url"\s*:\s*"/, ""); gsub(/".*/, ""); print; exit }
    }}' "$RELEASE_JSON")
  AAR_URL=$(awk 'BEGIN{IGNORECASE=1}
    /"name"/ { name=$0; if(name ~ /\.aar/) { gsub(/.*"name"\s*:\s*"/, ""); gsub(/".*/, ""); aarname=$0 } }
    /"browser_download_url"/ { if(aarname != "") { gsub(/.*"browser_download_url"\s*:\s*"/, ""); gsub(/".*/, ""); print; exit } }' "$RELEASE_JSON")
  AAR_NAME=$(awk 'BEGIN{IGNORECASE=1}
    /"name"/ { name=$0; if(name ~ /\.aar/) { gsub(/.*"name"\s*:\s*"/, ""); gsub(/".*/, ""); print; exit } }' "$RELEASE_JSON")
fi

if [ -z "$AMALG_URL" ]; then
  echo "No release asset with 'amalgamation' found in latest release for $OWNER_REPO" >&2
  rm -rf "$TMP_DIR"
  exit 3
fi

echo "Found amalgamation asset: $AMALG_URL"
ZIP_FILE="$TMP_DIR/amalgamation_asset.zip"

echo "Downloading amalgamation asset..."
if ! curl -L -o "$ZIP_FILE" "$AMALG_URL"; then
  echo "Failed to download amalgamation asset" >&2
  rm -rf "$TMP_DIR"
  exit 4
fi

# Ensure unzip is available
if ! command -v unzip >/dev/null 2>&1; then
  echo "unzip is required but not installed" >&2
  rm -rf "$TMP_DIR"
  exit 5
fi

UNZIP_DIR="$TMP_DIR/unzipped"
mkdir -p "$UNZIP_DIR"
unzip -q "$ZIP_FILE" -d "$UNZIP_DIR"

# Replace existing amalgamation dir
rm -rf "$AMALG_DIR"
mkdir -p "$(dirname "$AMALG_DIR")"

# Move extracted content into amalgamation
shopt -s nullglob
entries=("$UNZIP_DIR"/*)
if [ "${#entries[@]}" -eq 1 ] && [ -d "${entries[0]}" ]; then
  mv "${entries[0]}" "$AMALG_DIR"
else
  mkdir -p "$AMALG_DIR"
  mv "$UNZIP_DIR"/* "$AMALG_DIR/"
fi

# Download .aar asset (if found) into sqlite3mc/
mkdir -p "$AAR_DIR"
if [ -n "$AAR_URL" ]; then
  AAR_FILE_PATH="$AAR_DIR/${AAR_NAME:-downloaded_lib.aar}"
  echo "Found AAR asset in same release: $AAR_URL"
  echo "Downloading AAR to $AAR_FILE_PATH..."
  if ! curl -L -o "$AAR_FILE_PATH" "$AAR_URL"; then
    echo "Warning: failed to download AAR asset; continuing without it" >&2
  else
    echo "AAR downloaded to $AAR_FILE_PATH"
  fi
else
  echo "No .aar asset found in release; skipping AAR download"
fi

rm -rf "$TMP_DIR"

echo "Amalgamation installed to $AMALG_DIR"
[ -n "$AAR_URL" ] && echo "(If present) AAR is in $AAR_DIR"
exit 0
