#!/usr/bin/env python3
import os, sys, json, struct, hashlib, pathlib
from datetime import datetime

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
SDATA = os.environ.get("SDATA", os.path.join(ROOT, "Sdata"))
OUT = os.environ.get("OUT", os.path.join(ROOT, "assets_extracted"))
os.makedirs(OUT, exist_ok=True)

def sha1(p):
    h=hashlib.sha1()
    with open(p,"rb") as f:
        for b in iter(lambda:f.read(1<<20), b""): h.update(b)
    return h.hexdigest()

def extract_pe_data_section(pe_file, outdir):
    """Extract the .data section from a PE file which may contain embedded assets."""
    os.makedirs(outdir, exist_ok=True)

    with open(pe_file, "rb") as f:
        # Read DOS header
        dos_header = f.read(64)
        if dos_header[:2] != b'MZ':
            return []

        # Get PE header offset
        pe_offset = struct.unpack('<I', dos_header[60:64])[0]
        f.seek(pe_offset)

        # Read PE signature
        pe_sig = f.read(4)
        if pe_sig != b'PE\x00\x00':
            return []

        # Read COFF header
        coff_header = f.read(20)
        machine, num_sections = struct.unpack('<HH', coff_header[:4])
        size_of_opt_header = struct.unpack('<H', coff_header[16:18])[0]

        # Skip optional header
        f.seek(pe_offset + 24 + size_of_opt_header)

        # Read section headers
        sections = []
        for i in range(num_sections):
            section_header = f.read(40)
            name = section_header[:8].rstrip(b'\x00').decode('ascii', errors='ignore')
            virtual_size, virtual_addr, raw_size, raw_offset = struct.unpack('<IIII', section_header[8:24])
            sections.append((name, virtual_size, virtual_addr, raw_size, raw_offset))

        # Extract .data section
        extracted_files = []
        for name, vsize, vaddr, rsize, roffset in sections:
            if name == '.data' and rsize > 1024:  # Only extract if substantial data
                f.seek(roffset)
                data = f.read(rsize)

                # Save raw data section
                data_file = os.path.join(outdir, f"{name}_section.bin")
                with open(data_file, "wb") as df:
                    df.write(data)
                extracted_files.append(f"{name}_section.bin")

                # Try to identify embedded files by magic bytes
                offset = 0
                file_count = 0
                while offset < len(data) - 16:
                    # Look for common file signatures
                    chunk = data[offset:offset+16]

                    # BMP signature
                    if chunk.startswith(b'BM'):
                        try:
                            size = struct.unpack('<I', chunk[2:6])[0]
                            if 54 <= size <= len(data) - offset:
                                bmp_data = data[offset:offset+size]
                                bmp_file = os.path.join(outdir, f"embedded_{file_count:03d}.bmp")
                                with open(bmp_file, "wb") as bf:
                                    bf.write(bmp_data)
                                extracted_files.append(f"embedded_{file_count:03d}.bmp")
                                file_count += 1
                                offset += size
                                continue
                        except:
                            pass

                    # WAV signature
                    if chunk.startswith(b'RIFF') and chunk[8:12] == b'WAVE':
                        try:
                            size = struct.unpack('<I', chunk[4:8])[0] + 8
                            if size <= len(data) - offset:
                                wav_data = data[offset:offset+size]
                                wav_file = os.path.join(outdir, f"embedded_{file_count:03d}.wav")
                                with open(wav_file, "wb") as wf:
                                    wf.write(wav_data)
                                extracted_files.append(f"embedded_{file_count:03d}.wav")
                                file_count += 1
                                offset += size
                                continue
                        except:
                            pass

                    offset += 1

        return extracted_files

def main():
    dlls = sorted([p for p in pathlib.Path(SDATA).glob("data*.dll") if p.is_file()])
    manifest = []

    for dll in dlls:
        tag = dll.stem  # data00
        outdir = os.path.join(OUT, tag)
        files = extract_pe_data_section(str(dll), outdir)

        manifest.append({
            "dll": str(dll),
            "tag": tag,
            "outdir": os.path.relpath(outdir, OUT),
            "files": sorted(files),
            "sha1": sha1(str(dll))
        })

    with open(os.path.join(OUT, "manifest.json"), "w") as f:
        json.dump({"generated": datetime.now().isoformat(), "entries": manifest}, f, indent=2)

    total_files = sum(len(entry["files"]) for entry in manifest)
    print(f"[extract] wrote manifest for {len(manifest)} DLLs, {total_files} files extracted")

if __name__ == "__main__":
    main()