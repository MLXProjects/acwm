# acwm
an attempt to recreate the ClockWorkMod recovery UI using libaroma  
![acwm](https://user-images.githubusercontent.com/40145907/233492214-1bebc38a-60b7-4e0d-89ed-9f8d7f1bef16.png)

## It's you again?
yep, and I'll create random libaroma apps until I get tired of doing so lol

## Tell me about this
It's a GUI which tries to emulate the good old CWM recovery interface and some of it's functionality (like simple partition management or ZIP installing). Not much else, just the basics :)

## Well, why?
Some time ago I was gonna visit a friend and wanted to have some cool project to show, this is the result of very rushed C programming in 3 days (at least until source upload).

## Does something even work?
Currently "tested" features (not more than 2 devices already on recovery mode) are:
- partition related things like reading fstab, mount & format
- main UI menus are there and can be browsed :D
- writing to bottom screen log works (unfinished, single line for now)
- language support is not fully implemented but basics are there
- runs on Windows lol  

## What's missing?
A lot of things. If I had to think about some items:
- file selector
- ZIP flashing
- backup/restore
- emulated sdcard mount/handling (/data/media)
- almost all Advanced Menu items
- adb sideload
- /cache recovery commands & open recovery script
- reboot to normal and specific modes like bootloader/recovery
- rainbow mode (hey, if it was on CWM I want it here too)

At some point I would also like to support TWRP's fstab file.

## How to build?
Just like all my libaroma-based apps, the usual procedure:
- build & install [my libaroma fork](https://github.com/MLXProjects/libaroma)
- compile all C files in this repo linking to libaroma
- there you go

If you feel lazy doing things normal way, you can use my (stable enough yet WIP) classic build system to build libaroma and this app :) just check my [atools repo.](https://github.com/MLXProjects/atools)

## License
All my projects where I use libaroma are licensed under the Apache 2.0 license.
