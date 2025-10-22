#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"


////////////////////////////////////////
// Definitions of the functions       //
// declared in image_manip.h go here! //
////////////////////////////////////////

/* Convert a RGB pixel to a single grayscale intensity;
* uses NTSC standard conversion
*/
unsigned char pixel_to_gray( Pixel p ) {
	return (unsigned char)( 0.3 * p.r + 0.59 * p.g + 0.11 * p.b );
}

/* invert the intensity of each channel 
*/
Image invert(const Image in){

    assert(in.data);
    // subtracts each pixel value from 256 giving us the inverted intensity
	for (int i = 0; i < in.rows * in.cols; i++){ 
		
        in.data[i].r = 255 - in.data[i].r;
        in.data[i].g = 255 - in.data[i].g;
        in.data[i].b = 255 - in.data[i].b;
	}
	return in;
}

//______crop______
/* takes an image and points defining a bounding box,
* and crops that box out of the image, returning a newly
* created image containing just the cropped region
*/
Image crop( const Image in , Point top_left , Point bot_right ){

    // Assert to check that in.data is not null
    assert(in.data);

    // Calculate the new dimensions for the cropped image 
    int new_rows = bot_right.y - top_left.y;  
    int new_cols = bot_right.x - top_left.x;

    // Create a new image data struct because the const image data pointer cannot be pointed to a different image
    Image new_pic = {NULL, 0, 0};
    new_pic.rows = new_rows;
    new_pic.cols = new_cols;
    new_pic.data = (Pixel *)malloc(sizeof(Pixel) * new_rows * new_cols);  // Allocate a new data pointer for the a number of Pixel structs

    // Ensure data was allocated correctly
    if(new_pic.data == NULL){  
        fprintf(stderr, "Error: Memory allocation for new_pic in crop failed\n");
        return (Image) {NULL, 0, 0};
    }

    // Populate the new image data using pointer arthimetic and keeping it within the bounds; bounds will ensure it says within the 2nd point
    for (int i = 0; i < new_rows; i++){
        for (int j = 0; j < new_cols; j++){
            int orig_y = top_left.y + i;  // Calculate what the original points of the image would be
            int orig_x = top_left.x + j;

            int old_index = orig_y * in.cols + orig_x;  // Calculate the new and old indexes
            int new_index = i * new_cols + j;

            new_pic.data[new_index] = in.data[old_index];  // What Pixel Struct we're looking at
        }
    }
    return new_pic;  // Return the new image struct to be written to a new file
}


//______zoom_out______
/* "zoom out" an image, by taking a 2x2 square of pixels and averaging
* each of the three color channels to make a single pixel. If an odd
* number of rows in original image, we lose info about the bottom row.
* If an odd number of columns in original image, we lose info about the
* rightmost column.
*/
Image zoom_out( const Image in ){  
    assert(in.data);

    // Check if the image has odd rows or columns, as we will have to disregard them in the new dimensions
    int new_rows;
    int new_cols;
    if ((in.rows % 2) != 0){  // Meaning we have an odd value, so disregard the rightmost or bottom col/row
        new_rows = (in.rows - 1) / 2;
        new_cols = (in.cols - 1) / 2;
    } else {
        new_rows = (in.rows) / 2;
        new_cols = (in.cols) / 2;
    }

    // Create a new Image data struct
    Image zoom_pic = {NULL, 0, 0};
    zoom_pic.rows = new_rows;
    zoom_pic.cols = new_cols;
    zoom_pic.data = (Pixel *)malloc(sizeof(Pixel) * new_cols * new_rows); // Allocaite Pixel data with new dimensions 

    if (zoom_pic.data == NULL) {
        fprintf(stderr, "Error: Memory allocation for new_pic in zoom_out failed\n");
        return (Image) {NULL, 0, 0};
    }

    // Populate the new image data using pointer arthimetic and keeping it within the bounds; will look at 2x2 box at a time
    for (int i = 0; i < new_rows; i++){
        for (int j = 0; j < new_cols; j++){
            // Calculate upper left corner of the 2x2 box we're looking at
            int corner_x = i * 2;
            int corner_y = j * 2;

            // Define Pixels in 2x2 reference
            Pixel zero = in.data[corner_x * in.cols + corner_y]; // First Pixel
            Pixel one = in.data[corner_x * in.cols + corner_y + 1]; // Shift 1 to the right
            Pixel two = in.data[(corner_x + 1) * in.cols + corner_y];  // Shift down 1
            Pixel three = in.data[(corner_x + 1) * in.cols + corner_y + 1];  // Shift down 1 and to the right 1

            unsigned char average_r = (zero.r + one.r + two.r + three.r) / 4;  // Average the rgb values
            unsigned char average_g = (zero.g + one.g + two.g + three.g) / 4;
            unsigned char average_b = (zero.b + one.b + two.b + three.b) / 4;;

            int new_index = i * new_cols + j;  // Set new index

            // Set new Pixel data with averages of the 2x2 box
            zoom_pic.data[new_index] = (Pixel){average_r, average_g, average_b};
        }
    }

    return zoom_pic;
}

/* 
Helper function used in color_circular to check if the pixels are within the radius of the circle
*/
 double distance(int x1, int y1, int x2, int y2) {
    return sqrt(sq((x2 - x1)) + sq((y2 - y1)));
}

/*
 Helper function used in pointillism.
 Colors the pixels within a specified radius the same color as the center pixel.

 Parameters:
 - out: The output image to apply the colored circular region.
 - in: The input image to take the color from.
 - center_x: x-coordinate of the circle's center.
 - center_y: y-coordinate of the circle's center.
 - radius: radius of the circular area to be colored.

 The function loops through the pixels in a square bounding box around the 
 circle and checks if each pixel lies within the specified radius. If it does, 
 the pixel is colored with the value of the center pixel.
*/
void color_circular(Image out, const Image in, int center_x, int center_y, int radius) {
    for (int y = center_y - radius; y <= center_y + radius; y++)  
        for (int x = center_x - radius; x <= center_x + radius; x++) 
        
            // check if the boundaries are in bound in the image data
            if ((x >= 0 && x < in.cols) && (y >= 0 && y < in.rows)) 

                // only color pixels within circular area    
                if (distance(center_x, center_y, x, y) <= radius) 
                    out.data[y * in.cols + x] = in.data[center_y * in.cols + center_x];             
}

//_______pointilism________
/* apply a painting like effect, i.e. pointilism technique.
*/
Image pointilism( const Image in , unsigned int seed ) {
    // Check if the image data is valid
    if (in.data == NULL) {
        fprintf(stderr, "Image data is invalid\n");
        return (Image) {NULL, 0, 0};
    }

    srand(seed);  // set seed

    Image pointilism_img = {NULL, 0, 0};
    // calloc used to initialize all pixels into black
    pointilism_img.data = (Pixel *)calloc(in.cols * in.rows, sizeof(Pixel));
    pointilism_img.rows = in.rows;
    pointilism_img.cols = in.cols;

    // loops through each pixel in image data
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            int point_chance = (rand() % 100) + 1;        // gets a value between 1 and 100

            if (point_chance <= 3) { // 3 percent chance of getting selected
                int pix_radius = (rand() % 5) + 1;       // gets a value between 1 and 5 for the radius
                
                // color the surrounding pixels in a circular pattern
                color_circular(pointilism_img, in, j, i, pix_radius);
            }
        }
    }
    return pointilism_img;
}

/* 
 Function prototype to make sure clamp is defined before it is used throughout the blur functions.
*/
unsigned char clamp(double value);

/*
 Helper function used in blur to calculate the weighted pixel value given a Gaussian kernel.

 Parameters:
 - in: The input image.
 - pixel_row: The row of the target pixel.
 - pixel_col: The column of the target pixel.
 - size: The size of the Gaussian matrix.
 - *matrix: Pointer to the Gaussian kernel matrix used for convolution.

 This function computes the weighted sum of the neighboring pixels' color values using the 
 Gaussian kernel and returns the final blurred pixel value.
*/
Pixel filter_response(const Image in, int pixel_row, int pixel_col, int size, const double *matrix) {
    // Centers the pixel in our Gaussian matrix
    // Gets the end points of the surrounding pixels that overlap with the Gaussian matrix
    int mid = size / 2;
    double red = 0.0, green = 0.0, blue = 0.0;
    double sum = 0.0;  // Sum of contributing weights

    // Loop through the Gaussian kernel
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int x = pixel_col + j - mid;
            int y = pixel_row + i - mid;

            // Check if the pixel is within image bounds
            if (x >= 0 && x < in.cols && y >= 0 && y < in.rows) {
                Pixel current = in.data[y * in.cols + x];  // Correct pixel access
                double weight = matrix[i * size + j];      // Correct matrix index

                red += current.r * weight;
                green += current.g * weight;
                blue += current.b * weight;

                sum += weight;  // Accumulate the sum of weights
            }
        }
    }

    // Ensure the sum is not zero to avoid division by zero
    if (sum == 0.0) sum = 1.0;

    // Normalize the RGB values and clamp them
    Pixel convolved_pixel;
    convolved_pixel.r = clamp(red / sum);
    convolved_pixel.g = clamp(green / sum);
    convolved_pixel.b = clamp(blue / sum);

    return convolved_pixel;
}

/*
 Helper function to clamp pixel values to [0, 255].
*/
unsigned char clamp(double value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)value;
}

/*
 Helper function used in blur that generates a Gaussian matrix of size N x N with a given sigma.
*/
void gaussian_distribution(double sigma, int N, double *matrix) {
    int mid = N / 2;
    double sum = 0.0;

    // Calculate the Gaussian values for each element in the matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int dx = i - mid;
            int dy = j - mid;

            // Gaussian value calculation
            double g = (1.0 / (2.0 * PI * sq(sigma))) * exp(-(sq(dx) + sq(dy)) / (2 * sq(sigma)));

            matrix[i * N + j] = g;
            sum += g;  // Accumulate the sum for normalization
        }
    }
    // Normalize the matrix so all values sum to 1
    for (int i = 0; i < N * N; i++) {
        matrix[i] /= sum;
    }
}



/*
 Apply a blurring filter to the image.
*/
Image blur(const Image in, double sigma) {
    assert(in.data);  // Ensure the input image data is valid

    int N = sigma * 10;  // Gaussian matrix size (must be odd)
    if (N % 2 == 0) {
        N += 1;
    }

    double *gaussian_matrix = (double *)calloc(N * N, sizeof(double));
    if (gaussian_matrix == NULL) {
        fprintf(stderr, "Error: Memory allocation for Gaussian matrix failed.\n");
        return (Image){NULL, 0, 0};
    }

    // Helper function that creates a Gaussian distribution matrix of size N * N
    gaussian_distribution(sigma, N, gaussian_matrix);

    // Create a new image to store the blurred result
    Image blurred = {malloc(sizeof(Pixel) * in.rows * in.cols), in.rows, in.cols};
    if (blurred.data == NULL) {
        fprintf(stderr, "Error: Memory allocation for blurred image failed.\n");
        free(gaussian_matrix);
        return (Image){NULL, 0, 0};
    }

    // Apply Gaussian blur to each pixel in the image data
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            blurred.data[i * in.cols + j] = filter_response(in, i, j, N, gaussian_matrix);
        }
    }

    free(gaussian_matrix);  // Free the Gaussian matrix memory

    return blurred;
}

//______binarize______
/* convert to a black and white image based on a threshold
*/
Image binarize( const Image in , const int threshold ){
    // Ensure image data is not null
    assert(in.data);

    // Create new image data structure we will alter
    Image new_pic = {NULL, 0, 0};
    new_pic.rows = in.rows;
    new_pic.cols = in.cols;
    new_pic.data = (Pixel*)malloc(sizeof(Pixel) * in.cols * in.rows);

    // Check that memory allocation for the new image succeeded
    if (new_pic.data == NULL){
        fprintf(stderr, "Error: Memory allocation for new_pic in binarize failed\n");
        return (Image){NULL, 0, 0};  // Return null image
    }

    // Index through all of the pixels
    for (int i = 0; i < in.cols * in.rows; i++){
        // First, calculate gray binary value
        unsigned char gray_binary = pixel_to_gray(in.data[i]);

        // Second, set pixel to black or white based on the threshold passed
        unsigned char binarize_binary;

        if (gray_binary < threshold){
            binarize_binary = 0;  // Set to black  
        } 
        else{  // Else, gray_binary is > threshold
            binarize_binary = 255;  // Set to white
        }

        // Third, set all rgb values in the Pixel index to the binarize_binary value
        new_pic.data[i] = (Pixel){binarize_binary, binarize_binary, binarize_binary};
    }

    return new_pic;  // Return the new calculated Image struct
}