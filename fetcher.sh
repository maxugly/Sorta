#!/bin/bash
set -e

# --- Configuration ---
# NO CAPITALS ALLOWED
ARTIFACT_NAME="audiofiler-build" 
OUTPUT_DIR="build-artifacts"
BINARY_NAME="audiofiler"
BUILD_BRANCH="build-automation"

# 1. Check for GitHub CLI
if ! command -v gh &> /dev/null; then
    echo "❌ Error: GitHub CLI (gh) is not installed."
    exit 1
fi

echo "🔭 Looking for the latest successful build on branch: $BUILD_BRANCH"

# 2. Get the ID of the most recent SUCCESSFUL run
# We filter strictly for the lowercase project branch
RUN_ID=$(gh run list --branch "$BUILD_BRANCH" --status success --limit 1 --json databaseId --jq '.[0].databaseId')

if [ -z "$RUN_ID" ]; then
    echo "❌ Error: No successful builds found for $BUILD_BRANCH."
    echo "💡 Make sure your last GitHub Action run actually finished and was green."
    exit 1
fi

echo "✅ Found Run ID: $RUN_ID"

# 3. Clean and Download
echo "📥 Downloading artifacts into $OUTPUT_DIR..."
mkdir -p "$OUTPUT_DIR"
# Delete the old stuff first so we don't mix versions
rm -rf "${OUTPUT_DIR:?}/"*

gh run download "$RUN_ID" -n "$ARTIFACT_NAME" --dir "$OUTPUT_DIR"

# 4. Strictly Lowercase Search
echo "🔍 Searching for binary: $BINARY_NAME"

# We removed the '-o -name "Audiofiler"'—this is now a strictly lowercase hunt
BINARY_PATH=$(find "$OUTPUT_DIR" -type f -name "$BINARY_NAME" | head -n 1)

if [ -n "$BINARY_PATH" ]; then
    chmod +x "$BINARY_PATH"
    echo "🚀 Success! The true audiofiler is ready at: $BINARY_PATH"
    
    # Check if we are in Crostini/Nix environment
    if command -v nix &> /dev/null && [ -f "flake.nix" ]; then
        echo "❄️  Nix detected. You should run this with: nix develop --command ./$BINARY_PATH"
    else
        echo "💡 Run it with: ./$BINARY_PATH"
    fi
else
    echo "⚠️  CRITICAL ERROR: Binary '$BINARY_NAME' not found in artifact!"
    echo "📂 Here is what was actually inside the download:"
    ls -R "$OUTPUT_DIR"
    exit 1
fi
