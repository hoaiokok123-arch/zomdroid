# iOS Port Plan (Amethyst-style Platform Split)

## Reality check

Zomdroid today is tightly coupled to Android internals:
- Android app/UI stack (`Activity`, `SurfaceView`, input managers).
- Android linker namespace hooks (`liblinkernsbypass`, `android_dlext`).
- Embedded JVM bootstrap via JNI and dynamic loader behavior.
- Box64 runtime assumptions built around Android/Linux userspace.

Because of that, a direct "rebuild for iOS" is not possible.

## Target architecture

Follow a split similar to Amethyst-iOS style launcher separation:

1. `Core runtime layer` (portable C/C++)
- game lifecycle
- event queue
- input forwarding API
- renderer/runtime configuration

2. `iOS platform layer` (Objective-C++/Swift)
- view/layer lifecycle (`UIView` + `CAMetalLayer`/OpenGL fallback)
- touch/gamepad/keyboard mapping to core events
- filesystem sandbox paths
- audio/session handling

3. `Emulation/runtime backend`
- replace or rework Android-specific Box64 loader path
- remove Android linker namespace dependency
- define iOS-compatible dynamic loading strategy

## App Store and distribution constraints

iOS policy constraints strongly affect this project:
- JIT/dynamic code generation is restricted.
- Executing downloaded code/binaries is restricted for App Store.
- Self-managed runtime loading can fail review.

Practical implication:
- For App Store: major redesign is required.
- For sideload/TestFlight/AltStore experimentation: still feasible as a dev build path.

## Phased migration

### Phase 1: Decouple platform interfaces
- Status: done (initial cut).
- Portable API is in place (`ios2/include/zomdroid_core_api.h`).
- Shared native headers were adjusted to reduce Android-only type coupling.

### Phase 2: Build iOS shell
- Status: done (first runnable shell).
- Bridge implemented: `ios2/Sources/ZomdroidIOSBridge.mm`.
- UIKit app shell implemented: `ios2/App/ZomdroidGameViewController.mm`.
- Touch input is routed to native core event APIs.

### Phase 3: Runtime backend parity
- Status: in progress.
- Non-Android path was added in `app/src/main/cpp/zomdroid.c`.
- `MAP_JIT` path was added in `app/src/main/cpp/emulation.c`.
- Linker hook adaptation for non-Android was started in `app/src/main/cpp/linker.c`.

### Phase 4: Packaging and assets
- Status: pending.
- Next work: runtime payload installer and in-app import pipeline.

## Mapping from current files

- Android JNI glue:
  - `app/src/main/cpp/zomdroid_jni.c`
- Android runtime/bootstrap:
  - `app/src/main/cpp/zomdroid.c`
  - `app/src/main/cpp/linker.c`
  - `app/src/main/cpp/emulation.c`
- Android UI/input:
  - `app/src/main/java/com/zomdroid/GameActivity.java`
  - `app/src/main/java/com/zomdroid/GameLauncher.java`

iOS replacement areas:
- `ios2/Sources/ZomdroidIOSBridge.mm`
- `ios2/App/ZomdroidGameViewController.mm`
- `ios2/Sources/zomdroid_core_api_stub.c`

## Definition of done (first runnable iOS milestone)

- App launches to an iOS native shell.
- Core init/shutdown API is callable.
- Touch input reaches core event queue.
- A render surface lifecycle is managed on iOS.
- Runtime backend either starts successfully or fails with explicit diagnostics.
