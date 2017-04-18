
#include "opencv_webcam.h"

void getVideoStreamFromWebCam()
{
	VideoCapture vcap(0);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		//return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	while(true) //for (;;)
	{
		Mat frame;
		vcap >> frame;
		video.write(frame);
		imshow("Frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	int u = 0;
}


void takeOnePictureFromWebCam(std::string _fileNameToSaveFrame)
{
	// Capture the Image from the webcam
	VideoCapture vcap(0);

	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		//return -1;
	}
	//int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	//int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);

	if (vcap.get(CV_CAP_PROP_FRAME_COUNT) < 1) {
		std::cout << "error: stream from camera must have at least 1 frame";
		//return -1;
	}

	Mat save_img;
	
	// 1 Solution - Get the frame
	vcap >> save_img;
	// 2 Solution - Get the frame
	//vcap.read(save_img);
	

	if (save_img.empty())
	{
		std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
	}
	// Save the frame into a file
	//imwrite("test.jpg", save_img); // A JPG FILE IS BEING SAVED
	imwrite(_fileNameToSaveFrame, save_img); // A JPG FILE IS BEING SAVED
	

/*
	import cv2
		cap = cv2.VideoCapture(0)
		count = 0

		while True:
	# Capture frame - by - frame
		ret, frame = cap.read()

		# Our operations on the frame come here
		gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
		//		cv2.imwrite("frame%d.jpg" % ret, frame)     # save frame as JPEG file
		//		count += 1
		name = "frame%d.jpg"%count;
	    cv2.imwrite(name, frame);//     # save frame as JPEG file
*/

}

