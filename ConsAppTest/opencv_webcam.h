#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void getVideoStreamFromWebCam();
void takeOnePictureFromWebCam(std::string _fileNameToSaveFrame);
