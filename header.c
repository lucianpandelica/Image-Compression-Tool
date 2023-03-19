#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include "header.h"

/*
function used to initialize a quadtree
*/
void init_QTree (QTree **tree)
{
    // allocate root node
    (*tree) = (QTree*) malloc(sizeof(QTree));

    // initialize pointers to child nodes
    (*tree)->q1 = NULL;
    (*tree)->q2 = NULL;
    (*tree)->q3 = NULL;
    (*tree)->q4 = NULL;

    // initialize colours
    (*tree)->red = 0;
    (*tree)->green = 0;
    (*tree)->blue = 0;
}

/*
recursive function used to build compression quadtree based on the
pixels matrix of the image and the compression factor 
*/
void build_QTree_c (QTree *tree, pixel **grid, int x, int y, int size, int factor)
{
    /*
        for each call, the function covers the block that has grid[x][y] 
        as top-left element and a side length of 'size' pixels
    */

    int i = 0, j = 0;

    // medie_culoare = arithmetic mean of values that correspond to
    //                 that colour inside the current block
    // mean = similarity score for the current block
    unsigned long long medie_red = 0, medie_green = 0, medie_blue = 0;
    unsigned long long mean = 0;

    // sum values corresponding to each colour
    for(i = x; i < (x + size); i++)
        for(j = y; j < (y + size); j++)
        {
            medie_red = medie_red + grid[i][j].red;
            medie_green = medie_green + grid[i][j].green;
            medie_blue = medie_blue + grid[i][j].blue;
        }

    // divide by the number of pixels to obtain the mean
    medie_red = medie_red / (size * size);
    medie_green = medie_green / (size * size);
    medie_blue = medie_blue / (size * size);

    // assign means to current node
    tree->red = medie_red;
    tree->green = medie_green;
    tree->blue = medie_blue;

    // calculate value of similarity score
    for(i = x; i < (x + size); i++)
        for(j = y; j < (y + size); j++)
        {
            mean = mean + 
                   (medie_red - grid[i][j].red) * 
                   (medie_red - grid[i][j].red);
            mean = mean + 
                   (medie_green - grid[i][j].green) * 
                   (medie_green - grid[i][j].green);
            mean = mean + 
                   (medie_blue - grid[i][j].blue) * 
                   (medie_blue - grid[i][j].blue);
        }
    mean = mean / (3 * size * size);
  
    // verify if the current block can be divided into quarters
    if(size > 1)
        // verify if similarity score is greater than compression factor
        if(mean > factor)
        {
            /*
                divide the block into quarters, which have the following 
                top-left elements:
                - grid[x][y] for q1
                - grid[x][y + (size / 2)] for q2
                - grid[x + (size / 2)][y + (size / 2)] for q3
                - grid[x + (size / 2)][y] for q4
            */

            // initialize child node
            init_QTree(&tree->q1);
            // build sub-quadtree that corresponds to sub-block
            build_QTree_c(tree->q1, 
                          grid, 
                          x, 
                          y, 
                          (size / 2), 
                          factor);

            init_QTree(&tree->q2);
            build_QTree_c(tree->q2, 
                          grid, 
                          x, 
                          y + (size / 2), 
                          (size / 2), 
                          factor);

            init_QTree(&tree->q3);
            build_QTree_c(tree->q3, 
                          grid, 
                          x + (size / 2), 
                          y + (size / 2), 
                          (size / 2), 
                          factor);

            init_QTree(&tree->q4);
            build_QTree_c(tree->q4, 
                          grid, 
                          x + (size / 2), 
                          y, 
                          (size / 2), 
                          factor);
        }
}

/*
recursive function used build the compression quadtree based on the nodes array
*/
void build_QTree_d (QTree *tree, QuadtreeNode *node_vector, int index)
{
    /*
        index = index of the current node in node array
    */

    // assign colour values to current node
    tree->red = node_vector[index].red;
    tree->green = node_vector[index].green;
    tree->blue = node_vector[index].blue;

    // verify if current array node has child nodes
    if(node_vector[index].top_left != -1)
    {   

        // initialize sub-quadtrees corresponding to child nodes and build them
        init_QTree(&tree->q1);
        build_QTree_d(tree->q1, node_vector, node_vector[index].top_left);
    
        init_QTree(&tree->q2);
        build_QTree_d(tree->q2, node_vector, node_vector[index].top_right);
    
        init_QTree(&tree->q3);
        build_QTree_d(tree->q3, node_vector, node_vector[index].bottom_right);
    
        init_QTree(&tree->q4);
        build_QTree_d(tree->q4, node_vector, node_vector[index].bottom_left);
    }
}

/*
recursive function used to free a quadtree
*/
void free_QTree (QTree **tree)
{
    // verify if current node is a leaf node
    if((*tree)->q1 == NULL)
    {
        // free current node
        free((*tree));
        return;
    }
    
    // free child nodes
    free_QTree(&((*tree)->q1));
    free_QTree(&((*tree)->q2));
    free_QTree(&((*tree)->q3));
    free_QTree(&((*tree)->q4));
    
    // free current node
    free((*tree));
}

/*
function used to build the pixels matrix of image based on its .ppm file
*/
void build_grid_c (pixel ***grid, int *width, int *height, int *max_color, FILE *f)
{
    /*
        function passes the image dimensions ('width', 'height') and the 
        maximum value of a colour ('max_color') as parameters, after
        reading them from input file
    */

    int i = 0, j = 0;

    // allocate 'cuv' array of type char, which stores a character read
    // from input file in the first element (cuv[0]) and the NULL character
    // in the second element (cuv[1])
    char *cuv = malloc(2 * sizeof(char));
    cuv[1] = '\0';
    
    // read first three characters from input file (P, 6, \n)
    fread(cuv, sizeof(char), 1, f);
    fread(cuv, sizeof(char), 1, f);
    fread(cuv, sizeof(char), 1, f);

    // read characters that represent the image width until 
    // reaching ' ' character
    fread(cuv, sizeof(char), 1, f);
    while(cuv[0] != ' ')
    {   
        // convert characters to number
        (*width) = (*width) * 10 + atoi(cuv);
        fread(cuv, sizeof(char), 1, f); 
    }

    // read characters that represent the image height until 
    // reaching '\n' character
    fread(cuv, sizeof(char), 1, f);
    while(cuv[0] != '\n')
    {   
        // convert characters to number
        (*height) = (*height) * 10 + atoi(cuv);
        fread(cuv, sizeof(char), 1, f);
    }

    // read characters that represent the value of 'max_color' until
    // reaching '\n' character
    fread(cuv, sizeof(char), 1, f);
    while(cuv[0] != '\n')
    {   
        // convert characters to number
        (*max_color) = (*max_color) * 10 + atoi(cuv);
        fread(cuv, sizeof(char), 1, f);
    }

    // allocate pixels matrix

    // allocate lines
    (*grid) = (pixel **) malloc((*height) * sizeof(pixel*));
    // allocate columns
    for(i = 0; i < (*height); i++)
        (*grid)[i] = (pixel *) malloc((*width) * sizeof(pixel));
    
    // read pixels matrix from file
    for(i = 0; i < (*height); i++)
        for(j = 0; j < (*width); j++)
            fread(&(*grid)[i][j], sizeof(pixel), 1, f);
    
    // free 'cuv' array
    free(cuv);
}

/*
recursive function used to build pixels matrix based on compression quadtree
*/
void build_grid_d (QTree *tree, pixel **grid, int x, int y, int size)
{
    /*
        for each call, the function builds the squared area that has the
        top-left element located at line x, column y and size * size elements 
    */
    int i = 0, j = 0;

    // verify if current node is a leaf node
    if(tree->q1 == NULL)
    {
        // assign the colour of leaf node to the area that corresponds to it
        for(i = x; i < (x + size); i++)
            for(j = y; j < (y + size); j++)
            {
                grid[i][j].red = tree->red;
                grid[i][j].green = tree->green;
                grid[i][j].blue = tree->blue;
            }
    }
    else
    {
        // build sub-blocks corresponding to child nodes
        build_grid_d(tree->q1, grid, x, y, (size / 2));
        build_grid_d(tree->q2, grid, x, y + (size / 2), (size / 2));
        build_grid_d(tree->q3, grid, x + (size / 2), y + (size / 2), (size / 2));
        build_grid_d(tree->q4, grid, x + (size / 2), y, (size / 2));
    }
}

/*
recursive function used to build nodes array
(array of quadtree nodes)
*/
void build_vector (QTree *tree, QuadtreeNode *node_vector, int k, int depth, int *index_v)
{
    /*
        k = logarithm of 'width' value (or 'height' value) to base 2
        depth = depth of current node in quadtree
    */
    int aux_index = 0, i = 0;

    // assign colours of current node to array's corresponding element
    node_vector[(*index_v)].blue = tree->blue;
    node_vector[(*index_v)].green = tree->green;
    node_vector[(*index_v)].red = tree->red;

    // calculate the pixels area that the current node covers,
    // based on its depth and the value of 'k'.
    // we use the fact that a leaf node which has depth 'h' corresponds to 
    // a square area of image that has a side of 2^(k-h) pixels.
    node_vector[(*index_v)].area = 1;
    for(i = 0; i < (k - depth); i++)
        node_vector[(*index_v)].area = node_vector[(*index_v)].area * 2;
    node_vector[(*index_v)].area = node_vector[(*index_v)].area * 
                                node_vector[(*index_v)].area;

    aux_index = (*index_v);

    // verify if current node has child nodes
    if(tree->q1 != NULL)
    {
        // add child nodes to array

        // update current index
        (*index_v)++;
        // store index of child node
        node_vector[aux_index].top_left = (*index_v);
        // add corresponding sub-quadtree's nodes to the array
        build_vector(tree->q1, node_vector, k, depth + 1, index_v);

        // repeat for the other child nodes
        (*index_v)++;
        node_vector[aux_index].top_right = (*index_v);
        build_vector(tree->q2, node_vector, k, depth + 1, index_v);

        (*index_v)++;
        node_vector[aux_index].bottom_right = (*index_v);
        build_vector(tree->q3, node_vector, k, depth + 1, index_v);

        (*index_v)++;
        node_vector[aux_index].bottom_left = (*index_v);
        build_vector(tree->q4, node_vector, k, depth + 1, index_v);
    }
    else
    {
        // current node is a leaf node
        node_vector[aux_index].top_left = -1;
        node_vector[aux_index].top_right = -1;
        node_vector[aux_index].bottom_right = -1;
        node_vector[aux_index].bottom_left = -1;
    }
}

/*
recursive function used to calculate the number of 
leaf nodes a compression quadtree has
*/
uint32_t num_leaves (QTree *tree)
{
    // verify if current node is a leaf node
    if(tree->q1 == NULL)
        // count it and return
        return 1;
    
    // return total number of current quadtree's leaf nodes 
    return num_leaves(tree->q1) + 
           num_leaves(tree->q2) + 
           num_leaves(tree->q3) + 
           num_leaves(tree->q4);
}

/*
recursive function used to calculate the number of
nodes a compression quadtree has
*/
uint32_t num_nodes (QTree *tree)
{
    // verify if current node is a leaf node
    if(tree->q1 == NULL)
        // count it and return
        return 1;
    
    // return 1 + number of nodes the sub-quadtrees have
    return 1 +
           num_nodes(tree->q1) + 
           num_nodes(tree->q2) + 
           num_nodes(tree->q3) + 
           num_nodes(tree->q4);
}

/*
recursive function used to flip the image vertically,
by modifying it's quadtree
*/
void flip_vertical (QTree *tree)
{
    QTree *aux = NULL;

    // verify if current node is a leaf node
    if(tree->q1 == NULL)
        // nothing to be done, return
        return;

    // interchange top-left and bottom-left
    aux = tree->q1;
    tree->q1 = tree->q4;
    tree->q4 = aux;

    // interchange top-right and bottom-right
    aux = tree->q2;
    tree->q2 = tree->q3;
    tree->q3 = aux;

    // repeat steps for current node's children
    flip_vertical(tree->q1);
    flip_vertical(tree->q2);
    flip_vertical(tree->q3);
    flip_vertical(tree->q4);
}

/*
recursive function used to flip the image horizontally,
by modifying it's quadtree
*/
void flip_horizontal (QTree *tree)
{
    QTree *aux = NULL;

    // verify if current node is a leaf node
    if(tree->q1 == NULL)
        // nothing to be done, return
        return;

    // interchange top-left and top-right
    aux = tree->q1;
    tree->q1 = tree->q2;
    tree->q2 = aux;

    // interchange bottom-left and bottom-right
    aux = tree->q3;
    tree->q3 = tree->q4;
    tree->q4 = aux;

    // repeat steps for current node's children
    flip_horizontal(tree->q1);
    flip_horizontal(tree->q2);
    flip_horizontal(tree->q3);
    flip_horizontal(tree->q4);
}

/*
function used to calculate the value of logarithm of x to base 2
*/
int log_two (int x)
{
    int exp = 0;

    while(x != 1)
    {
        exp++;
        x = x / 2;
    }

    return exp;
}