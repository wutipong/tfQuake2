Overview
===

[![video as of 2024/03/19](http://img.youtube.com/vi/BRtTZugvs0I/0.jpg)](http://www.youtube.com/watch?v=BRtTZugvs0I)

This is a [The-Forge](https://github.com/ConfettiFX/The-Forge) port of the [vkQuake2](https://github.com/kondrak/vkQuake2), which is the official Quake 2 code v3.21 with Vulkan support and mission packs included. It use [SoLoud](https://github.com/jarikomppa/soloud) for audio playback.

Project Status
===

* Currently the demo level can runs with the UI still not rendering correctly. Some of the surface is not rendering correctly
* The framerate is superfast.
* The video playback is not working.
* `pak` file is not tested at the moment.

Building
===

Currently only Windows is supported as I have no access to other platform at the moment. The build system is CMake.

On Windows, The-Forge requires either Visual Studio 2017 or 2019. You can also use the Visual Studio Build Tools instead of the full IDE.

Running
===

After buiding the game, the build directory should contains  `game` directory and other expansion directory. Inside, there would be `gamex64.dll` files.

Extract the game files from the orginal game into the build directory, using tool like [PakExplorer](https://valvedev.info/tools/pakexplorer/). The game files from `game` directory should be under the same directory under the build directory, the sames goes to other expansion if needed.

## Music
For standard Quake 2, copy all tracks into the `baseq2/music` directory following the `trackXX.[ogg,flac,mp3,wav]` naming scheme (so track02.ogg, track03.ogg... for OGG files etc.). For "Ground Zero" and "The Reckoning", copy the tracks to `rogue/music` and `xatrix/music` directories respectively. 
