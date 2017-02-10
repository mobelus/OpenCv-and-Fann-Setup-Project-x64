
#include "opencv_video_car_counting.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool fun_onPictureCarRecogntionAndCounting(std::string strPathOfObject, cv::Mat &imgFinalResult)
{
	/////////////////////////////////////////////////////
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;
	std::vector<Blob> blobs;
	cv::Point crossingLine[2];
	int carCount = 0;

	cv::Mat imgOriginal;	 // input  image
							 //cv::Mat imgOriginalCopy; // input  image
							 //cv::Mat imgFinal;		 // output image

	//imgOriginal = cv::imread("..\\pic\\pic.jpg");          // open image
	imgOriginal = cv::imread(strPathOfObject);          // open image


	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);

	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;

	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	int frameCount = 2;

	if (!imgOriginal.empty())
	{
		cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
		cv::imshow("imgOriginal", imgOriginal); cv::waitKey(0);

		// !!! Here we Enter the DATA from the Picture
		imgFrame1 = imgOriginal.clone();
		imgFrame2 = imgOriginal.clone();

		std::vector<Blob> currentFrameBlobs;

		cv::Mat imgFrame1Copy = imgFrame1.clone();
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		cv::namedWindow("imgThresh", CV_WINDOW_AUTOSIZE);
		cv::imshow("imgThresh", imgThresh); cv::waitKey(0);


		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

		for (unsigned int i = 0; i < 2; i++) {
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::erode(imgThresh, imgThresh, structuringElement5x5);
		}

		cv::Mat imgThreshCopy = imgThresh.clone();

		// http://stackoverflow.com/questions/30514037/opencv-with-visual-studio-2013c-findcontours-breakpoint-error
		std::vector<std::vector<cv::Point> > contours;
		//std::vector<cv::Mat> contours;

		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		fun_drawAndShowContours(imgThresh.size(), contours, "imgContours");

		std::vector<std::vector<cv::Point> > convexHulls(contours.size());
		//std::vector<cv::Mat> convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			cv::convexHull(contours[i], convexHulls[i]);
		}

		fun_drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.currentBoundingRect.area() > 400 &&
				possibleBlob.dblCurrentAspectRatio > 0.2 &&
				possibleBlob.dblCurrentAspectRatio < 4.0 &&
				possibleBlob.currentBoundingRect.width > 30 &&
				possibleBlob.currentBoundingRect.height > 30 &&
				possibleBlob.dblCurrentDiagonalSize > 60.0 &&
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		fun_drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		else {
			fun_matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
		}

		fun_drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		//*
		imgFrame2Copy = imgFrame2.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

		fun_drawBlobInfoOnImage(blobs, imgFrame2Copy);

		bool blnAtLeastOneBlobCrossedTheLine = fun_checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

		if (blnAtLeastOneBlobCrossedTheLine == true) {
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
		}

		fun_drawCarCountOnImage(carCount, imgFrame2Copy);

		cv::namedWindow("imgFrame2Copy", CV_WINDOW_AUTOSIZE);
		cv::imshow("imgFrame2Copy", imgFrame2Copy); cv::waitKey(0);

		blnFirstFrame = false;
		frameCount++;
		chCheckForEscKey = cv::waitKey(1);
	}
	else
	{
		std::cout << "error: image not read from file\n\n";     // show error message on command line
		_getch();                                               // may have to modify this line if not using Windows
		return(0);                                              // and exit program
	}

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

	return(0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool fun_onVideoCarRecogntionAndCounting(std::string strPathOfObject, cv::Mat &imgFinalResult)
{
	/////////////////////////////////////////////////////
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;
	std::vector<Blob> blobs;
	cv::Point crossingLine[2];
	int carCount = 0;


	cv::VideoCapture capVideo;
	
	capVideo.open(strPathOfObject);
	//capVideo.open("..\\pic\\CarsDrivingUnderBridge.mp4");
	//capVideo.open("C:\\_WORK_RGS\\MyFiles\\DISSER\\ISTRATOV\\videos\\ConsoleApplicationOpenCV\\pic\\CarsDrivingUnderBridge.mp4");

	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "error reading video file" << std::endl << std::endl;      // show error message
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: video file must have at least two frames";
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);
	}

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);


	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);

	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;

	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	int frameCount = 2;


	while (capVideo.isOpened() && chCheckForEscKey != 27)
	{

		std::vector<Blob> currentFrameBlobs;

		cv::Mat imgFrame1Copy = imgFrame1.clone();
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		cv::imshow("imgThresh", imgThresh);

		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
		cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

		for (unsigned int i = 0; i < 2; i++) {
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::dilate(imgThresh, imgThresh, structuringElement5x5);
			cv::erode(imgThresh, imgThresh, structuringElement5x5);
		}

		cv::Mat imgThreshCopy = imgThresh.clone();

		// http://stackoverflow.com/questions/30514037/opencv-with-visual-studio-2013c-findcontours-breakpoint-error
		std::vector<std::vector<cv::Point> > contours;
		//std::vector<cv::Mat> contours;

		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		fun_drawAndShowContours(imgThresh.size(), contours, "imgContours");

		std::vector<std::vector<cv::Point> > convexHulls(contours.size());
		//std::vector<cv::Mat> convexHulls(contours.size());

		for (unsigned int i = 0; i < contours.size(); i++) {
			cv::convexHull(contours[i], convexHulls[i]);
		}

		fun_drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

		for (auto &convexHull : convexHulls) {
			Blob possibleBlob(convexHull);

			if (possibleBlob.currentBoundingRect.area() > 400 &&
				possibleBlob.dblCurrentAspectRatio > 0.2 &&
				possibleBlob.dblCurrentAspectRatio < 4.0 &&
				possibleBlob.currentBoundingRect.width > 30 &&
				possibleBlob.currentBoundingRect.height > 30 &&
				possibleBlob.dblCurrentDiagonalSize > 60.0 &&
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
				currentFrameBlobs.push_back(possibleBlob);
			}
		}

		fun_drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {
			for (auto &currentFrameBlob : currentFrameBlobs) {
				blobs.push_back(currentFrameBlob);
			}
		}
		else {
			fun_matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
		}

		fun_drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

		//*
		imgFrame2Copy = imgFrame2.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

		fun_drawBlobInfoOnImage(blobs, imgFrame2Copy);

		bool blnAtLeastOneBlobCrossedTheLine = fun_checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

		if (blnAtLeastOneBlobCrossedTheLine == true) {
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
		}
		else {
			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
		}

		fun_drawCarCountOnImage(carCount, imgFrame2Copy);


		cv::imshow("imgFrame2Copy", imgFrame2Copy);
		//cv::waitKey(0);                 // UNCOMMENT !!! this line to go frame by frame for debugging


		// now we prepare for the next iteration
		currentFrameBlobs.clear();

		imgFrame1 = imgFrame2.clone();           // move frame 1 up to where frame 2 is

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
			capVideo.read(imgFrame2);
		}
		else {
			std::cout << "end of video\n";
			break;
		}


		blnFirstFrame = false;
		frameCount++;
		chCheckForEscKey = cv::waitKey(1);
	}

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
		cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

	return(0);
}





#if 0 // BOTH FUNCTIONS IN ONE Using #define
//void fun_finalCarRecogntionAndCounting(cv::Mat imgOriginalCopy, cv::Mat &imgFinalResult)
void fun_finalCarRecogntionAndCounting(std::string strPathOfObject, cv::Mat &imgFinalResult)
{
	/////////////////////////////////////////////////////
	cv::Mat imgFrame1;
	cv::Mat imgFrame2;
	std::vector<Blob> blobs;
	cv::Point crossingLine[2];
	int carCount = 0;

#if MANAGE_PICTURE
	cv::Mat imgOriginal;	 // input  image
							 //cv::Mat imgOriginalCopy; // input  image
							 //cv::Mat imgFinal;		 // output image

	//imgOriginal = cv::imread("..\\pic\\pic.jpg");          // open image
	imgOriginal = cv::imread(strPathOfObject);          // open image
	strPathOfObject
#endif


#if MANAGE_VIDEO

	cv::VideoCapture capVideo;
	
	capVideo.open(strPathOfObject);
	//capVideo.open("..\\pic\\CarsDrivingUnderBridge.mp4");
	//capVideo.open("C:\\_WORK_RGS\\MyFiles\\DISSER\\ISTRATOV\\videos\\ConsoleApplicationOpenCV\\pic\\CarsDrivingUnderBridge.mp4");

	if (!capVideo.isOpened()) {                                                 // if unable to open video file
		std::cout << "error reading video file" << std::endl << std::endl;      // show error message
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);                                                              // and exit program
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
		std::cout << "error: video file must have at least two frames";
		_getch();                   // it may be necessary to change or remove this line if not using Windows
		return(0);
	}

	capVideo.read(imgFrame1);
	capVideo.read(imgFrame2);
#endif



	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35);

	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;

	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;

	char chCheckForEscKey = 0;

	bool blnFirstFrame = true;

	int frameCount = 2;

#if MANAGE_VIDEO

	while (capVideo.isOpened() && chCheckForEscKey != 27)
	{

		std::vector<Blob> currentFrameBlobs;

		cv::Mat imgFrame1Copy = imgFrame1.clone();
		cv::Mat imgFrame2Copy = imgFrame2.clone();

		cv::Mat imgDifference;
		cv::Mat imgThresh;

		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

		cv::imshow("imgThresh", imgThresh);
#endif

#if MANAGE_PICTURE
		if (!imgOriginal.empty())
		{
			cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);
			cv::imshow("imgOriginal", imgOriginal); cv::waitKey(0);

			// !!! Here we Enter the DATA from the Picture
			imgFrame1 = imgOriginal.clone();
			imgFrame2 = imgOriginal.clone();

			std::vector<Blob> currentFrameBlobs;

			cv::Mat imgFrame1Copy = imgFrame1.clone();
			cv::Mat imgFrame2Copy = imgFrame2.clone();

			cv::Mat imgDifference;
			cv::Mat imgThresh;

			cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);
			cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);

			cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
			cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

			cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);

			cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);

			cv::namedWindow("imgThresh", CV_WINDOW_AUTOSIZE);
			cv::imshow("imgThresh", imgThresh); cv::waitKey(0);

#endif

			cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
			cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
			cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
			cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));

			for (unsigned int i = 0; i < 2; i++) {
				cv::dilate(imgThresh, imgThresh, structuringElement5x5);
				cv::dilate(imgThresh, imgThresh, structuringElement5x5);
				cv::erode(imgThresh, imgThresh, structuringElement5x5);
			}

			cv::Mat imgThreshCopy = imgThresh.clone();

			// http://stackoverflow.com/questions/30514037/opencv-with-visual-studio-2013c-findcontours-breakpoint-error
			std::vector<std::vector<cv::Point> > contours;
			//std::vector<cv::Mat> contours;

			cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			fun_drawAndShowContours(imgThresh.size(), contours, "imgContours");

			std::vector<std::vector<cv::Point> > convexHulls(contours.size());
			//std::vector<cv::Mat> convexHulls(contours.size());

			for (unsigned int i = 0; i < contours.size(); i++) {
				cv::convexHull(contours[i], convexHulls[i]);
			}

			fun_drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");

			for (auto &convexHull : convexHulls) {
				Blob possibleBlob(convexHull);

				if (possibleBlob.currentBoundingRect.area() > 400 &&
					possibleBlob.dblCurrentAspectRatio > 0.2 &&
					possibleBlob.dblCurrentAspectRatio < 4.0 &&
					possibleBlob.currentBoundingRect.width > 30 &&
					possibleBlob.currentBoundingRect.height > 30 &&
					possibleBlob.dblCurrentDiagonalSize > 60.0 &&
					(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
					currentFrameBlobs.push_back(possibleBlob);
				}
			}

			fun_drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

			if (blnFirstFrame == true) {
				for (auto &currentFrameBlob : currentFrameBlobs) {
					blobs.push_back(currentFrameBlob);
				}
			}
			else {
				fun_matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
			}

			fun_drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

			//*
			imgFrame2Copy = imgFrame2.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

			fun_drawBlobInfoOnImage(blobs, imgFrame2Copy);

			bool blnAtLeastOneBlobCrossedTheLine = fun_checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount);

			if (blnAtLeastOneBlobCrossedTheLine == true) {
				cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
			}
			else {
				cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
			}

			fun_drawCarCountOnImage(carCount, imgFrame2Copy);

#if MANAGE_PICTURE
			cv::namedWindow("imgFrame2Copy", CV_WINDOW_AUTOSIZE);
			cv::imshow("imgFrame2Copy", imgFrame2Copy); cv::waitKey(0);
#endif

#if MANAGE_VIDEO
			cv::imshow("imgFrame2Copy", imgFrame2Copy);
			//cv::waitKey(0);                 // UNCOMMENT !!! this line to go frame by frame for debugging


			// now we prepare for the next iteration
			currentFrameBlobs.clear();

			imgFrame1 = imgFrame2.clone();           // move frame 1 up to where frame 2 is

			if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {
				capVideo.read(imgFrame2);
			}
			else {
				std::cout << "end of video\n";
				break;
			}
#endif

			blnFirstFrame = false;
			frameCount++;
			chCheckForEscKey = cv::waitKey(1);
		}
#if MANAGE_PICTURE
		else
		{
			std::cout << "error: image not read from file\n\n";     // show error message on command line
			_getch();                                               // may have to modify this line if not using Windows
			//return(0);                                              // and exit program
		}
#endif

		if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
			cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
		}
		// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows

		//return(0);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			if (existingBlobs[i].blnStillBeingTracked == true) {

				double dblDistance = fun_distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
			fun_addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			fun_addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {

	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double fun_distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//void fun_drawAndShowContours(cv::Size imageSize, std::vector<cv::Mat> contours, std::string strImageName)
void fun_drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName)
{
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName)
{

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool fun_checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition) {
				carCount++;
				blnAtLeastOneBlobCrossedTheLine = true;
			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);

			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			int intFontThickness = (int)std::round(dblFontScale * 1.0);

			cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void fun_drawCarCountOnImage(int &carCount, cv::Mat &imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);

	cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);

	cv::Point ptTextBottomLeftPosition;

	ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);

	cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);

}
