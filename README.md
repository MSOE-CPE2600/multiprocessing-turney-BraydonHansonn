## Name: Braydon Section: 121

## System Programming Lab 11 Multiprocessing

In this task, I created revised the mandel.c file that now uses multiple child processes to generate frames for a Mandelbrot set animation. The program forks child processes based on the user input specifying the number of processes (e.g., 1, 2, 5, 10, or 20).Each child process invokes the mandel program to generate a single frame of the Mandelbrot set by modifying parameters such as the scale and origin slightly for each frame.

## Screenshots

![App Screenshot](image.png)

## Discussion

The results show that increasing the number of child processes leads to a decrease in runtime. Initially, with 1 process, the program takes the longest time (21 seconds), while using 20 processes results in a much shorter runtime (3.3 seconds). This demonstrates the potential benefits of parallelism.

## Movie

![Movie](mandel.mpg)

