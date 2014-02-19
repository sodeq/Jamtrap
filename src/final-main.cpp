/*
 * final-main.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lynx
 */

#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <math.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include "header-final.hpp"

using namespace std;
using namespace cv;

std::string frame_ = "frame_";
std::string ext = ".jpg";
std::string buffer;
std::string frameNumber;

void ErrorMsg() {
	printf("<program-name> <path-to-lookup> <path-to-video>\n");
}

const std::string pesan = "PESAN";

int main(int n, char** arg)
{
	if(n != 3) {
		ErrorMsg();
		cin.get();
		return -1;
	}

	cv::VideoCapture cap = cv::VideoCapture(arg[2]);
	cv::CascadeClassifier cascade;
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//cap.open(arg[1]);


	if(cap.isOpened()) {
		cv::Mat frm;
		double skala = 1.1;
		cv::namedWindow("FRAME", CV_WINDOW_AUTOSIZE);

		cascade.load(arg[1]);
		if(cascade.empty()) {
			printf("Empty cascade file\n");
			return -1;
		}

		double fps = cap.get(CV_CAP_PROP_FPS);
		double numOfFrame = static_cast<double>(cap.get(CV_CAP_PROP_FRAME_COUNT));
		int w = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
		int h = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
		printf("FPS: %f\nnumOfFrame: %f\n", fps, numOfFrame);
		int fC(0);
		cv::Mat imageToBeProcess;
		
		// Detected objects container
		std::vector<cv::Rect> mobil;
		cv::Mat hasil;

		// Logging
		LOG_FILE *benchmark_me = new LOG_FILE("log.zbench");
		benchmark_me->Log<int, int>(w,h);

		while(1) {
			
			cap >> frm;
			imageToBeProcess = frm.clone();
			
			if(frm.empty()) {
				printf("Reached end of video\n");
				break;
			}
			
			// perform benchmarking
			TIMER *detectionLog = new TIMER;		// delta t untuk pendeteksian object
			TIMER *drawingLog = new TIMER;			// delta t untuk menggambar! (displaying purposes)
			
			// perform detection, use output rectangles to draw object
			detectionLog->Start();
			mobil = DetectReturnRect(imageToBeProcess, skala, cascade);
			detectionLog->Stop();

			drawingLog->Start();
			hasil = DrawObjects(frm, mobil, skala);
			drawingLog->Stop();

			double processTime = static_cast<double>(detectionLog->GetTimeDelta());

			frameNumber = Number2Str<int>(fC);
			buffer = frame_;
			buffer.append(frameNumber).append(ext);
			
			// LOGGING
			benchmark_me->Log<std::string, double>(buffer, processTime);

			// SHOWTIME
			cv::imshow("FRAME", hasil);
			
			// SAVE
			//SaveMyImage(buffer, hasil, CV_IMWRITE_JPEG_QUALITY, 95);

			// CLEAN UP
			mobil.clear(); // clean up
			delete detectionLog, drawingLog;
			buffer.clear();
			
			//cap.set(CV_CAP_PROP_POS_FRAMES, fC*static_cast<int>(fps));
			fC++;
			
			if(cv::waitKey(30)>0)
				break;
		}
		benchmark_me->StopLog();
	}

	return n;
}



