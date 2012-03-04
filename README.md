# README for COMP 428: Parallel Programming #
# Winter 2012 Assignment 2 #

## Requirements
1. Each submission has to be accompanied by a MakeFile.
2. For Q1: Source file name should be: psrs.cpp or psrs.c . After compiled using MakeFile, the name of the executable file must be psrs. Any other name except the mentioned will not be considered.
3. For Q2: source file name should be: pqsort.cpp or pqsort.c . After compiled using MakeFile, the name of executable file must be pqsort. Any other name except the mentioned will not be considered.
4. Program must be well commented.
5. Report must be submitted in pdf format only. Name of file: assignment2.pdf
6. To compare performances of Sequential and Parallel versions, both must use same time measurement unit (e.g., one cannot be in cycles and other in ms). Time unit should be in milliseconds.
7. Program has to take input from an input file and produce the output to an output file.
8: Input file name: input.txt
9. Input file Format: Input file contains integers separated by comma (,)
10. Output file name: output.txt
11. Output Format: sorted numbers are separated by comma (,)

## Compiling ##

*Release*

`make release`

*Debug*

`make debug`

Executables will be in the `bin` folder.

## Generate numbers ##

There is a script in the `tools` folder to help you generate numbers.

* To generate 100 numbers between 0 and 1000

`tools/generatenumbers.sh`

* To generate N numbers between 0 and 1000

`tools/generatenumbers.sh N`

* To generate N numbers between 0 and Z

`tools/generatenumbers.sh N Z`

* To generate N numbers between X and Z

`tools/generatenumbers.sh N Z X`
