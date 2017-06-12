//
//  ProcessAllInOnePGM.h
//  ImageProcessTest
//
//  Created by kitrol on 17/6/10.
//  Copyright (c) 2017年 kitrol. All rights reserved.
//

#ifndef ImageProcessTest_ProcessAllInOnePGM_h
#define ImageProcessTest_ProcessAllInOnePGM_h
static ImageInfo *imageInfo_ = NULL;

#include "ProcessPGM.h"
void step5();

//// first round
int labelingMatrix(int** originMatrix,int width,int height);
void checkPos(int** originMatrix,int x,int y,int* currentLabel,int width,int height);
int isPosAvailable(int** source,int x,int y,int width, int height);
void traceBackMark(int** originMatrix,int fromX,int fromY);

///  second round
void markForFullDirection(int** originMatrix,int fromX,int fromY);
int roundMark2(int** originMatrix,int width,int height);
int getRidOfNoise(int** originMatrix,int width,int height);

///// separate region
Rect* getRegions(int** originMatrix,int width,int height,int regions);


int saveDebugFile(int** dataSource,int width,int height, const char* fileName);
//
void step5()
{
    printf("step 5 starts \n");
    const char* fileName = "../Resources/contest.pgm";
    
    FILE *inputFile=fopen(fileName,"rb");
    ImageInfo allInOne = getImageInfo(inputFile);
    imageInfo_ = &allInOne;
    fclose(inputFile);
    
    ////   help matrix
    int** markMatrix = (int **)malloc(sizeof(int *)*allInOne.width_);
    for (int index=0; index < allInOne.width_; ++index)
    {
        markMatrix[index] = (int*)malloc(sizeof(int)*allInOne.height_);
    }
    
    ///////   init help martrix
    const char* outputFileName = "../Resources/contest_label.txt";
    FILE* outPut = fopen(outputFileName,"wb");
    if (outPut)
    {
        fprintf(outPut,"%s",allInOne.fileType_);
        fprintf(outPut,"%d %d\n",allInOne.width_,allInOne.height_ );
        fprintf(outPut,"%d\n",allInOne.color_);
        for (int row=0; row<allInOne.height_; ++row)
        {
            for (int line=0; line<allInOne.width_; ++line)
            {
                allInOne.imageData_[line][row] = allInOne.color_ - allInOne.imageData_[line][row];
                markMatrix[line][row] = (int)allInOne.imageData_[line][row]==0?0:1; /// int
                fputc(allInOne.imageData_[line][row]!=0?'1':'0',outPut);// char
            }
            fputs("\n", outPut);
        }
    }
    fclose(outPut);
    
    int firstLabelCnt = labelingMatrix(markMatrix,allInOne.width_,allInOne.height_);
    saveDebugFile(markMatrix,allInOne.width_,allInOne.height_, "../Resources/contest_firstRound.txt");
    int noiseCnt = getRidOfNoise(markMatrix,allInOne.width_,allInOne.height_);
    saveDebugFile(markMatrix,allInOne.width_,allInOne.height_, "../Resources/contest_noiseOut.txt");
    
    int secondLabelCnt = roundMark2(markMatrix,allInOne.width_,allInOne.height_);
    saveDebugFile(markMatrix,allInOne.width_,allInOne.height_, "../Resources/contest_secondRound.txt");

    printf("First Label %d Noise Count %d Final Label %d\n",firstLabelCnt,noiseCnt,secondLabelCnt);
    
    Rect* regions = getRegions(markMatrix,allInOne.width_,allInOne.height_,secondLabelCnt);
    
    for (int mark=0; mark<secondLabelCnt; ++mark)
    {
        Rect region = regions[mark];
        printf("For region %d x %d y %d width %d height %d\n",mark,region.x,region.y,region.width,region.height);
    }
    /////   free Memory
    free(regions);
    for (int index = 0; index<allInOne.width_; ++index)
    {
        free(markMatrix[index]);
    }
    free(markMatrix);
    freeImageInfo(&allInOne);
}


int labelingMatrix(int** originMatrix,int width,int height)
{
    int label = 1;
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            checkPos(originMatrix, i, j, &label, width, height);
        }
    }
    return label;
}

void traceBackMark(int** originMatrix,int fromX,int fromY)
{
    int x_1 = fromX-1;       /// A B C
    int x_2 = fromX;         /// D E F   current pos is C
    int x_3 = fromX+1;       /// G H K
    
    int y_1 = fromY-1;
    int y_2 = fromY;
    
    int currentMark = originMatrix[fromX][fromY];
    if (currentMark<=0)
    {
        return ;
    }
    int result_D = isPosAvailable(originMatrix,x_1,y_2,imageInfo_->width_,imageInfo_->height_);
    int result_F = isPosAvailable(originMatrix,x_3,y_2,imageInfo_->width_,imageInfo_->height_);
    int result_A = isPosAvailable(originMatrix,x_1,y_1,imageInfo_->width_,imageInfo_->height_);
    int result_B = isPosAvailable(originMatrix,x_2,y_1,imageInfo_->width_,imageInfo_->height_);
    int result_C = isPosAvailable(originMatrix,x_3,y_1,imageInfo_->width_,imageInfo_->height_);
    
    if (result_D>0 && result_D>currentMark)
    {
        originMatrix[x_1][y_2] = currentMark;
        traceBackMark(originMatrix,x_1,y_2);
    }
    if (result_F>0 && result_F>currentMark)
    {
        originMatrix[x_3][y_2] = currentMark;
        traceBackMark(originMatrix,x_3,y_2);
    }
    if (result_A>0 && result_A>currentMark)
    {
        originMatrix[x_1][y_1] = currentMark;
        traceBackMark(originMatrix,x_1,y_1);
    }
    if (result_B>0 && result_B>currentMark)
    {
        originMatrix[x_2][y_1] = currentMark;
        traceBackMark(originMatrix,x_2,y_1);
    }
    if (result_C>0 && result_C>currentMark)
    {
        originMatrix[x_3][y_1] = currentMark;
        traceBackMark(originMatrix,x_3,y_1);
    }
}

void checkPos(int** originMatrix,int x,int y,int* currentLabel,int width,int height)
{
    int x_1 = x-1;       /// A B C
    int x_2 = x;         /// D E F   current pos is C
    int x_3 = x+1;       /// G H K
    
    int y_1 = y-1;
    int y_2 = y;
    int y_3 = y+1;
    int mark = *currentLabel;
    if (isPosAvailable(originMatrix,x,y,width,height)>0)  ///current pos is not 0
    {
        int result_A = isPosAvailable(originMatrix,x_1,y_1,width,height);
        int result_B = isPosAvailable(originMatrix,x_2,y_1,width,height);
        int result_C = isPosAvailable(originMatrix,x_3,y_1,width,height);
        int result_D = isPosAvailable(originMatrix,x_1,y_2,width,height);
        
        int beforeArray[4] = {result_A,result_B,result_C,result_D};
        int labeled = 0;
        int minMarkBefore = 1024;
        for (int index =0; index<4; index++)
        {
            if (beforeArray[index]>0 && minMarkBefore>beforeArray[index])
            {
                minMarkBefore = beforeArray[index];
                labeled = 1;
            }
        }
        originMatrix[x][y] = minMarkBefore;
        mark = minMarkBefore;
        
        if (labeled == 0)     //// new aera
        {
            originMatrix[x][y] = *currentLabel;
            if (ISHOWDETAILS) { printf("x %d y %d mark as %d \n",x,y,originMatrix[x][y]);}
            *currentLabel += 1;
        }
    }
    
    if (isPosAvailable(originMatrix,x_3,y_2,width,height) > 0) //F
    {
        originMatrix[x_3][y_2] = mark;
    }
    if (isPosAvailable(originMatrix,x_1,y_3,width,height) > 0) //G
    {
        originMatrix[x_1][y_3] = mark;
    }
    if (isPosAvailable(originMatrix,x_2,y_3,width,height) > 0) //H
    {
        originMatrix[x_2][y_3] = mark;
    }
    if (isPosAvailable(originMatrix,x_3,y_3,width,height) > 0) //K
    {
        originMatrix[x_3][y_3] = mark;
    }
    traceBackMark(originMatrix, x, y);
}

int isPosAvailable(int** source,int x,int y,int width, int height)
{
    if ((x<0)||(x>=width)||(y<0)||(y>=height))
    {
        return -1; /// beyond boundary
    }
    return source[x][y];
}

void markForFullDirection(int** originMatrix,int fromX,int fromY)
{
    int x_1 = fromX-1;       /// A B C
    int x_2 = fromX;         /// D E F   current pos is C
    int x_3 = fromX+1;       /// G H K
    
    int y_1 = fromY-1;
    int y_2 = fromY;
    int y_3 = fromY+1;
    
    int currentMark = originMatrix[fromX][fromY];
    if (currentMark<=0)
    {
        return ;
    }

    int result_A = isPosAvailable(originMatrix,x_1,y_1,imageInfo_->width_,imageInfo_->height_);
    int result_B = isPosAvailable(originMatrix,x_2,y_1,imageInfo_->width_,imageInfo_->height_);
    int result_C = isPosAvailable(originMatrix,x_3,y_1,imageInfo_->width_,imageInfo_->height_);
    
    int result_D = isPosAvailable(originMatrix,x_1,y_2,imageInfo_->width_,imageInfo_->height_);
    int result_F = isPosAvailable(originMatrix,x_3,y_2,imageInfo_->width_,imageInfo_->height_);
    
    int result_G = isPosAvailable(originMatrix,x_1,y_3,imageInfo_->width_,imageInfo_->height_);
    int result_H = isPosAvailable(originMatrix,x_2,y_3,imageInfo_->width_,imageInfo_->height_);
    int result_K = isPosAvailable(originMatrix,x_3,y_3,imageInfo_->width_,imageInfo_->height_);
    

    if (result_A>0 && result_A>currentMark)
    {
        originMatrix[x_1][y_1] = currentMark;
        traceBackMark(originMatrix,x_1,y_1);
    }
    if (result_B>0 && result_B>currentMark)
    {
        originMatrix[x_2][y_1] = currentMark;
        traceBackMark(originMatrix,x_2,y_1);
    }
    if (result_C>0 && result_C>currentMark)
    {
        originMatrix[x_3][y_1] = currentMark;
        traceBackMark(originMatrix,x_3,y_1);
    }
    
    if (result_D>0 && result_D>currentMark)
    {
        originMatrix[x_1][y_2] = currentMark;
        traceBackMark(originMatrix,x_1,y_2);
    }
    if (result_F>0 && result_F>currentMark)
    {
        originMatrix[x_3][y_2] = currentMark;
        traceBackMark(originMatrix,x_3,y_2);
    }
    
    if (result_G>0 && result_G>currentMark)
    {
        originMatrix[x_1][y_3] = currentMark;
        traceBackMark(originMatrix,x_1,y_3);
    }
    if (result_H>0 && result_H>currentMark)
    {
        originMatrix[x_2][y_3] = currentMark;
        traceBackMark(originMatrix,x_2,y_3);
    }
    if (result_K>0 && result_K>currentMark)
    {
        originMatrix[x_3][y_3] = currentMark;
        traceBackMark(originMatrix,x_3,y_3);
    }
}

int isFoundFromArray(int* array,int range,int item)
{
    for (int i = 1; i<= range; ++i)
    {
        if (array[i] == item)
        {
            return i;
        }
    }
    return 0;
}

int roundMark2(int** originMatrix,int width,int height)
{
    int index = 1;
    int corresponding[1024] = {0};
    int countForRegion[1024] = {0};
    corresponding[1] = 1;
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int firstRoundMarkResult = originMatrix[i][j];
            if (firstRoundMarkResult == 0)
            {
                continue;
            }
            int savedBedore = isFoundFromArray(corresponding,index,firstRoundMarkResult);
            if (savedBedore == 0)
            {
                corresponding[index+1] = firstRoundMarkResult;
                countForRegion[index+1] += 1;
                originMatrix[i][j] = index+1;
                ++index;
            }
            else
            {
                originMatrix[i][j] = savedBedore;
                countForRegion[savedBedore] += 1;
            }
        }
    }
    
    
    for (int i=1; i<=index; ++i)
    {
        if (countForRegion[i] < NOISE_SIZE)
        {
            corresponding[i] = 0;
        }
    }
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int secondRoundMarkResult = originMatrix[i][j];
            if (corresponding[secondRoundMarkResult] == 0) /// noise
            {
                originMatrix[i][j] = 0;
            }
        }
    }
    return index;
}

int getRidOfNoise(int** originMatrix,int width,int height)
{
    int noiseCnt = 0;
    int maxLabel = 0;
    int countForRegion[1024] = {0};
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int firstRoundMarkResult = originMatrix[i][j];
            if (firstRoundMarkResult == 0)
            {
                continue;
            }
            if (firstRoundMarkResult > maxLabel)
            {
                maxLabel = firstRoundMarkResult;
            }
            countForRegion[firstRoundMarkResult] += 1;
        }
    }
    
    
    for (int i=1; i<=maxLabel; ++i)
    {
        if (countForRegion[i] < NOISE_SIZE)
        {
            countForRegion[i] = 0;
            noiseCnt++;
        }
    }
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int firstRoundMarkResult = originMatrix[i][j];
            if (firstRoundMarkResult == 0)
            {
                continue;
            }
            if (countForRegion[firstRoundMarkResult] == 0) /// noise
            {
                originMatrix[i][j] = 0;
            }
        }
    }
    return noiseCnt;
}

int saveDebugFile(int** dataSource,int width,int height, const char* fileName)
{
    FILE* outPut = fopen(fileName,"wb");
    if (outPut)
    {
        for (int row=0; row<height; ++row)
        {
            for (int line=0; line<width; ++line)
            {
                fputc(dataSource[line][row]!=0?'a'-1+dataSource[line][row]:'0',outPut);// char
            }
            fputs("\n", outPut);
        }
    }
    else
    {
        return 0;
    }
    fclose(outPut);
    return 1;
}
Rect* getRegions(int** originMatrix,int width,int height,int regions)
{
    Rect *rectArray = (Rect *)malloc(sizeof(Rect)*regions);
    int boundary = width>height?width:height;
    for (int index = 0; index <regions; ++index)
    {
                           ////// minx miny maxx maxy
        rectArray[index] = (Rect){boundary,boundary,0,0};
    }
//    typedef struct
//    {
//        int x;
//        int y;
//        int width;
//        int height;
//    }Rect;
    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int mark = originMatrix[i][j];
            if (mark == 0)
            {
                continue;
            }
            Rect *region = &rectArray[mark-1];
            if (i < region->x)
            {
                region->x = i; //minx
            }
            if (j < region->y)
            {
                region->y = j; //miny
            }
            
            if (i > region->width)
            {
                region->width = i; ///maxx
            }
            if (j > region->height) ///maxy
            {
                region->height = j;
            }
        }
    }
    for (int index = 0; index <regions; ++index)
    {
        Rect *region = &rectArray[index];
        //// height = maxY - minY  width = maxX - minX;
        region->height = region->height - region->y;
        region->width = region->width - region->x;
    }
    
    return rectArray;
}

#endif
