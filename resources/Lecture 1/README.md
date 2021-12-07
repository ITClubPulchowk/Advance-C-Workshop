# Lecture 1 - DSA

## Syllabus:

* Array
* Hash Table
* Linked List
* Doubly Linked List
* Binary Tree
* Merge Sort
* Time and Space Complexity

## Assignments
___
### Merge Sort
Use merge sort to sort the data in **“mergesort-input.csv”** per line and output sorted array perline to **“mergesort-output.csv”**.

### Hash Table
Implement an **open chained** hashmap with **adler32** as hash function to solve the following problem.
In **airport_a.txt** you are given names of 15 airports. In **airport_b.csv** you are given pairs of airports.
“Bhairahawa”,”Kathmandu” means a flight leaves bhairahawa to kathmandu. Calculate the city with **the highest number of incoming flights**, put it in **airport_output.csv** and follow it with a list of incoming airports. Each airport must be enclosed in double quotes and comma separated.

 Miniature example:

| Start     |Destination |
|-----------|------------|
| Pokhara   | Kathmandu  |
| Pokhara   | Jiri       |
| Pokhara   | Jomsom     |
| Kathmandu | Pokhara    |
| Kathmandu | Jomsom     |

Only Jomsom has 2 incoming flight. It wins.
airport_output.csv should be:
```
2,”Pokhara”.”Kathmandu”
```

### Binary Tree

Given a list of **polynomial expressions** in **polynomial_derivative.txt**, calculate the **indefinite derivative** and sort the terms in ascending order of power. And write it to **polynomial_derivative_out.txt**.

**Input file:**
```
5x^2 + 3x^3 + 9x +5
6x^3+ 5x^5 + 3x^3
```

**Output file:**
```
9x^2+10x+9
25x^4 + 27x^2
```

Create an infix calculator. Input will be as expressions per line in “infix_input.txt”. Output results perline in infix_output.txt.

**Input file**
```
5*7 + 9/3 + 1/2
6-12*12+1/3
```

**Output file**
```
24.5
-133.666….
```

## Resources
- Slide: https://tunepal-my.sharepoint.com/:p:/g/personal/rujal11_762419_puc_tu_edu_np/EZtfjYb12T5DsIxiZt0l57wB6u3z12dELVb6DVKQhwmGiw?rtime=lmYmCESz2Ug

### Linked Lists
- Basics: http://cslibrary.stanford.edu/103/LinkedListBasics.pdf
- Application: http://www.cs.ucf.edu/courses/cop3502h.02/linklist3.pdf

- Binary Trees: http://cslibrary.stanford.edu/110/BinaryTrees.pdf

### Merge Sort
- Algorithm Visualizations: http://algoanim.ide.sk/index.php?page=showanim&id=118
- Mergesort Concepts: https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/Mergesort.html

### Hashmap
- Open Hashing: https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/OpenHash.html
- Bucket Hashing: https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/BucketHash.html

