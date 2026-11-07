# Advancements 1.1.3

Advancements is a standalone Android ARM64 native mod for Bedrock `1.26.5*.**`. It provides five Minecraft-style advancement categories, embedded item icons, locked/unlocked node states, completion popups, completion chat messages, per-world session state, a UI scale setting, and Default UI/Revamped UI styles.

## Standalone behavior

This release has no LeviLamroid or NMDK runtime dependency. It uses the Preloader native mod, mod-menu, touch, and draw-command interfaces directly. It is linked against the launcher-owned `libpreloader.so` ABI pinned by LeviLaunchroid v1.6.0; that library is not bundled into the LeviPack. The Advancements button is always available as an in-game floating touch button; there is no pause-menu-only mode and no pause-screen resource-pack modification.

The package contains only the native library and icon. It does not overwrite resource packs or inject pause-screen JSON. The image-registration call is not imported; advancement icons are rendered using draw primitives instead. The native library has a `DT_NEEDED` entry for `libpreloader.so`, which makes Android resolve the launcher’s built-in Preloader before resolving the mod’s Mod Menu symbols.

## Features

The five categories are Minecraft, Nether, The End, Adventure, and Husbandry. The catalog retains all 126 entries in their original categories. Nodes use embedded Minecraft item textures selected from their advancement IDs and descriptions, and locked icons are dimmed.

Settings include:

- **Advancements UI scale:** `0.00` to `1.00`; `0.00` hides the panel.
- **Revamped UI:** Default UI or Revamped UI.
- **Advancement popup:** independent toggle.
- **Advancement chat message:** independent toggle.

The completion popup is drawn in the lower-right so it does not overlap Bedrock recipe notifications. Completion state is tracked separately by active world/session key where the host provides world identity.

## Build

The build requires Android NDK r28c or compatible, CMake, Ninja, Preloader headers/library, and fmt headers if required by the host SDK. Configure `PRELOADER_INCLUDE_DIR` and `PRELOADER_LIBRARY` when those paths differ:

```sh
cmake -S . -B build-standalone -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/android-ndk/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
cmake --build build-standalone --parallel
```

The build produces `libAdvancements.so` and `Advancements.levipack`. The LeviPack manifest has an empty dependency list and targets Minecraft `1.26.5*.**`.
