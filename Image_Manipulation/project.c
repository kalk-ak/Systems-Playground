//project.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_io.h"
#include <assert.h>
#include "image_manip.h"

// Return (exit) codes
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5   
#define RC_OP_ARGS_RANGE_ERR  6      
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8

//printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");

void print_usage(void);


int main (int argc, char* argv[]) {

    // Checks if the image to be altered in passed in the command line argument
    if (argc < 2) {
        fprintf(stderr, "Missing <input-image> argument in command line.\n");
        print_usage();
        return RC_MISSING_FILENAME; 
    }
    // Checks if file output name is provided in the command line argument
    if (argc < 3) {
        fprintf(stderr, "Missing <output-image> argument in command line.\n");
        print_usage();
        return RC_MISSING_FILENAME;
    }

    // Asserts the user inputed an operation
    if (argc < 4) {
        fprintf(stderr, "Image operation not specified. Missing <command-name> argument in command line.\n");
        print_usage();
        return RC_INVALID_OPERATION;
    }
    
    char * filename = argv[1];   // The name of the image input we're reading

    char * out_filename = argv[2]; // The name of the file we're writing to

    char *task = argv[3];          // The operation to be preformed on image

    FILE *in = fopen(filename, "rb");
    
    // Checks if file opened successfully.
    if ( !in ) {
        fprintf(stderr, "Error occured while opening %s for reading\n", filename);
        return RC_OPEN_FAILED;
    }
    Image img = read_ppm( in ); 
    fclose ( in );

    // Asserts read_ppm() worked as expected 
    if (img.data == NULL) {
        fprintf(stderr, "Error occured while reading ppm file. %s might be corrupted or invalid.\n", filename);
        return RC_INVALID_PPM;
    }
    /*
    using an if else statement to identify which task to be performed on image and carries it out if valid commands are passed.
    */
    if (strcmp(task, "invert") == 0) {    // image is to be inverted
        
        /* Ensures no extra command line arguments are inputed. */
        if (argc != 4) { 
            fprintf(stderr, "%d extra command argument passed in for invert.\n", argc - 4);
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        }

        img = invert(img);
        
    } else if (strcmp(task, "crop") == 0) {

        // Checks if there are any extra or missing command arguments
        if (argc < 8) {
            fprintf(stderr, "Fewer than needed command arguments passed for crop.\n");
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        } else if (argc > 8) {
            fprintf(stderr, "%d extra command argument(s) passed for crop.\n", argc - 8);
            free_image( &img );
            return  RC_INVALID_OP_ARGS;
        }

        // These are placeholders for the 4 passed command line values for crop
        Point top_left = {-1, -1};
        Point bottom_right = {-1, -1};

        // Pointers to top left and bottom right points that we will crop to 
        Point *top_ptr = & top_left;
        Point *bottom_ptr =  & bottom_right;

        /*
        block of code checks if valid integers are passed for the rows and columns
        */
        { // Used to check if there are non-int values passed for the points
            char *endptr; 
            int upper_left_col = (int) strtol(argv[4], &endptr, 10);  
            if (*endptr != '\0') {
                fprintf(stderr,"Invalid number( %s ) for upper left column in crop.\n", argv[4]);
                free_image( &img );
                return RC_OP_ARGS_RANGE_ERR;
            }
            int upper_left_row = (int) strtol(argv[5], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr,"Invalid number( %s ) for upper left row in crop.\n", argv[5]);
                free_image( &img );
                return RC_OP_ARGS_RANGE_ERR;
            }
            int lower_right_col = (int) strtol(argv[6], &endptr, 10);
            if (*endptr != '\0') {
                fprintf(stderr,"Invalid number( %s ) for lower right column in crop.\n", argv[6]);
                free_image( &img );
                return RC_OP_ARGS_RANGE_ERR;
            }
            int lower_right_row = (int) strtol(argv[7], &endptr, 10);   
            if (*endptr != '\0') {
                fprintf(stderr,"Invalid number( %s ) for lower right row in crop.\n", argv[7]);
                free_image( &img );
                return RC_OP_ARGS_RANGE_ERR;
            }
            /*
            Checks if the rows and columns are valid and in range of the image row and columns
            */
            {
                if (upper_left_col < 0 || upper_left_row < 0 || lower_right_col < 0 || lower_right_row < 0) {
                    fprintf(stderr, "Rows and cols can't be negative value in crop.\n");
                    free_image( &img );
                    return RC_OP_ARGS_RANGE_ERR;
                } 
                if ((upper_left_col >= lower_right_col) || (upper_left_row >= lower_right_row)) {
                    fprintf(stderr, "Invalid range for rows and columns to crop %s.\n", argv[1]);
                    free_image( &img );
                    return RC_OP_ARGS_RANGE_ERR;
                }
                if ((lower_right_col > img.cols) || (lower_right_row > img.rows)) {
                    fprintf(stderr, "Lower right points exceed the dimensions of %s.\n", argv[1]);
                    free_image( &img );
                    return RC_OP_ARGS_RANGE_ERR;
                }
            }
        /** organizes the rows and colums to points */
        Point upper_left = {upper_left_col, upper_left_row};
        Point lower_right = {lower_right_col, lower_right_row};
        
        /** Assigns the organized local points to top_left and bottom_right */
        *top_ptr = upper_left;
        *bottom_ptr = lower_right;

        } 
        /* Asserts if rows and collumns were assigned to top_left and bottom-right */
        assert(top_left.x != -1 && bottom_right.x != -1);
        assert(top_left.y != -1 && bottom_right.y != -1);


        Image img_cropped = crop(img, top_left, bottom_right);

        if (img_cropped.data == NULL) { // Malloc failed in crop
            // Error message printed in crop function
            return RC_UNSPECIFIED_ERR;
        }

        free_image( &img );  // Free image data
        img = img_cropped;  
    
    }
    else if (strcmp(task, "zoom_out") == 0) {
        /* Checks if there are any extra command arguments for zoom_out */
        if (argc != 4) {
            fprintf(stderr, "%d extra command argument passed in for zoom out.\n", argc - 4);
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        } 

        Image img_zoomed = zoom_out(img);

        if (img_zoomed.data == NULL) { // Malloc failed in zoom_out
            // Error message printed in zoom_out function
            return RC_UNSPECIFIED_ERR;
        }
        free_image( &img );  // Free image data

        img = img_zoomed;

    } else if (strcmp(task, "binarize") == 0) {
        /* Checks if there are any extra command arguments for binarize */
        if (argc < 5) {
            fprintf(stderr, "Threshold number between 0 and 255 inclusive needed to binarize %s\n", argv[1]);
            free_image ( &img );
            return RC_INVALID_OP_ARGS;
        }
        if (argc > 5) {
            fprintf(stderr, "%d extra command argument passed in for binarize.\n", argc - 5);
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        }

        /* Gets the threshold number and checks if its in valid range */
        char *endptr;
        int threshold = (int) strtol(argv[4], &endptr, 10);  
        if (*endptr != '\0') {
            fprintf(stderr,"Invalid number( %s ) for threshold in binarize.\n", argv[4]);
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        }

        if (threshold > 255 || threshold < 0) {
            fprintf(stderr, "Threshold number (%d) out of range for binarize. Must be between 0 and 255 inclusive.\n", threshold);
            free_image( &img );
            return RC_OP_ARGS_RANGE_ERR;
        }

        Image binarized_img = binarize(img, threshold);
        if (binarized_img.data == NULL) { // Malloc failed in binarize
            // Error message printed in binarize function
            return RC_UNSPECIFIED_ERR;
        }
        
        free_image(&img);  // Free image
        img = binarized_img;
    
    } else if (strcmp(task, "pointilism") == 0) {
        /* Checks if there are any extra command arguments for binarize */
        if (argc != 4) {
            fprintf(stderr, "%d extra command argument passed in for pointilism.\n", argc - 4);
            free_image( &img );
            return RC_INVALID_OP_ARGS;
        }
        int seed = 1;  // Initialize given seed for randomization

        Image img_pointilism= blur(img, seed);

        if (img_pointilism.data == NULL) { // Malloc failed
            // Error printed in pointilism
            return RC_UNSPECIFIED_ERR;
        }
        free_image( &img );

        img = img_pointilism;

    } else if (strcmp(task, "blur") == 0) {
        /* Checks if there are any extra command arguments for binarize */
            if (argc < 5) {
                fprintf(stderr, "Sigma number needed to blur %s\n", argv[1]);
                free_image ( &img );
                return RC_INVALID_OP_ARGS;
            }
            if (argc > 5) {
                fprintf(stderr, "%d extra command argument passed in for blur.\n", argc - 5);
                free_image( &img );
                return RC_INVALID_OP_ARGS;
            }

            /* Gets sigma and checks if its in valid range */
            char *endptr;
            double sigma = strtod(argv[4], &endptr);
            if (*endptr != '\0') {
                fprintf(stderr,"Invalid sigma number( %s ) for blur.\n", argv[4]);
                free_image( &img );
                return RC_INVALID_OP_ARGS;
            }

            if (sigma < 0) {  
                fprintf(stderr, "Sigma number for blur can't be negative\n");
                free_image( &img );
                return RC_OP_ARGS_RANGE_ERR;
            }
            // sets the minimum size of the gaussian blur matrix to 5 * 5 matrix
            if (sigma < 0.5) {
                sigma = 0.5;
            };
            
        Image img_blurred = blur(img, sigma);

        if (img_blurred.data == NULL) { // Malloc failed in blur
            // Error printed in blur
            return RC_UNSPECIFIED_ERR;
        }
        free_image( &img );

        img = img_blurred;
    } else {
        fprintf(stderr,"Invalid command line argument.\n");
        print_usage();
        return RC_UNSPECIFIED_ERR;
    }
    
    // Writes the file to the file name passed in the command line arguments
    FILE *out = fopen(out_filename, "wb");
    int answer = write_ppm(out, img);
    
    /* Checks if image was written successfully */
    if (answer == EXIT_FAILURE) {
        fprintf(stderr, "Error occured while writing %s\n", argv[2]);
        free_image( &img );
        fclose( out );
        return RC_WRITE_FAILED;
    }

    free_image( &img );
    fclose( out );

	/* Everything finished successfully */
    printf("%s created successfully.\n", argv[2]);
	return RC_SUCCESS; 
}

void print_usage(void) {
    printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
    printf("SUPPORTED COMMANDS:\n");
    printf("   invert\n" );
    printf("   zoom-out\n" );
    printf("   binarize <threshold>\n" );
    printf("   pointilism\n" );
    printf("   blur <sigma>\n" );
    printf("   crop <bottom left (x,y)> <top right (x,y)>\n" );
}
