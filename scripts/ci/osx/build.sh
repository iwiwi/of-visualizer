#!/bin/bash
set -ev
ROOT=${TRAVIS_BUILD_DIR:-"$( cd "$(dirname "$0")/../../.." ; pwd -P )"}
echo "**** Building oF + visualizer - OSX Template Project ****"
xcodebuild -configuration Release -target visualizer -project "$ROOT/apps/myapps/visualizer/visualizer.xcodeproj"
