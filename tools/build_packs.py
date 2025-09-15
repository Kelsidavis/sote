#!/usr/bin/env python3
import os, sys, json, zipfile, pathlib, shutil
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
EXTRACTED = os.path.join(ROOT, "assets_extracted")
OUT = os.environ.get("PACK_OUT", os.path.join(ROOT, "assets_packs"))
os.makedirs(OUT, exist_ok=True)

# Heuristics map: put likely images/sounds/scripts into stable folders.
def classify(name):
    n = name.lower()
    if n.endswith((".bmp",".pcx",".tga",".png",".raw",".spr",".tim")): return "textures"
    if n.endswith((".wav",".voc",".ogg",".mp3")): return "audio"
    if n.endswith((".pal",".act",".clut")): return "palettes"
    if n.endswith((".bin",".dat",".scr",".lvl",".map",".chunk",".rcd",".rcdata")): return "data"
    return "misc"

def make_pack(tag):
    src = os.path.join(EXTRACTED, tag)
    if not os.path.isdir(src): return False
    meta = {"tag": tag, "files": []}
    pack_path = os.path.join(OUT, f"{tag}.sotepak")
    with zipfile.ZipFile(pack_path, "w", compression=zipfile.ZIP_DEFLATED) as z:
        for r,_,fs in os.walk(src):
            for f in fs:
                if f == "winedump_resource.txt": continue
                rel = os.path.relpath(os.path.join(r,f), src)
                bucket = classify(rel)
                arc = f"{bucket}/{rel.replace(os.sep,'/')}"
                z.write(os.path.join(src, rel), arc)
                meta["files"].append({"src": rel, "arc": arc, "bucket": bucket})
        z.writestr("metadata.json", json.dumps(meta, indent=2))
    return True

def main():
    tags = [p.name for p in pathlib.Path(EXTRACTED).glob("data*") if p.is_dir()]
    built = []
    for tag in sorted(tags):
        if make_pack(tag):
            built.append(tag)
    with open(os.path.join(OUT,"packs.index.json"),"w") as f:
        json.dump({"packs": built}, f, indent=2)
    print(f"[packs] built {len(built)} packs into {OUT}")
if __name__=="__main__":
    main()