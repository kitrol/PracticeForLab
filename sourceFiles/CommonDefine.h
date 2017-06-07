
#ifndef FILECOUNT
#define FILECOUNT 100
#endif

#ifndef K
#define K 3
#endif

#ifndef NUMBER_COUNT
#define NUMBER_COUNT 10
#endif

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