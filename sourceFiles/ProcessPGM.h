#include <stdio.h>
#include <stdlib.h>

// typedef void (*pointerFuncWithChar)(char* fileSize);

//  avoid of reading annotate

void getAttributeForPGM(char *dest,FILE *f);
void changeToOpositeGrayAndSave(ImageInfo *info,const char * newFileName);

ImageInfo getImageInfo(FILE *fileSource);
void readImageData(FILE *fileSource,ImageInfo* info);
void normalizationForFile(ImageInfo* source);

ImageFolderInfo readDataFromFolder(const char* fileFormat ,const char* realNumberFileName ,int fileCount, int fromNum);
ImageFolderInfo getFolderInfo(int fileCount);

int updataDistanceForImage(ImageInfo *info, ImageInfo *newTarget, double dis);
void caculateNearestKPoint(ImageInfo *info, ImageFolderInfo* trainDataInfo);
int showResultForTestImage(ImageInfo *info);

void changeImage_1();
void step3_CaculateDistace(char* fileFormat ,int fileCount);
void step4_getAllHandWritting();


void getAttributeForPGM(char *dest,FILE *f)
{
	fgets(dest, 128, f);
	while(dest[0] == '#')
	{
		fgets(dest, 128, f);
	}
}


void changeToOpositeGrayAndSave(ImageInfo *info,const char * newFileName)
{
	FILE* newFile_=fopen(newFileName,"wb");
	if (newFile_)
	{
		fprintf(newFile_,"%s",info->fileType_);            
	    fprintf(newFile_,"%d %d\n",info->width_,info->height_);       
	    fprintf(newFile_,"%d\n",info->color_);           
	    for(int i=0;i<info->width_;i++)
	    {
			for(int j=0;j<info->height_;j++)
			{
	    		fputc((info->color_- (int)info->imageData_[i][j]),newFile_);
	    	}
	    }
	}
    fclose(newFile_);
}

ImageInfo getImageInfo(FILE *fileSource)
{
	fseek(fileSource, 0, SEEK_SET);
	ImageInfo info={"","","",0,0,0,0,0.0};
	for (int i = 0; i < 20; ++i)
	{
		info.savedK_Point_[i] = NULL;
	}
	getAttributeForPGM(info.fileType_,fileSource);
	getAttributeForPGM(info.fileSize_,fileSource);
	getAttributeForPGM(info.fileColorRange_,fileSource);
	initFileSize(&info);
	readImageData(fileSource,&info);
	return info;
}

void readImageData(FILE *fileSource,ImageInfo* info)
{
	info->imageData_ = (unsigned char **)malloc(sizeof(unsigned char *)*info->width_);
	for (int r = 0; r < info->width_; ++r)
	{
		info->imageData_[r] = (unsigned char*)malloc(sizeof(unsigned char)*info->height_);
	}
	for (int i = 0; i < info->width_; ++i)
	{
		for (int j = 0; j < info->height_; ++j)
		{
			info->imageData_[i][j] = fgetc(fileSource);
		}
	}
}

ImageFolderInfo readDataFromFolder(const char* fileFormat ,const char* realNumberFileName ,int fileCount, int fromNum)
{
	ImageFolderInfo folderInfo_ = getFolderInfo(fileCount);
	folderInfo_.fileCount_ = fileCount;
	FILE* labelFile=fopen(realNumberFileName,"rb");
	if (labelFile)
	{
		for (int i = 0; i < fileCount; ++i)
		{
            char number[10];
            fgets(number, 3, labelFile);  //char	*fgets(char * __restrict, int, FILE *); read n-1 chars from file
			folderInfo_.numberLabel_[i] = atoi(&number[0]);  /// number for 0 to 9 a char is enough
//			 printf("index is %d the number is  %d\n", i,folderInfo_.numberLabel_[i]);
		}
	}
	fclose(labelFile);

	for (int i = 0; i < fileCount; ++i)
	{
		char fileName_[120];
		sprintf(fileName_,fileFormat,(fromNum+i));
		///  file index is (index+fromNum) cause folder testdata 1000 the first index is from 2000 to 2999 all 1000 files
		FILE *file=fopen(fileName_,"rb");
		if (file)
		{
			folderInfo_.imageArray_[i] = getImageInfo(file);
			folderInfo_.imageArray_[i].label_ = folderInfo_.numberLabel_[i];
			normalizationForFile(&(folderInfo_.imageArray_[i]));
			folderInfo_.isSuccess_[i] = 1;
		}
		fclose(file);
	}
	printf("************  read files form %s success !!!!  ************\n",fileFormat);
	return folderInfo_;
}

ImageFolderInfo getFolderInfo(int fileCount)
{
	ImageFolderInfo Info;
	Info.imageArray_= (ImageInfo *)malloc(sizeof(ImageInfo)*fileCount);
	Info.numberLabel_ = (int *)malloc(sizeof(int)*fileCount);
	Info.isSuccess_ = (int *)malloc(sizeof(int)*fileCount);
    for (int index=0; index<fileCount; ++index)
    {
        Info.isSuccess_[index] = 0;
    }
	return Info;
}

void normalizationForFile(ImageInfo* source)
{
	double sum_ = 0.0;
	source->vectorArray_ = (double *)malloc(sizeof(double)*source->width_*source->height_);
	for (int i = 0; i < source->width_; ++i)
	{
		for (int j = 0; j < source->height_; ++j)
		{

			source->vectorArray_[source->width_*i + j] = (255-source->imageData_[i][j])/255.0;
			sum_ += source->vectorArray_[source->width_*i + j];
		}
	}
	for (int i = 0; i < source->width_; ++i)
	{
		for (int j = 0; j < source->height_; ++j)
		{
			source->vectorArray_[source->width_*i + j] = source->vectorArray_[source->width_*i + j]/sum_;
		}
	}
	// printf("sum_ <%f>  \n",sum_);
}

int updataDistanceForImage(ImageInfo *info, ImageInfo *newTarget, double dis)
{
	for (int index = 0; index < K; ++index)
	{
		ImageInfo *savedTrainTarget = info->savedK_Point_[index];
		if (dis < savedTrainTarget->distanceWithTestData_)
		{
			info->savedK_Point_[index] = newTarget;
			return 1;
			/////////  success to replace old data
		}
	}
	return 0;  /////  failed to replace old data
}

void caculateNearestKPoint(ImageInfo *info, ImageFolderInfo* trainDataInfo)
{
	for (int index = 0; index < trainDataInfo->fileCount_; ++index)
	{
		ImageInfo* trainIamgeInfo = &trainDataInfo->imageArray_[index];
		///////  reset distance
		trainIamgeInfo->distanceWithTestData_ = 0.0;
		int vectorSize = info->width_*info->height_;
		double dis = caculateDistance(info->vectorArray_,trainIamgeInfo->vectorArray_,vectorSize);
		trainIamgeInfo->distanceWithTestData_ = dis;
		// printf("dis is %.10lf  \n", dis);
		if (index < K)
		{
			info->savedK_Point_[index] = trainIamgeInfo;
		}
		else
		{
			updataDistanceForImage(info,trainIamgeInfo,dis);
		}
	}
}

int showResultForTestImage(ImageInfo *info)
{
	int result[10] = {0,0,0,0,0,0,0,0,0,0};
    double distance[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	for (int i = 0; i < K; ++i)
	{
		ImageInfo* trainDataInfo = info->savedK_Point_[i];
		// printf("label si %d \n",trainDataInfo->label_ );
		if (trainDataInfo == NULL)
		{
			break;
		}
		int labelIndex = trainDataInfo->label_;
		result[labelIndex] += 1;
        distance[labelIndex] += trainDataInfo->distanceWithTestData_;
	}

	int maxCnt = 0;
	int predictNum = -1;
	for (int i = 0; i < 10; ++i)
	{
		if (result[i] > maxCnt)
		{
			maxCnt = result[i];
			predictNum = i;
		}
        //// same mark count, select the nearest one
        else if ((result[i]>0) && (result[i] == maxCnt) && (distance[i] < distance[predictNum]))
        {
            maxCnt = result[i];
            predictNum = i;
        }
	}
	// for (int i = 0; i < 10; ++i)
	// {
	// 	printf("count for index %d is %d\n",i, result[i]);
	// }
	int returnValue = 1;
	if (info->label_ != predictNum)
	{
		returnValue = 0;
	}
    if (ISHOWDETAILS)
    {
        printf("when K is %d.real number for test data is %d and prediction is %d. is Correct %s \n",K,info->label_,predictNum,returnValue>0?"true":"false");
    }
	
	return returnValue;
}

void resetFolderData(ImageFolderInfo *info)
{
    for (int index = 0; index <info->fileCount_; ++index)
    {
        ImageInfo* target = &info->imageArray_[index];
        target->distanceWithTestData_ = 0;
        for (int j=0; j<30; ++j)
        {
            target->savedK_Point_[j] = NULL;
        }
    }
}
//////////////////////////////// FUNCTION CALL /////////////////////////////////////////////////////


void changeImage_1()
{
	const char* fileName = "Lenna.pgm";
	FILE* fileSource_=fopen(fileName,"rb");
	if (fileSource_)
	{
		ImageInfo info = getImageInfo(fileSource_);
		changeToOpositeGrayAndSave(&info , "../Resources/oppsiteGrayForLenna.pgm");
		freeImageInfo(&info);
	}
	fclose(fileSource_);
}

void step3_CaculateDistace(char* fileFormat ,int fileCount)
{
	ImageInfo *imageArray_ = (ImageInfo *)malloc(sizeof(ImageInfo)*fileCount);
	int* isSuccess_ = (int *)malloc(sizeof(int)*fileCount);
	for (int i = 0; i < fileCount; ++i)
	{
		char fileName_[40];
		sprintf(fileName_,fileFormat,i);
		FILE *file=fopen(fileName_,"rb");
		isSuccess_[i] = 0;
		if (file)
		{
			imageArray_[i] = getImageInfo(file);
			normalizationForFile(&(imageArray_[i]));
			isSuccess_[i] = 1;
		}
	}
	int index = 0;
	printf("please input an index form 0 to %d\n",fileCount);
	scanf("%d", &index);
	printf("caculate the distance for index = %d with others\n", index);
	for (int i = 1; i < fileCount; ++i)
	{
		if ((index>=0)&&(index < fileCount)&&(isSuccess_[i]==1)&&(isSuccess_[index]==1))
		{
			ImageInfo info0_ = imageArray_[0];
//			ImageInfo infoN_ = imageArray_[index];
			printf("the distance between %d and %d is %1lf\n",index,i+1,caculateDistance(info0_.vectorArray_,info0_.vectorArray_,info0_.width_*info0_.height_));
		}
		
	}
	for (int i = 0; i < fileCount; ++i)
	{
		if (isSuccess_[i]==1)
		{
			freeImageInfo(&(imageArray_[i]));
		}
	}
}

void step4_getAllHandWritting()
{

	ImageFolderInfo trainData_2000 = readDataFromFolder("../Resources/train_data/d%d.pgm","../Resources/train_data/train_label.txt",2000,0);
	ImageFolderInfo testData_100   = readDataFromFolder("../Resources/test_image_100/d%d.pgm","../Resources/test_image_100/test_label_100.txt",100,0);
	ImageFolderInfo testData_1000  = readDataFromFolder("../Resources/test_data_1000/d%d.pgm","../Resources/test_data_1000/test_label_1000.txt",1000,2000);

    float rateFor100[20] = {0};
    float rateFor1000[20] = {0};
    
    int seconds_1 = (int)time((time_t*)NULL);
    printf("Test Start  %d\n", seconds_1);
    for (int t=0; t<20; ++t)
    {
        printf("K is %d \n",K);
        int correctTimes = 0;
        
        for (int i = 0; i < testData_100.fileCount_; ++i)
        {
            ImageInfo info = testData_100.imageArray_[i];
            caculateNearestKPoint(&info,&trainData_2000);
            int result = showResultForTestImage(&info);
            correctTimes += result;
        }
        
        
        double rate = (double) correctTimes/testData_100.fileCount_;
        printf("100 testData totaly correct times is %d rate is %f%%\n\n\n",correctTimes,rate*100);
        rateFor100[t] = rate;
        
        printf("Start test data 1000 with train data \n\n\n");
        correctTimes = 0;
        
        
        for (int i = 0; i < testData_1000.fileCount_; ++i)
        {
            ImageInfo info = testData_1000.imageArray_[i];
            caculateNearestKPoint(&info,&trainData_2000);
            int result = showResultForTestImage(&info);
            correctTimes += result;
        }
        
        rate = (double) correctTimes/testData_1000.fileCount_;
        printf("1000 testData totaly correct times is %d rate is %f%%\n",correctTimes,rate*100);
        rateFor1000[t] = rate;
        
        resetFolderData(&trainData_2000);
        resetFolderData(&testData_100);
        resetFolderData(&testData_1000);
        K = K+1;
    }
    
    FILE* resultFile=fopen("../Resources/testReslut.txt","wb");
    if (resultFile)
    {
        for (int index = 0; index <20; ++index)
        {
            float rate100_ = rateFor100[index];
            float rate1000_ = rateFor1000[index];
            const char* form = "%f,%f\n";
            char fileName_[120];
            sprintf(fileName_,form,rate100_,rate1000_);
            fputs(fileName_, resultFile);
        }
    }
    fclose(resultFile);
    

    printf("Start test data 100 with train data\n\n\n");

	int seconds_2 = (int)time((time_t*)NULL);
    printf("Total time cost %d \n",seconds_2-seconds_1);

	freeImageFolderInfo(&trainData_2000);  /// free memory
	freeImageFolderInfo(&testData_100);
	freeImageFolderInfo(&testData_1000);
}


