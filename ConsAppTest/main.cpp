#include "main.h"


bool cvExample_SearchCircles(std::string strPathOfObject, cv::Mat &imgFinalResult)
{
	cv::Mat imgOriginal = cv::imread(strPathOfObject);          // open image
	if (!imgOriginal.empty())
	{
		cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
		cv::imshow("imgOriginal", imgOriginal); cv::waitKey(0);
	}
	else
	{
		return false;
	}

#if 1
	cv::Mat src_gray2;
	cv::cvtColor(imgOriginal, src_gray2, CV_BGR2GRAY);

	cv::namedWindow("imgGray2", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgGray2", src_gray2); cv::waitKey(0);

	GaussianBlur(src_gray2, src_gray2, cv::Size(9, 9), 2, 2);

	cv::namedWindow("imgGaussianBlur", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgGaussianBlur", src_gray2); cv::waitKey(0);

	std::vector<cv::Vec3f> circles;
	/*
	HoughCircles(src_gray2
	, circles
	,CV_HOUGH_GRADIENT
	,4   // accumulator resolution (size of the image / 2)
	,5  // minimum distance between two circles
	,100 // Canny high threshold
	,100 // minimum number of votes
	,0   // min radius
	,100//0 // max radius
	);
	*/

//*
	//HoughCircles(src_gray2, circles, CV_HOUGH_GRADIENT, 1, src_gray2.rows / 8, 200, 20, 0, 0);
	HoughCircles(src_gray2, circles, CV_HOUGH_GRADIENT,
		2,   // accumulator resolution (size of the image / 2)
		5,  // minimum distance between two circles
		100, // Canny high threshold
		100, // minimum number of votes
		0, 1000); // min and max radius
//*/


//	HoughCircles(src_gray2, circles, cv::HOUGH_GRADIENT, 1, 10,
//		100, 30, 0, 0 //1, 30 // change the last two parameters
//					  // (min_radius & max_radius) to detect larger circles
//	);


	std::cout << circles.size() << std::endl;
	std::cout << "end of test" << std::endl;

	cv::circle(src_gray2
		, cv::Point(100, 100) // circle centre
		, 100      // circle radius
		, SCALAR_GREEN   //cv::Scalar(255) // color
		, 2				// thickness
	);
//*
	std::vector<cv::Vec3f>::const_iterator itc = circles.begin();
	while (itc != circles.end())
	{
		//cv::circle(InputOutputArray img, Point center, int radius, const Scalar& color, int thickness = 1, int lineType = LINE_8, int shift = 0)
		//int a0=0, a1=0, a2=0, a3=0, a4=0;
		//a0 = (*itc)[0];
		//a1 = (*itc)[1];
		//a2 = (*itc)[2];

		cv::circle(src_gray2
			,cv::Point((*itc)[0], (*itc)[1]) // circle centre
			,(*itc)[2]      // circle radius
			,SCALAR_GREEN   //cv::Scalar(255) // color
			,2				// thickness
		    );              

		++itc;
	}
//*/

	cv::namedWindow("imgeWithCircles", CV_WINDOW_AUTOSIZE);
	cv::imshow("imgeWithCircles", src_gray2); cv::waitKey(0);

	return circles.size() ? true : false;

#endif


#if 0
	//*

	// Use the Hough transform to detect circles in the combined threshold image
	cv::Mat img = imgOriginalCopy;
	if (imgOriginalCopy.empty())
	{
	return false;
	}

	cv::Mat cimg;
	medianBlur(img, imgOriginalCopy, 5);
	cvtColor(imgOriginalCopy, cimg, cv::COLOR_GRAY2BGR);
	//	cvtColor(imgOriginalCopy, imgFinalResult, cv::COLOR_GRAY2BGR);
	std::vector<cv::Vec3f> circles;
	HoughCircles(imgOriginalCopy, circles, cv::HOUGH_GRADIENT, 1, 10,
	100, 30, 0, 0 //1, 30 // change the last two parameters
	// (min_radius & max_radius) to detect larger circles
	);

	for (size_t i = 0; i < circles.size(); i++)
	{
	cv::Vec3i c = circles[i];
	circle(imgFinalResult, cv::Point(c[0], c[1]), c[2], cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
	circle(imgFinalResult, cv::Point(c[0], c[1]), 2, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
	}
	//*/


	imshow("detected circles", cimg); cv::waitKey();

	//imgFinalResult = cimg;
	cv::namedWindow("imgCircles", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgCircles", imgFinalResult); cv::waitKey(0);
#endif
}



bool cvExample_ShowSomeFunctions(cv::Mat imgOriginalCopy, cv::Mat &imgFinalResult)
{
	cv::Mat imgGrayscale;       // grayscale of input image
	cv::Mat imgBlurred;         // intermediate blured image
	cv::Mat imgCanny;           // Canny edge image

	if (imgOriginalCopy.empty()) {                                  // if unable to open image
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		_getch();                                               // may have to modify this line if not using Windows
		return false;                                              // and exit program
	}

	cv::cvtColor(imgOriginalCopy, imgGrayscale, CV_BGR2GRAY);       // convert to grayscale

	cv::GaussianBlur(imgGrayscale,  // input image
		imgBlurred,                 // output image
		cv::Size(5, 5),             // smoothing window width and height in pixels
		1.5);                       // sigma value, determines how much the image will be blurred

	cv::Canny(imgBlurred,           // input image
									//imgCanny,                   // output image
		imgFinalResult,
		100,                        // low threshold
		200);                       // high threshold

									// declare windows
									// CV_WINDOW_AUTOSIZE is the default
	cv::namedWindow("imgOriginalCopy", CV_WINDOW_AUTOSIZE);     // note: you can use CV_WINDOW_NORMAL which allows resizing the window
	cv::imshow("imgOriginalCopy", imgOriginalCopy); cv::waitKey(0); // show window, until user presses a key

	cv::namedWindow("imgGrayscale", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgGrayscale", imgGrayscale); cv::waitKey(0);

	cv::namedWindow("imgBlurred", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgBlurred", imgBlurred); cv::waitKey(0);

	cv::namedWindow("imgCanny", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgCanny", imgFinalResult); cv::waitKey(0);
}




bool cvExample_SearchCirclesOnCannyImg(cv::Mat imgOriginalCopy, cv::Mat &imgFinalResult)
{
	cv::Mat imgOriginal = imgOriginalCopy.clone();
	cv::Mat imgGrayscale;       // grayscale of input image
	cv::Mat imgBlurred;         // intermediate blured image
	cv::Mat imgCanny;           // Canny edge image

	if (imgOriginalCopy.empty()) {                                  // if unable to open image
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		_getch();                                               // may have to modify this line if not using Windows
		return false;                                              // and exit program
	}

	cv::cvtColor(imgOriginalCopy, imgGrayscale, CV_BGR2GRAY);       // convert to grayscale

	cv::GaussianBlur(imgGrayscale,  // input image
		imgBlurred,                 // output image
		cv::Size(5, 5),             // smoothing window width and height in pixels
		1.5);                       // sigma value, determines how much the image will be blurred

	cv::Canny(imgBlurred,           // input image
									//imgCanny,                   // output image
		imgFinalResult,
		100,                        // low threshold
		200);                       // high threshold

	int imgHeight = imgOriginal.size[0];
	int imgWidth = imgOriginal.size[1];
	std::vector<cv::Vec3f> circles;
	HoughCircles(imgFinalResult, circles, CV_HOUGH_GRADIENT
		,2      // accumulator resolution (size of the image / 2)
		,imgWidth/2 //5      // minimum distance between two circles
		,100    // Canny high threshold
		,100    // minimum number of votes
		,0      // min radius
		, imgWidth/3 //1000 // max radius
	);		  

	std::cout << circles.size() << std::endl;
	std::cout << "end of test" << std::endl;

	//*
	std::vector<cv::Vec3f>::const_iterator itc = circles.begin();
	while (itc != circles.end())
	{
		//cv::circle(InputOutputArray img, Point center, int radius, const Scalar& color, int thickness = 1, int lineType = LINE_8, int shift = 0)
		//int a0=0, a1=0, a2=0, a3=0, a4=0;
		//a0 = (*itc)[0];
		//a1 = (*itc)[1];
		//a2 = (*itc)[2];

		cv::circle(imgOriginal
			, cv::Point((*itc)[0], (*itc)[1]) // circle centre
			, (*itc)[2]      // circle radius
			, SCALAR_GREEN   //cv::Scalar(255) // color
			, 1				// thickness
		);

		++itc;
	}
	//*/

	cv::namedWindow("imgOriginalCopy", CV_WINDOW_AUTOSIZE);     // note: you can use CV_WINDOW_NORMAL which allows resizing the window
	cv::imshow("imgOriginalCopy", imgOriginal); cv::waitKey(0); // show window, until user presses a key

	cv::namedWindow("imgGrayscale", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgGrayscale", imgGrayscale); cv::waitKey(0);

	cv::namedWindow("imgBlurred", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgBlurred", imgBlurred); cv::waitKey(0);

	cv::namedWindow("imgCanny", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgCanny", imgFinalResult); cv::waitKey(0);
}


#if 1
///////////////////////////////////////////////////////////////////////////////////////////////////
void main(void)
{
#if	EXAMPLE_FANN
	int res =  fannSimpleTrainExample();
#endif

#if	EXAMPLE_OPENCV
	cv::Mat imgOriginal;  // input  image
	cv::Mat imgOriginalCopy;  // input  image
	cv::Mat imgFinal;     // output image

	std::string strPathOfObject("..\\pic\\pic.jpg");

//*
	//imgOriginal = cv::imread("img.jpg");          // open image
	imgOriginal = cv::imread(strPathOfObject);          // open image
	int imgHeight = imgOriginal.size[0];
	int imgWidth  = imgOriginal.size[1];

	/*
	std::cout << "temp.dims = " << imgOriginal.dims << " temp.size = [";
	for (int i = 0; i < imgOriginal.dims; ++i) {
		// Height (|) X Width (_)
		if (i) std::cout << " X ";
		std::cout << imgOriginal.size[i];
	}
	std::cout << "] temp.channels = " << imgOriginal.channels() << std::endl;
	*/




	if (imgOriginal.empty()) {                                  // if unable to open image
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		_getch();                                               // may have to modify this line if not using Windows
		//return(0);                                              // and exit program
	}
	else
	{ imgOriginalCopy = imgOriginal.clone(); }

	cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
	cv::imshow("image", imgOriginal); cv::waitKey(0);

	// Exaple Function we will need:
//	cvExample_ShowSomeFunctions(imgOriginal.clone(), imgFinal);
	//cvExample_SearchCircles(imgOriginal, imgFinal);
//	cvExample_SearchCircles(strPathOfObject, imgFinal);
	cvExample_SearchCirclesOnCannyImg(imgOriginal.clone(), imgFinal);

//	fun_onPictureCarRecogntionAndCounting(strPathOfObject, imgFinal);
//*/

	// CV_WINDOW_AUTOSIZE is the default
	//cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	//cv::imshow("imgOriginal", imgOriginal); cv::waitKey(0);

	cv::namedWindow("imgFinal", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
	cv::imshow("imgFinal", imgFinal); cv::waitKey(0);

	cv::waitKey(0);                 // hold windows open until user presses a key
#endif




}




#endif




