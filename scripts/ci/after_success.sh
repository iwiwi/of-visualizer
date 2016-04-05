#!/bin/bash
set -ev
ROOT=${TRAVIS_BUILD_DIR:-"$( cd "$(dirname "$0")/../../.." ; pwd -P )"}

echo "**** Uploading to Dropbox ****"
cd $ROOT/apps/visualizer/emptyExample

$ARCHIVE_FILENAME=$TARGET.tar.gz
tar czvf $ARCHIVE_FILENAME bin

curl --fail -X POST https://content.dropboxapi.com/2/files/upload \
     --header "Authorization: Bearer $DROPBOX_TOKEN" \
     --header "Dropbox-API-Arg: {\"path\": \"$ARCHIVE_FILENAME\",\"mode\": \"add\",\"autorename\": false,\"mute\": false}" \
     --header "Content-Type: application/octet-stream" \
     --data-binary @$ARCHIVE_FILENAME \
