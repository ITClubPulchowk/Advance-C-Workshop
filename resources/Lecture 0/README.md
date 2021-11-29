# Lecture 0

## Syllabus: Alignment, Paddign and Heap Allocation

* Endianness (Little vs Big) 
* Padding
* Structure alignment and Padding
* Bitfields
* Structure Reordering
* Readibility and cache locality
* Overriding alignment rules
* C operators (alignof and alignas)
* Review of Compilation and Assembly Code
* Memory Segmentation
  * Code Segment
  * Data Segment
  * BSS Segment
  * Stack
  * Heap
* Pointer Alignment

## Assignments

1. Read from a file that consists of details required to draw rectangles (x, y, width, height) and circles (x, y, radius) into a [ppm file](https://en.wikipedia.org/wiki/Netpbm#PPM_example).
   The input file is supposed to have the following syntax:
   ```
   image_width image_height
   c center_x center_y radius r g b
   r center_x center_y width height r g b
   ```
   As an example:
   ```
   800 800
   c 100 100 55 255 255 255
   r 250 600 50 30 255 0 255
   r 350 500 50 30 0 255 0
   c 20 20 5 255 0 0
   c 300 300 150 0 0 255
   r 450 400 50 30 100 100 100   
   c ... ... ...
   c ... ... ...
   ```
   The given details ask you to create an image of dimensions 800x800 and draw a circle of radius 55 at (100, 100) of color rgb(255, 255, 255) a rectangle 50x30 at (250, 600) of color rgb(255, 0, 255) and so on.

   ***note**: you might not be able to view the .ppm file out of the box in windows, and might need to resort to 3rd party apps.*

2. Read [postfix](https://en.wikipedia.org/wiki/Reverse_Polish_notation) instructions from a given file, find the result and write to results.txt.
   You can assume you're provided with a file, "expressions.txt" in the form
   ```
    5 6 + 5 -
    1 3 * 4 +
    ......
    ......
   ```
   and you need to generate the answers in "results.txt" in the form,
   ```
   6.000000
   7.000000
   ```
   ***note** : use floats to store the results since there can be division operations as well.*
