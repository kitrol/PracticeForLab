#include <stdio.h>

long int add(int x1, int x2)
{
	return (long int)(x1+x2);
}

int sumToN(int n)
{
	int result_ = 0;
	for (int i = 1; i <= n; ++i)
	{
		result_ += i;
	}
	return result_;
}

double caculateDistance(double* vector_1,double* vector_2,int size)
{
	double distance_ = 0.0;
	for (int i = 0; i < size; ++i)
	{
		distance_ += (vector_1[i] - vector_2[i])*(vector_1[i] - vector_2[i]);
	}
	return sqrt(distance_);
}

//////////////////////////////  IMAGE   ///////////////////////////////////////////////

void freeImageInfo(ImageInfo *info)
{
	for (int i = 0; i < info->width_; ++i)
	{
		free(info->imageData_[i]);
	}
	free(info->imageData_);
	free(info->vectorArray_);
}

void freeImageFolderInfo(ImageFolderInfo *info)
{
	for (int i = 0; i < info->fileCount_; ++i)
	{
        if (info->isSuccess_[i])
        {
            freeImageInfo(&(info->imageArray_[i]));
        }
	}
	free(info->imageArray_);
	free(info->numberLabel_);
	free(info->isSuccess_);
}

void initFileSize(ImageInfo* info)
{
	char *position = strchr(info->fileSize_,' ');
	if (position)
	{
	 	// printf("position is%dend\n",position-info->fileSize_ );
		char* p;   
		char* buffer;   
		const char* delims=" ";
		buffer=info->fileSize_;  
		p=strtok(buffer,delims);
		info->width_ = atoi(p);
		p=strtok(NULL,delims);
		// printf("p is%s\n", p);
		info->height_ = atoi(p);
	 }
	info->color_ = atoi(info->fileColorRange_);
	// printf("width%d   height%d    color%d\n",info->width_,info->height_,info->color_);
}

unsigned char* readAllFile(char* fileName)
{
	FILE* file=fopen(fileName,"rb");
	if (file)
	{
		long lSize;
	    unsigned char * buffer;
	    size_t result;
	    fseek (file , 0 , SEEK_END);
	    lSize = ftell (file);
	    rewind (file);
	    buffer = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
	    if (buffer == NULL)  
	    {
	        fputs ("Memory error",stderr);
	        exit (2);
	    }
	    result = fread (buffer,1,lSize,file);
	    if (result != lSize)
	    {
	        fputs ("Reading error",stderr);
	        exit (3);
	    }
	    return buffer;
    }  
    // printf("%s",       );   
    fclose(file);
    return NULL;
}




