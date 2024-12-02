/// 
//  File: mandel.c
//  Author: Braydon H
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include <string.h>
#include <math.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max);

static void show_help();





int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	char   *outfile = "mandel";
	double xcenter = -0.563;
	double ycenter = -0.645;
	double xscale = 4;
	int    children = 1;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;


	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:c:o:h"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'c':	// added this to allow number of children to be entered
				children = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

    // Process to create images 
    int total_images = 50;
    int images_per_child = total_images / children;

    // Assign Process
    pid_t pid;
	double scales[total_images];
    for (int i = 0; i < total_images; i++) {
        //gets an equal incrementation for the array of scales
		double temp = (double) i  / (total_images - 1);
		// creates how fast the rate of x and y scale are
		scales[i] = xcenter * pow(0.1, temp);
    }


	for(int i = 0; i < children; i++) {
			pid = fork();
			if(pid == 0) {      // child
				int start = i * images_per_child;
				int end = (i == children - 1) ? total_images : start + images_per_child;
				// for loop to make all images
				for(int k = start; k < end; k++) {
					// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
					yscale = (scales[k] * (double)(image_width * image_height) / 1000000) / 2;
					xscale = (scales[k] * (double)(image_width * image_height) / 1000000) / 2;

					printf("mandel%d: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n", k, xcenter,ycenter,xscale,yscale,max,outfile);

					// Create a raw image of the appropriate size.
					imgRawImage* img = initRawImage(image_width, image_height);

					// Fill it with a black
					setImageCOLOR(img,0);

					// Compute the Mandelbrot image
					compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max);

					// Allocate space for the output file name, considering the base name and extension
					char outfile_buffer[256];  // Adjust size as needed

					// Create the output filename, appending the index and ".jpeg"
					snprintf(outfile_buffer, sizeof(outfile_buffer), "%s%d.jpg", outfile, k+1);

					// Now, use the formatted filename
					storeJpegImageFile(img, outfile_buffer);
				}
				exit(1);
			}
		}
		// wait for all of the children to be complete
		for(int i = 0; i < children; i++) {
			wait(NULL);
		}
		return 0;
	}




/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max)
{
	int i,j;

	int width = img->width;
	int height = img->height;

	// For every pixel in the image...

	for(j=0;j<height;j++) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);

			// Set the pixel in the bitmap.
			setPixelCOLOR(img,i,j,iteration_to_color(iters,max));
		}
	}
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}