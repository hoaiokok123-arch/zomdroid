# Zomdroid iOS Port (Jailbreak/JIT Path)

This folder now contains a runnable iOS shell and native runtime bridge, not just planning docs.

## What is implemented

- Portable runtime API:
  - `ios/include/zomdroid_core_api.h`
- iOS bridge functions:
  - `ios/include/zomdroid_ios_bridge.h`
  - `ios/Sources/ZomdroidIOSBridge.mm`
- Core launcher thread that calls native `zomdroid_init` + `zomdroid_start_game`:
  - `ios/Sources/zomdroid_core_api_stub.c`
- iOS app shell (UIKit):
  - `ios/App/main.m`
  - `ios/App/ZomdroidAppDelegate.*`
  - `ios/App/ZomdroidGameViewController.mm`
- Optional in-process emulation hook backend wiring:
  - `ios/Sources/ZomdroidDyldInterpose.c`
  - `app/src/main/cpp/linker.c` (non-Android hook path)
  - `app/src/main/cpp/emulation.c` (`MAP_JIT` support)

## Launch config

Copy `ios/Resources/launch_config.example.json` to iOS app Documents as:

- `Documents/zomdroid-launch.json`

It controls:
- game dir
- runtime/library dir
- main class
- renderer
- JVM/linker dylib paths
- JVM args / game args / env vars

## Build (macOS + Xcode toolchain)

Example:

```bash
cmake -S ios -B build-ios -G Xcode \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=16.0 \
  -DCMAKE_OSX_SYSROOT=iphoneos \
  -DZOMDROID_IOS_ENABLE_EMULATION=ON \
  -DZOMDROID_IOS_BUILD_APP=ON
cmake --build build-ios --config Release
```

Main target:
- `ZomdroidIOSApp`

Shortcut script:
- `ios/build_ios.sh`

## Entitlements

Provided templates:
- `ios/entitlements.sideload.plist`
- `ios/entitlements.trollstore.plist`

Use the one matching your install path (sideload vs TrollStore/jailbreak), then sign with your preferred flow.

## Notes

- This path targets jailbroken / JIT-enabled usage.
- You still need compatible runtime payloads (JRE + native libs + box64 dependencies) in your configured `library_dir`.
- If you disable `ZOMDROID_IOS_ENABLE_EMULATION`, the shell still runs but JNI emulation hooks are not enabled.

Detailed deployment steps:
- `ios/DEPLOY.md`
