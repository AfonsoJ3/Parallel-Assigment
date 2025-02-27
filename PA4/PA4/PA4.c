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

    int k=10;
    temp=(int*)malloc(height*width*sizeof(int));
    //your image processing here
    //pick a pixel
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            int index=i*width+j; //center pixel index
            int sum=0;
            int counter=0;
            //actual convolution sum
            for(int u=-k;u<=k;u++){
                for(int v=-k;v<=k;v++){
                    int cindex=(i-u)*width+(j-v); //conv index
                    if(i-u<0 || i-u>=height ||j-v<0 || j-v>=width)
                        continue;
                    sum+=matrix[cindex];
                    counter++;
                }
            }
            temp[index]=sum/counter;
        }
    }


    //save image
    matToImage("processedImage.jpg",temp,dims);

    free(dims);
    free(matrix);
    return 0;
}
