# Assignments

**Note: You are expected to submit each step of the assignment**

### 1. [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set)

  a. Create a pixel buffer of size 800x600 and clear it to green color (0, 0, 255) and write it to BMP file named `image.bmp`.
BMP is a very simple image storage format. You can implement the features of BMP you need to write a BMP file using the [reference here](https://docs.microsoft.com/en-us/windows/win32/gdi/bitmap-storage). If you do not want to implement the BMP writer yourself, @Pramish-Aryal has a very easy to integrate and use [single header library here](https://github.com/Pramish-Aryal/pimp).

  b. Plot a mandelbrot set in the pixel buffer. There are [various algorithms](https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set) to implement the mandelbrot set. The pesudo code for one of the simplest method to plot mandelbrot set is as follows (for more details read, wikipedia page):
![MandelbrotPseudoCode](./MandelbrotPseudoCode.png)

For color palette, use the following values: 
```
Position = 0.0     Color = (  0,   7, 100)
Position = 0.16    Color = ( 32, 107, 203)
Position = 0.42    Color = (237, 255, 255)
Position = 0.6425  Color = (255, 170,   0)
Position = 0.8575  Color = (  0,   2,   0)
```
Here, you will need to map the iteration value, which ranges from 0 to you maximum iteration count to 0 to 1. Use linear map function to map the range from 0 to max iteration into 0 to 1.
Then use [linear interpolation](https://en.wikipedia.org/wiki/Linear_interpolation#Programming_language_support) to blend the color between various position values. 

  c. Create another thread and plot the mandelbrot set in another thread. In the main thread, you just wait for the child thread to finish rendering, after the child thread finishes rendering, the main thread should writes the pixel buffer to BMP file.

  d. Create a shared variable of type `float` and while the child thread is rendering the mandelbrot set, it should also write the amount of progress done in that shared variable. Here progress can be something along the lines of:
```
progress = pixels_color / total_number_of_pixels;
```
The main thread should print progress in the console in regular intervals of time. You may want the main thread to sleep for few milliseconds before it reports the progress in the console, so that the main thread is not using 100% of a core.

  e. Create an optimal number of threads for your CPU (optimal number of threads == the number of cores present in your processor) and give each thread different parts of mandelbrot set to render.

Hint: Create a shared data structure were you can put the input parameters to calculate the color of a single pixel, and each thread can read from that data structure and color the single pixel.
    - Windows: https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsysteminfo
    - Linux: https://man7.org/linux/man-pages/man3/sysconf.3.html

The main thread should still print progress of the plot.

  f. Convert the program such that it takes in the number of threads it should create to plot the mandelbrot set. If the input number of threads is given as 1, then only a single woker thread should render the entire mandelbrot set.

  g. Insert performance counters in your program to calculate the amount of time it requires to plot the mandelbrot set.
    - Windows: https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
    - Linux: https://man7.org/linux/man-pages/man3/clock.3.html

  h. Increase the number of threads gradually and observe the performance of the program. Record these data and place them in a table as such:
```
Threads, Performance
<N>, <Time>
....
```
Do this for 2 times the number of cores present in your processor.

  i. (**OPTIONAL**) Create a window and show the pixel buffer in the main thread as they are being plotted by the child threads.

Reference Links(Windows):
- Creating Window: https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window
- Rendering to Window: https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchdibits

Reference Links(SDL/Linux): https://www.libsdl.org/



### 2. Multithreading Merge Sort

  a. Read an input file named `input.txt` which will have  N numbers of type `uint32_t`. Sort them using merge sort in ascending or descing order based on the input parameters. Write the result to `sorted.txt`.
The input file will begin the number N which is the number of input numbers which are to be sorted, followed by either 'a' or 'd', where a = acsending and d = descending. After 'a' or 'd', the file will have N numbers. 
Example of input file (there may be newlines in input file):
```
4 a 5 90 74 54
```
Example of output file (there shall be no newlines in output file):
```
5 54 74 90
```
Hint: In order to check if the data is correct or not, you can use [qsort](https://en.cppreference.com/w/c/algorithm/qsort) from the C runtime library and check if the sorted data are same or not.

  b. Create a total of 3 threads (excluding the main thread) and use those threads to sort the data using merge sort.
Hint: Each `Merge` in the Merge sort can be done in separated thread. You may need to create a shared data structure from where the threads get the data to be merged.

The code to generate data has been included in the [repository here](./numbers.c)
