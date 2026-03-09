![Status](https://github.com/dimosikrus/Taikos/actions/workflows/multi-os-ci.yml/badge.svg)
# Taikos
> Support Osu! Beatmaps (only Taiko, only folders) :3
>
## Build
> `Build now can only on Windows x64` `Linux / MacOS configuration Not configured in Cmake`
> In Main Directory Project
> Debug
> ```bash
> cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
> cmake --build build --config Debug
> ./build/bin/Debug/Taikos.exe
> ```
> Release
> ```bash
> cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
> cmake --build build --config Release
> ./build/bin/Debug/Taikos.exe
> ```
> 
## Known Issues
> If Game show you error with error vcruntime / ucrtbase / msvcp / etc.
> Just copy .dll files from `./dlls32bit/Debug` or `./dlls32bit/Release`
## Controls
> ```
> ESC                      - back (or pause/unpause ingame)
> Q                        - leave from beatmap
> D, K                     - Blue (or yellow / magenta)
> F, J                     - Red  (or yellow / magenta)
> LCtrl + Mouse Scroll+/-  - HitSounds Volume (HitSounds hard linked on dfjk)
> LAlt  + Mouse Scroll+/-  - Music Volume
> E                        - Restart beatmap
> Scroll (in Song Select)  - Scroll Beatmaps
> Up/Down                  - Scroll Beatmaps
> ```
## TODO
```diff
+ Settings
- Fix Volume
+ Conveyor
+ Scrolling Speed
+ BackGrounds
+ New Song Select Menu
+ Rework Engine
+ Cmake builds on another systems (Linux / MacOS)
+ Stars / Performance Calculator
+ Accuracy Counter
```
## Images
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/f7994f79-fb7b-4793-93f1-c4337e7e3a0c" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/00c18d9c-ac0e-4c72-9836-4d81ce70b3f7" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/a8813275-9924-4216-9f81-107d56fc4cbc" />
<img width="1280" height="720" alt="image" src="https://github.com/user-attachments/assets/7895a1c7-b0aa-40f7-9dce-64f1c93697bd" />
