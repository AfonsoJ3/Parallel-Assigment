#include <stdio.h>
#include <stdlib.h>
 

int main(int argc, char* argv)
{
	size_t size_bytes = 1024L * 1024 * 1024;
	int* array = (int*)malloc(size_bytes);
	
	if (array == NULL)
	{
		printf("Array failt!!!\n");
		return 1;
	}
	
	for (int i = 0; i < size_bytes / sizeof(int); i++)
	{	
		array[i] = i; 
	}
		
	printf("The size is: %zu\n", size_bytes / sizeof(int));
	
	for(int i = 0; i < 100; i++)
	{
		printf("Array[%d]: %d.\n", i, array[i]);
	}
	return 0;
}
