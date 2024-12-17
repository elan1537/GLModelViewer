#!/bin/bash
fswatch -o "${PWD}" | while read event
do
    rsync -avz --delete --exclude='model/' --exclude='build/' --exclude=".vscode/" --exclude="sync.sh" "${PWD}/" wrl-l40:/home/wrl-ubuntu/GSViewer/
done