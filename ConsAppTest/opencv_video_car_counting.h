#pragma once

#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows

// How To Set Envoriment for OpenCV correctly
// https://lithiumdenis.wordpress.com/2016/01/28/windows-установка-opencv-3-1-на-vs-2015/

#include <opencv2/opencv.hpp>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d/features2d.hpp"


#include "Blob.h"

#define SHOW_STEPS            // un-comment or comment this line to show steps or not

#define MANAGE_VIDEO	0
#define MANAGE_PICTURE	1

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// general Idea of this code is searching for car conturs, on 2 different frames
// while the car is moving it lets a trace. And all the moving object can be found by
// searching and matching theese traces
bool fun_onPictureCarRecogntionAndCounting(std::string strPathOfObject, cv::Mat &imgFinalResult);
bool fun_onVideoCarRecogntionAndCounting(std::string strPathOfObject, cv::Mat &imgFinalResult); // this function uses all the ither functions underneath


void fun_matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
// function prototypes ////////////////////////////////////////////////////////////////////////////
void fun_matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void fun_addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void fun_addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double fun_distanceBetweenPoints(cv::Point point1, cv::Point point2);
void fun_drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void fun_drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
bool fun_checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount);
void fun_drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void fun_drawCarCountOnImage(int &carCount, cv::Mat &imgFrame2Copy);

