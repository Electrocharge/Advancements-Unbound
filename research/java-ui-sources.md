# Java UI reference sources

The experimental Java UI style follows the vanilla Java advancement screen and toast conventions described by the Minecraft Wiki:

- https://minecraft.fandom.com/wiki/Advancement
- https://minecraft.wiki/w/Java_Edition_GUI_textures
- https://github.com/DaFuqs/PaginatedAdvancements

The first source documents the five advancement tabs, the advancement screen, the top-right completion toast, the chat announcement behavior, and the `announceAdvancements` convention. The second source identifies the vanilla Java texture names used by this port: `Advancements_gui`, the five advancement backgrounds, obtained/unobtained frames, and toast boxes. The downloaded textures are stored under `assets/java-ui/` and embedded into `include/advancements/java_ui.hpp` for the experimental renderer.
