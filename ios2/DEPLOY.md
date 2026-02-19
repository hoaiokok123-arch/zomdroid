# Deploy Guide (Jailbreak / TrollStore)

## 1) Build

On macOS with Xcode toolchain:

```bash
./ios2/build_ios.sh
```

Output app:

- `build-ios2/Release-iphoneos/ZomdroidIOSApp.app`

## 2) Sign

Pick one entitlement template:

- `ios2/entitlements.sideload.plist`
- `ios2/entitlements.trollstore.plist`

Example (ldid):

```bash
ldid -Sios2/entitlements.trollstore.plist build-ios2/Release-iphoneos/ZomdroidIOSApp.app/ZomdroidIOSApp
```

## 3) Install

- Install with TrollStore, or your jailbreak signing flow.

## 4) Runtime payload layout

Copy runtime payload to device, for example:

- `/var/mobile/Documents/zomdroid/runtime/`
- `/var/mobile/Documents/zomdroid/instances/default/game/`

Runtime folder should contain at least:

- JRE with `libjvm` (`.../lib/server/libjvm.dylib`)
- Zomboid jars/classes
- Native libs required by the game
- Optional: `libzomdroidlinker.dylib` if using external linker backend

## 5) Launch config

Create file:

- `Documents/zomdroid-launch.json`

Use template:

- `ios2/Resources/launch_config.example.json`

Adjust paths to your device.

## 6) Run

- Open app.
- Tap `Start`.
- Check on-screen status text for init/start errors.
