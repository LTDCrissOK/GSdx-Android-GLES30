# GSdx Android GLES 3.0 integration module

This directory is an independent Android integration foundation for the original PCSX2 GS sources. It does not replace or modify the desktop OpenGL renderer.

## Target
- Android NDK and CMake
- OpenGL ES 3.0 / GLSL ES 3.00
- ARM32 (`armeabi-v7a`) and ARM64 (`arm64-v8a`)
- EGL window context supplied by the Android host

## Implemented in this initial commit
- `AndroidGLESContext`: creates an EGL OpenGL ES 3 context and Android window surface, makes it current, swaps buffers, and releases EGL resources.
- CMake static-library target linking Android, EGL, GLESv3, and log.

## Status: integration foundation, not a complete GSdx renderer

The original GSdx source remains under `PCSX2-GS/pcsx2/GS`. Its desktop OpenGL renderer cannot simply be compiled against GLES headers. The current upstream implementation contains desktop-only or optional desktop paths (for example persistent buffer storage, desktop DSA entry points, desktop texture readback, and desktop GLSL variants). These must be replaced while preserving GS behavior.

PS2Recomp's current interface is in `ps2xRuntime/include/runtime/gs/gs_backend.h`; its data contracts are in `gs_types.h`. The `GSRasterBackend` adapter still needs to implement all operations: primitive submission, CLUT, image transfers, VRAM read/write and snapshot, flush/synchronization, framebuffer clear, and presentation. This foundation deliberately does not include a fake adapter that reports successful rendering without implementing those semantics.

## Required next implementation stages
1. Port GSdx's device layer into a separate GLES device implementation; keep original files intact.
2. Convert and validate all GSdx shaders as GLSL ES 3.00.
3. Replace desktop-only buffer, texture, framebuffer, readback, sync, and state APIs with GLES 3.0 equivalents or explicit fallbacks.
4. Preserve PS2-specific blending, alpha test/DATE, framebuffer masks, depth formats, CLUT, swizzle, and transfer behavior.
5. Implement the adapter against the exact checked-in PS2Recomp headers and connect it to the runtime's backend selection.
6. Build ARM32 and ARM64 and test on real GLES 3.0 Android devices.

## Build configuration

Build this directory from an Android NDK CMake project. Configure both ABIs in Gradle:

    android {
        defaultConfig {
            ndk { abiFilters 'armeabi-v7a', 'arm64-v8a' }
        }
    }

The same C++ sources are used for both ABIs. ABI compilation does not guarantee a device's GPU driver supports GLES 3.0; check the actual EGL context and GL_VERSION at runtime.

## Licensing

The original PCSX2 GS sources carry GPL-3.0-or-later notices. Preserve upstream copyright and license notices and check license compatibility before distributing a combined binary with PS2Recomp.