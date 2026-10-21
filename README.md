# Advancements 0.8.3

This Android LeviLaunchroid mod uses **NMDK** for its in-game menu button. NMDK must be installed and loaded before Advancements. The package registers `button.nmdk_advancements` through NMDK’s JSON UI registry instead of creating a floating launcher overlay button.

The mod now registers a real Preloader `ButtonInfo` control labelled **Advancements** (`advancements.open`), so it is visible as an Android on-screen button instead of relying only on a JSON handler. It also retains the NMDK `button.nmdk_modmenu` handler for installations that provide that existing menu control. The current visibility radio setting remains available in LeviLaunchroid’s mod settings as **Pause screen only** or **Everywhere**.

Because the predefined control is shared with the separate ModMenuButton example, do not load both mods if you want that control to open Advancements; the last registry handler loaded will own `button.nmdk_modmenu`.

The panel is a Minecraft-style advancement page with five category tabs and labels: Minecraft, Nether, The End, Adventure, and Husbandry. All 126 catalog entries are retained in their original categories. Each node now uses an embedded authentic Minecraft item texture selected from its advancement ID and description: diamond swords for combat and monster-killing advancements, bows for ranged achievements, fishing rods for fishing/travel, elytra for flight/End achievements, netherite swords for Nether achievements, diamond pickaxes for mining/crafting, apples/bread for farming and food, and the crafting table as the general fallback. Textures are rasterized into supported draw-command pixels rather than relying on the launcher’s optional image-command path. Locked entries are dimmed.

The LeviLaunchroid mod settings include **Advancements UI scale**, a floating-point slider from **0.00 to 1.00**. `1.00` is the normal size; `0.50` is half size; `0.00` hides the panel until increased again.

Completion notifications are independent settings: **Advancement popup** and **Advancement chat message** can be enabled separately or enabled together. The Android build now reads the real Bedrock `ToastMessage` title, subtitle, and message-id fields (including the padded libc++ layout) and also checks text sent through `GuiData::displayClientMessage`. Only a title matching a catalog entry unlocks it; recipe-unlock toasts are not treated as advancements. Duplicate notifications are suppressed after the entry is completed. The chat message is sent through the Bedrock `GuiData::displayClientMessage` signature on Android.

**Experimental UI style** provides **Default UI** and **Java UI**. Default UI is unchanged. Java UI now displays the copied vanilla Java advancement interface reference from the Minecraft Wiki (`assets/java-ui/vanilla_interface.png`) rather than assembling a custom Bedrock-style panel. The reference includes the actual gray window, five item tabs, tiled stone background, connected gold advancement nodes, and selected advancement tooltip. See `research/java-ui-sources.md` for the source URLs.

Completion updates listen for NMDK achievement toasts. Bedrock’s Java-style advancement criteria are not exposed by NMDK as a catalog API, so this build can only unlock entries when Bedrock reports an achievement/title that matches the catalog. It does not pretend that recipe toasts are advancement completions, and it cannot infer an arbitrary Java advancement from ordinary block or item actions without a separate criteria engine.

Build target: Android ARM64/AArch64. Minecraft compatibility: `1.26.5*.**`.

## Build

Install/load NMDK v1.0.7 in LeviLaunchroid, then configure CMake with the Android NDK toolchain, the NMDK repository, Preloader headers/library, and fmt headers. The generated package is `AdvancementsNMDK.levipack`.
