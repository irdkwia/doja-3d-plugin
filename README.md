# Z3D DoJa emulator plugin recreation

## Purpose

This project aims at recreating the `j3d2.dll` to be used with DoJa SDK emulator (3.0 or 3.5),
which enables Z3D capabilities of the emulator.

Note: `doja_g.exe` requires `j3d2_g.dll` instead (which can just be the same DLL as `j3d2.dll`)

All libraries (`j3d2.dll` AND additional libraries) must be put inside your DoJa emulator `bin` folder (where `doja.exe` is located)

## Compile

This project can be compiled via Visual Studio https://visualstudio.microsoft.com/

NuGet packages must be installed for compilation to work.

## Additional libraries needed

This project uses Mesa 3D x86 dynamic libraries to implement OpenGL: https://mesa3d.org/

**THESE LIBRARIES ARE REQUIRED FOR THIS COMPILED DLL TO WORK!**

You can find pre-compiled DLLs of mesa here: https://github.com/pal1000/mesa-dist-win

Recommended version: https://github.com/pal1000/mesa-dist-win/releases/tag/25.1.0

Choose "release msvc" or "release mingw" and copy all files from the x86 folder inside the archive.

## Credits

`EGL\egl.h`, `EGL\eglplatform.h`, `GL\glext.h`, `KHR\khrplatform.h` header files are from the khronos registry (https://registry.khronos.org/)

`GL\gl.h` header file from Mesa 3D (https://mesa3d.org/)

ZLib package used for gzip decompression (https://zlib.net/)