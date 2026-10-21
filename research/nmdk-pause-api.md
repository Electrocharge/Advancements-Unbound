# NMDK / pause-menu research

Sources:
- https://github.com/nexcaise/NMDK
- https://github.com/spygamingog/LeviLamroid
- Local supplied source: `/home/ubuntu/upload/ModMenuButton.zip`

Findings:

1. NMDK is an Android ARM64 SDK for LeviLaunchroid. Its public repository includes `nmdk/include/nmdk/api/ui/JsonUIButtonRegistry.hpp` and `nmdk/include/nmdk/events/ClientInstance.hpp`.
2. `JsonUIButtonRegistry::registerButton(name, ButtonState, PreviousButtonState, callback)` registers handlers for JSON UI buttons. The header comment says NMDK attaches registered handlers to menu `ScreenController` instances, specifically MainMenu/StartMenu, on screen open.
3. The supplied ModMenuButton source uses `button.nmdk_modmenu`, registers it through `JsonUIButtonRegistry`, and calls `pl::modmenu::requestOpenMenu()`. This is the relevant pattern for an on-screen launcher/pause-menu button.
4. NMDK exposes `ClientInstanceIsPreGameEvent` with `getIsPreGame()`. This can distinguish the pre-game/start-menu context from an in-world pause context. The intended pause-only guard is to reject the JSON-button callback while `isPreGame` is true, allowing the same registered button only while in-world.
5. NMDK exposes `ToastManagerPushToastEvent`, but its public `ToastMessage` header exposes only `mType` at offset 0x0C. `ToastMessageType::Achievement` exists, but the public API does not expose an advancement ID or completion payload.
6. LeviLaunchroid/Preloader exposes touch callbacks and native draw commands, but the current Advancements project used a floating external `ButtonBuilder` button. That launcher overlay path is why the button was visible outside the pause screen. The NMDK JSON UI button route should replace it.
7. The current Advancements catalog was compared against the original `/tmp/adv-upstream/Advancements/data/advancements` data: Story 16, Nether 23, End 9, Adventure 47, Husbandry 31. No catalog IDs were missing.
8. Live completion state cannot be reliably recovered from the public NMDK headers alone. The existing `gCompleted` vector is initialized false. A safe next step is to consume achievement toasts only if a stable achievement ID/payload is recovered; otherwise a direct client advancement-state hook or server-to-client state bridge is required. Do not mark all entries completed as a false fix.
