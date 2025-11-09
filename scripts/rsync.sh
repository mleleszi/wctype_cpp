#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <ssh-target> [remote-path]"
    echo "Example: $0 user@server.com ~/dev/wctype_cpp"
    exit 1
fi

TARGET="$1"
REMOTE_PATH="${2:-~/dev/wctype_cpp}"
LOCAL_DIR="../"

ssh "$TARGET" "mkdir -p $REMOTE_PATH"

rsync -avz \
    --exclude='.git' \
    --exclude-from="../.gitignore" \
    "$LOCAL_DIR/" \
    "$TARGET:$REMOTE_PATH/"
