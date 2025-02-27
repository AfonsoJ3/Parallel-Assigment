#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int* imageToMat(char* name, int* dims);
extern void matToImage(char* name, int* mat, int* dims);

int main( int argc, char** argv ) {
    int *matrix;
    int *temp;
    char *name="image.jpg";
    int *dims;
    dims=(int*) malloc(2*sizeof(int));

    //read image
    matrix=imageToMat(name,dims);
    int height=dims[0];
    int width=dims[1];
    
    //your image processing here


    //save image
    matToImage("processedImage.jpg",matrix,dims);

    free(dims);
    free(matrix);
    return 0;
}
