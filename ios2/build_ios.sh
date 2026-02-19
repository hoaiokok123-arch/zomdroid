#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/../build-ios2"

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=16.0 \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DZOMDROID_IOS_ENABLE_EMULATION=ON \
  -DZOMDROID_IOS_BUILD_APP=ON

cmake --build "${BUILD_DIR}" --config Release --target ZomdroidIOSApp

echo "Built app at: ${BUILD_DIR}/Release-iphoneos/ZomdroidIOSApp.app"
