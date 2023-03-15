This is the Source code to the Skyrim Mod [NPCs Use Potions](https://www.nexusmods.com/skyrimspecialedition/mods/67489).
Bug reports and suggestions should be posted in the Bug / Forum section of the nexusmods page.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `SkyrimOutputPath` to point to the folder where you want to store your compiled Plugins.
* [The Elder Scrolls V: Skyrim VR](https://store.steampowered.com/app/611670)
	* For the VR Edition
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE)
	* use the `dev` branch to build CommonLibSSE and add its path to the environment variable `CommonLibSSEPath`
	* use the `dev-1.6.353` branch to build CommonLibSSE and add its path to the environment variable `CommonLibSSE353Path`
	* only when building for SE / AE
* [CommonLibVR](https://github.com/alandtse/CommonLibVR)
	* use the `vr` branch to build CommonLibVR and add its patch to the environment variable `CommonLibVRPath`
	* only when building for VR

## User Requirements
* [Skyrim Script Extender](https://skse.silverlock.org/)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
	* For the Special Edition or Anniversary Edition
* [VR Address Library for SKSEVR](https://www.nexusmods.com/skyrimspecialedition/mods/58101)
	* For the VR Edition

## Building for SSE / AE
```
git clone https://github.com/muenchk/NPCsUsePotions.git
cd NPCsUsePotions
cmake --preset ae 				# for Skyrim Anniversary Edition
cmake --build buildAE --config Release

cmake --preset ae353			# for Skyrim Anniversary Edition version 1.6.353
cmake --build buildAE353 --config Release

cmake --preset se				# for Skyrim Special Edition
cmake --build buildSE --config Release
```

## Building for VR
```
git clone https://github.com/muenchk/NPCsUsePotions.git
cd NPCsUsePotions
cmake --preset vr				# for Skyrim VR
cmake --build buildvr --config Release
```