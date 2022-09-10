# NES emulator

Little pet project to try and code a NES emulator.

This project is cross-platform and should work on Windows and Linux.

Based on the wonderful tutorial from OneLoneCoder: https://github.com/OneLoneCoder/olcNES

Close to completeness. Already possible to play some games from start to finish, with sound.

**-- DISCLAMER --**

There is no rom included in this project, except some test roms. You need to dump your original games to use them with this emulator.

## Dependencies (New exe, linux/windows)
- ImGui
- glfw
- OpenGL
- googletest
- RTAudio

## Implemented features
- Full support for offical opcodes for the 6502 CPU (and some illegal opcodes)
- Keyboard support
- NTSC roms only (PAL is not yet supported)
- Some mappers (list bellow)
- Basic debugging with "gotos" and disassembly
- Can play from start to finish some games.
- Basic load/save state
- Rom loading in UI
- Sound fully functional (except DMC interrupt)

## Full interface
<img src="images/debug.png">

## Supported mappers
| Mappers  | Game Example       | Video    |
| ----------------| ------------------- | -------- |
| 0               | Super Mario Bros.   | <img src="images/smb.gif" width="300" height="300" /> |
| 1               | The Legend of Zelda | <img src="images/zelda.gif" width="300" height="300" />  |
| 2               | Duck Tails          | <img src="images/ducktails.gif" width="300" height="300" />  |
| 3               | DonkeyKong Classics | <img src="images/dkclassics.gif" width="300" height="300" />  |
| 4               | Super Mario Bros. 3 | <img src="images/smb3.gif" width="300" height="300" />  |
| 40              | SMB: The Lost Levels| <img src="images/smb_lost_levels.gif" width="300" height="300" />  |
| 66              | Duck Hunt + SMB     | <img src="images/duckhunt.gif" width="300" height="300" />  |

## Compiling on Ubuntu
```
git clone --recursive https://github.com/Amathlog/nes-emulator.git
apt-get install cmake build-essentials clang
cd nes-emulator
mkdir build
cd build
cmake ..
make
```

## Running the program
```
./NesEmulatorGL <path_to_your_rom>
```

Controls:
| Keyboard | NES Controller |
| -------- | -------------- |
| Arrows   | D-Pad          |
| Z        | A              |
| X        | B              |
| A        | Select         |
| S        | Start          |
