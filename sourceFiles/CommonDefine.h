#ifndef ImageProcessTest_CommonDefine_h
#define ImageProcessTest_CommonDefine_h

#ifndef FILECOUNT
#define FILECOUNT 100
#endif

//#ifndef K
//#define K 1
//#endif

static int K = 1;

#ifndef NUMBER_COUNT
#define NUMBER_COUNT 10
#endif

#define NOISE_SIZE 25

#define ISHOWDETAILS 0

#define MAX_WIDTH_HEIGHT 30

typedef struct Image
{
	char fileType_[128];
	char fileSize_[128];
	char fileColorRange_[128];

	int height_;
	int width_;
	int color_;
	int label_;
	//////////////////////////////////////   for train data /////////////////////////
	double distanceWithTestData_;     //// distance with test data
	//////////////////////////////////////   for train data /////////////////////////

	unsigned char** imageData_;
	double* vectorArray_;
	struct Image* (savedK_Point_[30]);   /// record the nearest train data 
}ImageInfo;

typedef struct ImageFolderInfo
{
	ImageInfo *imageArray_ ;
	int* numberLabel_;
	int* isSuccess_;
	int fileCount_;

}ImageFolderInfo;

typedef struct
{
    int x;
    int y;
}Pos;

typedef struct
{
    int x;
    int y;
    int width;
    int height;
}Rect;


#endif