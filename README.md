# Advancements 0.9.5

Advancements is an Android ARM64 native mod for LeviLaunchroid/LeviLamroid and Minecraft Bedrock `1.26.5*.**`. It provides five Minecraft-style advancement categories, embedded Minecraft item icons, locked/unlocked node states, completion popups, completion chat messages, per-world session state, a scale slider, and Default UI/Java UI styles.

## LeviLamroid integration

This version does not include or link against NMDK. It uses LeviLamroid’s public APIs for pause-menu lifecycle callbacks, render-surface dimensions, pause-state polling, Preloader draw commands, touch callbacks, and mod registration. **Everywhere is the initial visibility state**, so the button is registered even if LeviLamroid’s persisted configuration callback arrives after the enable phase. Selecting **Pause screen only** is synchronized against `ll::ui::isInPauseMenu()` during rendering, so it is removed outside the pause screen even when a lifecycle callback is missed; selecting **Everywhere** keeps it registered outside the pause screen as well.

The mod still uses Preloader’s native draw-command and signature-hook interfaces for rendering and for observing Bedrock `GuiData` chat traffic. Completion matching is deliberately title/key based so recipe-unlock messages cannot unlock unrelated advancements. The chat hook accepts catalog titles and known Bedrock achievement aliases; notifications are deduplicated per world-session key.

## Features

The five categories are Minecraft, Nether, The End, Adventure, and Husbandry. The catalog retains all 126 entries in their original categories. Nodes use embedded authentic Minecraft item textures selected from their advancement IDs and descriptions: diamond swords for combat, bows for ranged achievements, fishing rods for fishing/travel, elytra for flight/End entries, Netherite swords for Nether entries, diamond pickaxes for mining/crafting, food icons for Husbandry, and crafting tables as a fallback. Locked icons are dimmed.

LeviLaunchroid settings include:

- **Show Advancements:** Pause screen only or Everywhere.
- **Advancements UI scale:** `0.00` to `1.00`; `0.00` hides the panel.
- **Experimental UI style:** Default UI or Java UI.
- **Advancement popup:** independent toggle.
- **Advancement chat message:** independent toggle.

Both completion notifications can be enabled simultaneously. The popup is drawn at the lower-right so it does not overlap Bedrock’s recipe-unlock notification area. Completion state is held separately for each active world/level pointer and is usable in creative and pre-existing worlds; it is not a global completed/uncompleted flag.

The Java UI is an interactive native advancement tree styled after Java Edition: tabs, connected node lines, selected-node details, dimmed locked icons, and completion coloring. The style parser accepts both numeric radio values and literal labels such as `Java`, `Java UI`, and `Everywhere`. It is not a non-interactive screenshot overlay.

## Build

The build expects the LeviLamroid source/header tree, Preloader headers/library, Android NDK r28c or compatible, and fmt headers. Configure `LEVILAMROID_DIR`, `PRELOADER_INCLUDE_DIR`, `PRELOADER_LIBRARY`, and `FMT_INCLUDE_DIR` if their locations differ:

```sh
cmake -S . -B build-levilamroid -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/android-ndk/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
cmake --build build-levilamroid --parallel
```

The build produces `libAdvancementsLeviLamroid.so` and `AdvancementsLeviLamroid.levipack`. The LeviPack manifest declares LeviLamroid as its runtime dependency and uses author `Electrocharge`.
