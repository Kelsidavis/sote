#!/usr/bin/env python3
import os, sys, json, struct, hashlib, pathlib, subprocess
from datetime import datetime

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SDATA = os.environ.get("SDATA", os.path.join(ROOT, "Sdata"))
OUT   = os.environ.get("OUT",  os.path.join(ROOT, "assets_extracted"))
os.makedirs(OUT, exist_ok=True)

def sha1(p):
    h=hashlib.sha1()
    with open(p,"rb") as f:
        for b in iter(lambda:f.read(1<<20), b""): h.update(b)
    return h.hexdigest()

def have(cmd):
    return subprocess.call(["bash","-lc", f"command -v {cmd} >/dev/null 2>&1"]) == 0

# Prefer 'wrestool' (icoutils) or 'winedump' for resources; fall back to pefile if present.
def extract_with_wrestool(dll, outdir):
    # Extract all resources (RT_RCDATA/RT_BITMAP/RT_WAVE/etc.)
    # wrestool -x -o outdir -t rcdata -t bitmap -t cursor -t icon -t version <dll>
    types = ["rcdata","bitmap","wave","cursor","icon","group_icon","version"]
    os.makedirs(outdir, exist_ok=True)
    for t in types:
        subprocess.call(["bash","-lc", f"wrestool -x -o {outdir} -t {t} '{dll}' >/dev/null 2>&1 || true"])
    return True

def extract_with_winedump(dll, outdir):
    os.makedirs(outdir, exist_ok=True)
    # Dump list, then use wrestool if available; otherwise keep the map for manual parsing.
    subprocess.call(["bash","-lc", f"winedump -j resource '{dll}' > '{outdir}/winedump_resource.txt' 2>/dev/null || true"])
    return os.path.exists(os.path.join(outdir,"winedump_resource.txt"))

def main():
    dlls = sorted([p for p in pathlib.Path(SDATA).glob("data*.dll") if p.is_file()])
    manifest = []
    for dll in dlls:
        tag = dll.stem  # data00
        outdir = os.path.join(OUT, tag)
        os.makedirs(outdir, exist_ok=True)
        ok = False
        if have("wrestool"):
            ok = extract_with_wrestool(str(dll), outdir)
        elif have("winedump"):
            ok = extract_with_winedump(str(dll), outdir)
        else:
            print(f"[warn] no wrestool/winedump; add pefile extraction if needed", file=sys.stderr)
        files = []
        for r,_,fs in os.walk(outdir):
            for f in fs:
                if f == "winedump_resource.txt": continue
                p = os.path.join(r,f)
                files.append(os.path.relpath(p, outdir))
        manifest.append({
            "dll": str(dll),
            "tag": tag,
            "outdir": os.path.relpath(outdir, OUT),
            "files": sorted(files),
            "sha1": sha1(str(dll))
        })
    with open(os.path.join(OUT,"manifest.json"),"w") as f:
        json.dump({"generated": datetime.utcnow().isoformat()+"Z", "entries": manifest}, f, indent=2)
    print(f"[extract] wrote manifest for {len(manifest)} DLLs to {OUT}/manifest.json")
if __name__ == "__main__":
    main()