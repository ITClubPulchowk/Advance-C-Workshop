# MandelbrotSample

### Building
* Windows: Run `cl_build.bat` for MSVC or `clang_build.bat` for CLANG
* Linux: Run `build.sh`

### Configuration
- The default configuration renders 1280x720 framebuffer as BMP
- By default 16 threads are started which can be changed by supplying number of threads as command line argument
- Renders chunks (square partition of framebuffer) whose default size is 16

### Output
![MandelbrotOutput](./output.bmp)
