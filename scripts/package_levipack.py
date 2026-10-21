import argparse
import json
import zipfile
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("--library", type=Path, required=True)
p.add_argument("--icon", type=Path, required=True)
p.add_argument("--output", type=Path, required=True)
a = p.parse_args()

manifest = {
    "type": "preload-native",
    "name": "Advancements",
    "author": "Electrocharge",
    "description": "Minecraft-style advancement trees with category icons, labels, descriptions, and LeviLamroid pause-menu integration.",
    "version": "0.9.2",
    "entry": "libAdvancementsLeviLamroid.so",
    "icon": "icon.jpg",
    "minecraft_versions": ["1.26.5*.**"],
    "dependencies": ["levilamroid"],
    "overwrite_files": ["icon.jpg"],
    "overwrite_folders": []
}

a.output.parent.mkdir(parents=True, exist_ok=True)
with zipfile.ZipFile(a.output, "w", zipfile.ZIP_DEFLATED, 9) as z:
    z.writestr("manifest.json", json.dumps(manifest, indent=2) + "\n")
    z.write(a.library, "libAdvancementsLeviLamroid.so")
    z.write(a.icon, "icon.jpg")
