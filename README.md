This is the Source code to the Skyrim Mod [NPCs Use Potions](https://www.nexusmods.com/skyrimspecialedition/mods/67489).
Bug reports and suggestions should be posted in the Bug / Forum section of the nexusmods page.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [The Elder Scrolls V: Skyrim Special Edition](https://store.steampowered.com/app/489830)
	* Add the environment variable `SkyrimOutputPath` to point to the folder where you want to store your compiled Plugins.
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [CommonLibSSE](https://github.com/powerof3/CommonLibSSE)
	* use the `dev` branch tu build CommonLibSSE and add its path to the environment variable `CommonLibSSE`

## User Requirements
* [Skyrim Script Extender] (https://skse.silverlock.org/)
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## Building
```
git clone https://github.com/muenchk/NPCsUsePotions.git
cd NPCsUsePotions
cmake --preset ae 			# for Skyrim Anniversary Edition
cmake --preset oldversion	# for Skyrim Special Edition
cmake --build build --config Release
```