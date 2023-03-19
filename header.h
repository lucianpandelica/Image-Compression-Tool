#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>

/*
structure of pixel
*/
typedef struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} __attribute__ ((packed)) pixel;

/*
structure of quadtree

q1 = child node that represents the top-left sub-area
q2 = child node that represents the top-right sub-area
q3 = child node that represents the bottom-right sub-area
q4 = child node that represents the bottom-left sub-area
*/
typedef struct QTree
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    struct QTree *q1, *q2, *q3, *q4;
} QTree;

/*
structure of node array element
*/
typedef struct QuadtreeNode
{
    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
} __attribute__ ((packed)) QuadtreeNode;

void init_QTree (QTree **tree);
void build_QTree_c (QTree *tree, pixel **grid, int x, int y, int size, int factor);
void build_QTree_d (QTree *tree, QuadtreeNode *node_vector, int index);
void free_QTree (QTree **tree);

void build_grid_c (pixel ***grid, int *width, int *height, int *max_color, FILE *f);
void build_grid_d (QTree *tree, pixel **grid, int x, int y, int size);

void build_vector (QTree *tree, QuadtreeNode *node_vector, int k, int depth, int *index_v);

uint32_t num_leaves (QTree *tree);
uint32_t num_nodes (QTree *tree);

void flip_vertical (QTree *tree);
void flip_horizontal (QTree *tree);

int log_two(int x);

#endif