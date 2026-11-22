import argparse
import json
import zipfile
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("--library", type=Path, required=True)
p.add_argument("--icon", type=Path, required=True)
p.add_argument("--resource-pack", type=Path, required=True)
p.add_argument("--output", type=Path, required=True)
a = p.parse_args()

manifest = {
    "type": "preload-native",
    "name": "Advancements",
    "author": "Electrocharge",
    "description": "Minecraft-style advancement trees with category icons, labels, descriptions, and completion notifications.",
    "version": "1.1.5",
    "entry": "libAdvancements.so",
    "icon": "icon.jpg",
    "minecraft_versions": ["1.26.6*.**"],
    "dependencies": []
}

a.output.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(a.output, "w", zipfile.ZIP_DEFLATED, 9) as z:
    z.writestr("manifest.json", json.dumps(manifest, indent=2) + "\n")
    z.write(a.library, "libAdvancements.so")
    z.write(a.icon, "icon.jpg")
    for source in sorted(a.resource_pack.rglob("*")):
        if source.is_file():
            z.write(source, Path("resources/minecraft_resource_packs/advancements") / source.relative_to(a.resource_pack))
