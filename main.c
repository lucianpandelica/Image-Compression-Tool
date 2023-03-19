#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "header.h"

int main(int argc, char *argv[])
{
    int i = 0;

    /*
        determine first argument type and further
        proceed depending on this information
    */

    // command's first argument is "-c" (image compression)
    if(strcmp(argv[1], "-c") == 0)
    {
        // the argv[2] element stores 
        // the compression factor
        int factor = 0;
        factor = atoi(argv[2]);

        // the following two arguments represent the input file and
        // the output file names
        FILE *f = NULL, *g = NULL;
        f = fopen(argv[3], "rb");
        g = fopen(argv[4], "wb");

        // verify if input file is opened
        if(f != NULL)
        {
            int width = 0, height = 0, max_color = 0;
            int k = 0;
            
            pixel **grid = NULL;
            QTree *tree = NULL;

            // initialize the quadtree
            init_QTree(&tree);

            // build the pixels matrix of image
            build_grid_c(&grid, &width, &height, &max_color, f);

            // build the compression quadtree based on
            // the pixels matrix
            build_QTree_c(tree, grid, 0, 0, width, factor);
            
            // calculate total number of nodes and store it in 'nodes'
            // variable
            // calculate number of leaf nodes and store it in 'leaves'
            // variable
            uint32_t nodes = num_nodes(tree), leaves = num_leaves(tree);

            // write the two values in the binary output file
            fwrite(&leaves, sizeof(uint32_t), 1, g);
            fwrite(&nodes, sizeof(uint32_t), 1, g);

            // allocate the array of quadtree nodes
            QuadtreeNode *node_vector = NULL;
            node_vector = (QuadtreeNode*) malloc(nodes * sizeof(QuadtreeNode));

            // find value of k, where 2^k = width
            k = log_two(width);

            // index_v = index of the array element that was
            //           last added
            int index_v = 0;

            // build array of nodes
            build_vector(tree, node_vector, k, 0, &index_v);

            // write array in the binary output file
            fwrite(node_vector, sizeof(QuadtreeNode), nodes, g);
            
            // free array and quadtree
            free(node_vector);
            free_QTree(&tree);

            // free pixels matrix
            for(i = 0; i < height; i++)
                free(grid[i]);
            free(grid);
        }

        // close files
        fclose(f);
        fclose(g);
    }

    // command's first argument is "-d" (image decompression)
    if(strcmp(argv[1], "-d") == 0)
    {
        // the following two arguments represent the input file and
        // the output file names
        FILE *f = NULL, *g = NULL;
        f = fopen(argv[2], "rb");
        g = fopen(argv[3], "wb");

        // verify if input file is opened
        if(f != NULL)
        {   
            pixel **grid = NULL;
            QTree *tree = NULL;

            // read values of 'nodes' and 'leaves' variables from
            // input file
            uint32_t nodes = 0, leaves = 0;
            fread(&leaves, sizeof(uint32_t), 1, f);
            fread(&nodes, sizeof(uint32_t), 1, f);

            // allocate nodes array
            QuadtreeNode *node_vector = NULL;
            node_vector = (QuadtreeNode*) malloc(nodes * sizeof(QuadtreeNode));

            // read array from input file
            fread(node_vector, sizeof(QuadtreeNode), nodes, f);

            // initialize quadtree
            init_QTree(&tree);

            // build quadtree based on nodes array
            build_QTree_d(tree, node_vector, 0);

            unsigned long long total_area = 0;

            // calculate area of image
            for(i = 0; i < nodes; i++)
                if(node_vector[i].top_left == -1)
                    total_area = total_area + node_vector[i].area;

            // calculate image dimensions
            int height = sqrt(total_area);
            int width = height;

            // allocate pixels matrix
            grid = (pixel **) malloc(height * sizeof(pixel*));
            for(i = 0; i < height; i++)
                grid[i] = (pixel *) malloc(width * sizeof(pixel));

            // build pixels matrix based on quadtree
            build_grid_d(tree, grid, 0, 0, width);

            // allocate 'cuv' array, which stores the header for the
            // .ppm output file
            char *cuv = malloc(50 * sizeof(char));
            sprintf(cuv, "P6\n%d %d\n255\n", width, height);

            // write header in output file
            fwrite(cuv, sizeof(char), strlen(cuv), g);

            // write pixels matrix in output file
            for(i = 0; i < height; i++)
                fwrite(grid[i], sizeof(pixel), width, g);

            // free nodes array and quadtree
            free(node_vector);
            free_QTree(&tree);

            // free pixels matrix
            for(i = 0; i < height; i++)
                free(grid[i]);
            free(grid);

            // free 'cuv' array
            free(cuv);
        }
        
        // close files
        fclose(f);
        fclose(g);
    }

    // command's first argument is "-m" (image flip)
    if(strcmp(argv[1], "-m") == 0)
    {
        // argv[2][0] element represents the flip type 
        char type = '\0';
        type = argv[2][0];

        // argv[3] represents the next argument, compression factor
        int factor = atoi(argv[3]);

        // the following two arguments represent the input file and
        // the output file names
        FILE *f = NULL, *g = NULL;
        f = fopen(argv[4], "rb");
        g = fopen(argv[5], "wb");

        // verify if input file is opened
        if(f != NULL)
        {
            int width = 0, height = 0, max_color = 0;
            pixel **grid = NULL;
            QTree *tree = NULL;

            // initialize compression quadtree
            init_QTree(&tree);

            // build initial pixels matrix
            build_grid_c(&grid, &width, &height, &max_color, f);

            // build compression quadtree based on
            // initial pixels matrix
            build_QTree_c(tree, grid, 0, 0, width, factor);

            // modify quadtree to flip the image
            if(type == 'v')
                flip_vertical(tree);
            else
                flip_horizontal(tree);
            
            // modify pixels matrix based on modified quadtree
            build_grid_d(tree, grid, 0, 0, width);

            // write header in .ppm output file
            char *cuv = malloc(50 * sizeof(char));
            sprintf(cuv, "P6\n%d %d\n255\n", width, height);
            fwrite(cuv, sizeof(char), strlen(cuv), g);

            // write pixels matrix in .ppm output file
            for(i = 0; i < height; i++)
                fwrite(grid[i], sizeof(pixel), width, g);

            // free quadtree
            free_QTree(&tree);

            // free pixels matrix
            for(i = 0; i < height; i++)
                free(grid[i]);
            free(grid);

            // free 'cuv' array
            free(cuv);
        }

        // close files
        fclose(f);
        fclose(g);
    }

    return 0;
}
