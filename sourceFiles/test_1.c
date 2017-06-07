//test.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "CommonDefine.h"
#include "CommonFuncs.h"
#include "ProcessPGM.h"

void randomNum(int times)
{
	srand((unsigned)time(NULL));
	for (int i = 0; i < times; ++i)
	{
		printf("%d     \n",rand());
	}
}

char* getSpace(int number)
{
	int baseSpace = 10;
	int selfLength = 1;
	while((number/10)>0)
	{
		number = number/10;
		selfLength+=1;
	}
	// printf("selfLength is %d   \n",selfLength);
	int spaceLength = baseSpace - selfLength;
	char *oringin = "              ";
	char *space_ = (char*)malloc(sizeof(char)*spaceLength+1);
	strncpy(space_, oringin,spaceLength);
	space_[spaceLength] = '\0';
	return space_;
}

void getMatrix(int rows,int coulumes)
{
	int** temp = malloc(sizeof(int*)*rows);
	for (int r = 0; r < rows; ++r)
	{
		temp[r] = (int*)malloc(sizeof(int)*coulumes);
	}
    for (int i = 0; i < rows; ++i)
    {
    	for (int j = 0; j < coulumes; ++j)
    	{
    		temp[i][j] = sumToN(i)*sumToN(j);
    		char* space = getSpace(temp[i][j]);
    		printf("%d%s", temp[i][j],space);
    		free(space);
    	}
    	printf("\n");
    }
    for (int r = 0; r < rows; ++r)
    {
    	free(temp[r]);
    }
    free(temp);
}

 
int main(void)
{
    printf("Hello World!\n");
    printf("the result of sum of 19, 123 is %ld \n", add(19,123));
    // getMatrix(13,15);
	// randomNum(20);
    // changeImage_1();
    // step3_CaculateDistace("../Resources/test_data/d%d.pgm",100);
    step4_getAllHandWritting();
    return 0;
}