#!/bin/bash
set -e

# --- Configuration ---
BUILD_BRANCH="build-automation"
ARTIFACT_NAME="audiofiler-build" 
OUTPUT_DIR="build-artifacts"
BINARY_NAME="audiofiler" 
START_BRANCH=$(git rev-parse --abbrev-ref HEAD)

# --- Safety Net ---
cleanup() {
    echo "🏠 Returning to $START_BRANCH..."
    git checkout "$START_BRANCH" &> /dev/null
}
trap cleanup EXIT

echo "🚀 Starting sync, build, and YOLO launch for audiofiler..."

# 1. Dependency & Auth Checks
if ! command -v gh &> /dev/null; then
    echo "❌ Error: GitHub CLI (gh) is not installed."
    exit 1
fi

# 2. Branch Management
git fetch origin
if git rev-parse --verify $BUILD_BRANCH &> /dev/null; then
    git checkout $BUILD_BRANCH
    git reset --hard origin/main
else
    echo "✨ Creating new build branch: $BUILD_BRANCH"
    git checkout -b $BUILD_BRANCH origin/main
fi

# 3. Sync & Push
if [ "$START_BRANCH" != "$BUILD_BRANCH" ] && [ "$START_BRANCH" != "" ]; then
    echo "🛠️  Merging changes from $START_BRANCH..."
    git merge "$START_BRANCH" --no-edit
fi

echo "📤 Force-pushing to GitHub..."
git push origin $BUILD_BRANCH --force

# 4. The "Wake Up" Buffer
echo "⏳ Waiting 15s for GitHub Actions to index..."
sleep 15 

# 5. Automated Build Watch
echo "🔭 Watching the latest build on $BUILD_BRANCH..."
RUN_ID=$(gh run list --branch "$BUILD_BRANCH" --limit 1 --json databaseId --jq '.[0].databaseId')

if [ -z "$RUN_ID" ]; then
    echo "❌ Error: Could not find a recent run."
    exit 1
fi

if ! gh run watch "$RUN_ID" --exit-status; then
    echo "❌ Build failed on GitHub!"
    exit 1
fi

# 6. Download (FIXED LINE)
echo "✅ Build successful! Downloading..."
mkdir -p "$OUTPUT_DIR"
rm -rf "${OUTPUT_DIR:?}/"*
gh run download "$RUN_ID" -n "$ARTIFACT_NAME" --dir "$OUTPUT_DIR"

# 7. Launch Logic
echo "🔍 Looking for binary: $BINARY_NAME"
pkill -x "$BINARY_NAME" || true

# Check for lowercase 'audiofiler' or 'Audiofiler' locally just in case
BINARY_PATH=$(find "$OUTPUT_DIR" -type f -name "$BINARY_NAME" -o -name "Audiofiler" | head -n 1)

if [ -n "$BINARY_PATH" ]; then
    chmod +x "$BINARY_PATH"
    echo "🔥 Launching $BINARY_PATH..."
    "$BINARY_PATH" & 
else
    echo "⚠️  Binary '$BINARY_NAME' not found in artifact!"
    ls -R "$OUTPUT_DIR"
fi

echo "🎉 Process complete."