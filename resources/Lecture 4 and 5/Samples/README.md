# Threading Samples
These are the sample code provided for the Lecture 3: Threading I of the Advanced-C-Workshop.
These programs are intended to be followed along with the provided note.

## Runner
You can use the `runner.c` program to run your program for **N** number of times. The command line arguments is as follows:
- Linux
```
./runner N <path_to_process>
```

- Windows
```
.\runner.exe N <path_to_process>
```

## Building

* Windows(MSVC):  Run `cl_build.bat` from Visual Studio Developer Prompt
* Windows(CLANG): Run `clang_win_build.bat` from command line
* Linux:          Run `linux_build.sh` from terminal (GCC and CLANG compilers supported)

The build for `runner.c` will be in the root directory itself (`runner` in linux and `runner.exe` in windows).
The build for other examples are in `debug` and `optimized` build. As the names of the directory themselves, the `debug` directory will contain unoptimized executable and `optimized` directory will contains optimized executables.
