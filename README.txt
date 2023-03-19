
			Image transforming tool
			

Firstly, we need to identify the type of operation that will apply to our image.
We further proceed depending on this information. 


1* Command's first argument is "-c" (image compression)

In this case, we know the following arguments represent, in this order: 
the compression factor, the input file and the output file.

Using the "atoi" C function, we store the compression factor value as 
an integer inside the "factor" variable.

We begin solving this task by calling the "build_grid_c" function to build the 
pixels matrix of the .ppm image. Then, the "init_QTree" function initializes 
the compression quadtree and the "build_QTree_c" function constructs it using 
the pixels matrix.

After that, we calculate the number of leaf nodes and the total number of nodes 
by calling the "num_leafs" and "num_nodes" functions. Then, we write these 
values in the binary output file.

Next, we allocate the array of tree nodes, fill it using the "build_vector" 
function and write it in the binary output file.


2* Command's first argument is "-d" (image decompression)

In this case, the following arguments represent, in this order: the input file 
and the output file.

From the input file, we read the total number of nodes and the number of leaf 
nodes. With that information, we allocate the array of tree nodes and also read 
it from the input file. Using the "init_QTree" and "build_QTree_d" functions, 
we initialize the compression quadtree and build it.

After that, we calculate the image area by summing the ones each leaf node 
covers (by looking at the "area" field of the array's element structure). Knowing 
that the image is squared, we can find its dimensions (width/height) by 
calculating the square root of its area.

Next, we allocate the pixels matrix and build it by calling the "build_grid_d" 
function. We then write the .ppm type header and the pixels matrix in the 
output file.


3* Command's first argument is "-m" (image flip)

In this case, the following arguments are, in this order: the type of flip, 
the compression factor, the input file and the output file.

We solve this task by calling the "build_grid_c" function to construct the 
pixels matrix of the image and then the "init_QTree" and "build_QTree_c" 
functions to build the quadtree. 

To flip the image, we modify the quadtree using the "flip_vertical" (if the type 
of flip is "v") or the "flip_horizontal" function (if the type of flip is "h"). 
Based on the new arrangement of the quadtree, we modify the pixels matrix using 
the "build_grid_d" function.

Finally, we write the .ppm output file the same way we did for the "-d" argument.
