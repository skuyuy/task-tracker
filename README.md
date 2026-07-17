# C Task Tracker

Task Tracker command line utility written in C to practice the basic language concepts.

This project makes use of:
* Basic C language concepts
* Unity builds
* C Standard Library

## Building

This program uses a **unity build**, which means that the main file excludes all headers at the top and all C-files at the bottom. 

Therefore, a single large compilation unit is formed which is compiled into an executable (perfect for small projects in my opinion).

You only need to compile *task_tracker.c* (where `main` resides) with the compiler of your choice. 
> I personally use Clang: `clang -g task_tracker.c -o build/task-tracker.exe` (`-g` for debug symbols)
