# SOTE Build Dependencies Analysis

## Missing Dependencies Report

### Critical Missing Dependencies

#### DirectX SDK
- **Status**: LIKELY MISSING
- **Required For**: DirectDraw functionality (ddraw.h, ddraw.lib)
- **Impact**: Build will fail without DirectX headers
- **Resolution**: 
  - Install DirectX SDK for Windows
  - Alternative: Use modern Windows SDK with legacy DirectX support
  - MinGW users: Install mingw-w64-i686-directx-headers package

#### Cross-compilation Tools (if building on non-Windows)
- **Status**: PLATFORM DEPENDENT
- **Required For**: Building Win32 executable on Linux/macOS
- **Impact**: Native build tools may not support Win32 target
- **Resolution**:
  - Install MinGW-w64 cross-compiler
  - Use Wine for Windows-only tools
  - Set up cross-compilation toolchain

### Optional Missing Dependencies

#### Static Analysis Tools
- **Tool**: cppcheck
- **Status**: OPTIONAL
- **Impact**: No static analysis during build
- **Resolution**: `apt-get install cppcheck` or equivalent

#### Resource Compiler
- **Tool**: windres
- **Status**: NOT CURRENTLY USED
- **Impact**: Cannot compile Windows resources (if added later)
- **Resolution**: Included with MinGW-w64

### Build Environment Validation

#### Required Compilers
- [x] GCC with i386 support (`gcc -m32`)
- [x] Standard C library
- [ ] DirectX headers (ddraw.h)
- [ ] Windows headers (windows.h) - typically available

#### Required Libraries (Link-time)
- [x] kernel32 (usually available)
- [x] user32 (usually available)
- [x] gdi32 (usually available)
- [ ] ddraw (requires DirectX SDK)
- [x] winmm (usually available)
- [x] advapi32 (usually available)
- [x] shell32 (usually available)
- [x] comctl32 (usually available)
- [x] comdlg32 (usually available)
- [x] winspool (usually available)

### Platform-Specific Issues

#### Windows (Native)
- **Missing**: Potentially none if Visual Studio or MinGW-w64 installed
- **Action**: Verify DirectX SDK installation

#### Linux (Cross-compilation)
- **Missing**: Win32 cross-compiler, DirectX headers
- **Action**: 
  ```bash
  sudo apt-get install gcc-mingw-w64-i686
  sudo apt-get install mingw-w64-i686-dev
  # For DirectX headers (if available):
  sudo apt-get install mingw-w64-i686-directx-headers
  ```

#### macOS (Cross-compilation)
- **Missing**: Win32 cross-compiler, DirectX headers
- **Action**:
  ```bash
  brew install mingw-w64
  # DirectX headers may need manual installation
  ```

### Dependency Resolution Commands

#### Ubuntu/Debian
```bash
# Install cross-compiler
sudo apt-get install gcc-mingw-w64-i686 g++-mingw-w64-i686

# Install development tools
sudo apt-get install make pkg-config

# Install DirectX headers (if available)
sudo apt-get install mingw-w64-i686-directx-headers || echo "DirectX headers not available via package manager"
```

#### Fedora/CentOS
```bash
# Install cross-compiler
sudo dnf install mingw32-gcc mingw32-gcc-c++

# Install development tools  
sudo dnf install make pkg-config

# DirectX headers may need manual installation
```

#### Windows (MSYS2/MinGW-w64)
```bash
# Install toolchain
pacman -S mingw-w64-i686-gcc
pacman -S mingw-w64-i686-make

# Install DirectX headers
pacman -S mingw-w64-i686-directx-headers
```

### Manual DirectX SDK Installation

If DirectX headers are not available via package manager:

1. Download DirectX 7 SDK or later
2. Extract headers to appropriate directory
3. Update Makefile CPPFLAGS to include DirectX header path:
   ```makefile
   CPPFLAGS += -I/path/to/directx/include
   ```
4. Update LDFLAGS to include DirectX library path:
   ```makefile
   LDFLAGS += -L/path/to/directx/lib
   ```

### Build Testing

To test if dependencies are satisfied:

```bash
# Check basic compilation
make check-deps

# Attempt to build (will fail at link stage if libraries missing)
make clean
make all

# Test specific missing dependencies
gcc -m32 -E -dM - < /dev/null | grep -i directx
pkg-config --exists ddraw || echo "DirectX pkg-config not found"
```

### Alternative Build Strategies

#### Option 1: Stub DirectX Implementation
- Create minimal ddraw.h stub for compilation
- Implement DirectDraw calls as no-ops for testing
- Limited functionality but allows build completion

#### Option 2: Modern DirectX
- Port DirectDraw calls to Direct3D 11/12
- Requires significant code modification
- Better long-term compatibility

#### Option 3: SDL Replacement
- Replace DirectDraw with SDL2
- Cross-platform compatibility
- Requires moderate code modification

### Conclusion

The primary blocking dependency is the DirectX SDK for DirectDraw support. All other dependencies are typically available in standard development environments. Cross-compilation setups require additional toolchain installation but are well-supported by MinGW-w64.

**Recommendation**: Install MinGW-w64 with DirectX headers as the most straightforward solution for building SOTE.