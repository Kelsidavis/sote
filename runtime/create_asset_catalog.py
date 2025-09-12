#!/usr/bin/env python3

import os
import json
import hashlib
import subprocess
from pathlib import Path

def get_file_sha256(filepath):
    """Calculate SHA256 hash of a file"""
    hash_sha256 = hashlib.sha256()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_sha256.update(chunk)
    return hash_sha256.hexdigest()

def get_file_magic(filepath):
    """Get file magic/format using file command"""
    try:
        result = subprocess.run(['file', filepath], 
                              capture_output=True, text=True)
        return result.stdout.strip()
    except Exception as e:
        return f"error: {e}"

def determine_asset_kind(filename, magic_output):
    """Determine asset kind and codec hint based on filename and magic bytes"""
    filename_lower = filename.lower()
    
    if filename_lower.endswith('.bmp'):
        if 'PC bitmap' in magic_output:
            return 'bitmap', 'bmp_windows3x'
        else:
            return 'bitmap', 'bmp_unknown'
    
    elif filename_lower.endswith('.wav'):
        if 'RIFF' in magic_output and 'WAVE audio' in magic_output:
            if 'Microsoft PCM' in magic_output:
                return 'sound', 'riff_wave_pcm'
            else:
                return 'sound', 'riff_wave_unknown'
        else:
            return 'sound', 'wav_unknown'
    
    elif filename_lower.endswith('.san'):
        if 'LucasArts Smush Animation Format' in magic_output:
            return 'movie', 'lucasarts_san'
        else:
            return 'movie', 'san_unknown'
    
    elif filename_lower.endswith('.dll'):
        if 'PE32 executable (DLL)' in magic_output:
            return 'dll', 'pe32_dll'
        else:
            return 'dll', 'dll_unknown'
    
    elif filename_lower.endswith('.exe'):
        if 'PE32 executable' in magic_output:
            return 'dll', 'pe32_exe'  # Use dll kind for executable dependencies
        else:
            return 'dll', 'exe_unknown'
    
    else:
        return 'data', 'unknown'

def determine_container_type(filename, filepath):
    """Determine the container type based on file location"""
    if 'Sdata' in str(filepath):
        return 'sdata_file'
    elif 'Shadows.exe' in str(filepath):
        return 'pe_resource'
    else:
        return 'standalone_file'

def process_sdata_directory():
    """Process all files in Sdata directory"""
    base_dir = Path('/media/k/vbox1/Shadows')
    sdata_dir = base_dir / 'Sdata'
    
    # Load resource index to get referenced files
    resource_index_path = base_dir / 'SOTE' / 'runtime' / 'resource.index.json'
    referenced_files = set()
    
    try:
        with open(resource_index_path, 'r') as f:
            resource_index = json.load(f)
            
        # Extract referenced filenames from all categories
        for category in ['bitmap_references', 'movie_references', 'sound_references']:
            if category in resource_index:
                for ref in resource_index[category]:
                    if 'path_or_id' in ref:
                        referenced_files.add(ref['path_or_id'])
    except Exception as e:
        print(f"Warning: Could not load resource index: {e}")
    
    catalog_entries = []
    
    # Process all files in Sdata
    for file_path in sdata_dir.iterdir():
        if file_path.is_file():
            filename = file_path.name
            
            # Get file size
            file_size = file_path.stat().st_size
            
            # Get SHA256 hash
            sha256_hash = get_file_sha256(file_path)
            
            # Get magic file type
            magic_output = get_file_magic(file_path)
            
            # Determine kind and codec hint
            kind, codec_hint = determine_asset_kind(filename, magic_output)
            
            # Determine container
            container = determine_container_type(filename, file_path)
            
            # Create catalog entry
            entry = {
                "name": filename,
                "kind": kind,
                "codec_hint": codec_hint,
                "container": container,
                "size": file_size,
                "sha256": sha256_hash,
                "source": str(file_path.relative_to(base_dir)),
                "magic": magic_output,
                "referenced_in_index": filename in referenced_files
            }
            
            catalog_entries.append(entry)
    
    return catalog_entries

def create_resource_catalog():
    """Create the main resource catalog"""
    
    print("Processing Sdata directory...")
    catalog_entries = process_sdata_directory()
    
    # Sort by kind then by name
    catalog_entries.sort(key=lambda x: (x['kind'], x['name']))
    
    # Create catalog structure
    catalog = {
        "metadata": {
            "stage": "SOTE Stage 2: Ground Truth Asset Extraction",
            "timestamp": "2025-09-11T21:47:00Z",
            "tool": "file + sha256 + custom analysis",
            "source_directory": "Sdata/",
            "total_assets": len(catalog_entries),
            "schema_version": "1.0"
        },
        "statistics": {
            "by_kind": {},
            "by_container": {},
            "total_size_bytes": 0
        },
        "assets": catalog_entries
    }
    
    # Calculate statistics
    for entry in catalog_entries:
        kind = entry['kind']
        container = entry['container']
        
        catalog['statistics']['by_kind'][kind] = catalog['statistics']['by_kind'].get(kind, 0) + 1
        catalog['statistics']['by_container'][container] = catalog['statistics']['by_container'].get(container, 0) + 1
        catalog['statistics']['total_size_bytes'] += entry['size']
    
    return catalog

if __name__ == "__main__":
    catalog = create_resource_catalog()
    
    # Write catalog to file
    output_path = Path('/media/k/vbox1/Shadows/SOTE/runtime/resource.catalog.json')
    
    with open(output_path, 'w') as f:
        json.dump(catalog, f, indent=2)
    
    print(f"Resource catalog created: {output_path}")
    print(f"Total assets: {catalog['metadata']['total_assets']}")
    print("Asset kinds:", catalog['statistics']['by_kind'])
    print("Container types:", catalog['statistics']['by_container'])
    print(f"Total size: {catalog['statistics']['total_size_bytes']:,} bytes")