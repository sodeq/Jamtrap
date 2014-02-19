/*
 * header-final.hpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lynx
 */

#ifndef HEADER_FINAL_HPP_
#define HEADER_FINAL_HPP_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>	// getTickCount, getTickFrequency

#include <string>
#include <fstream>
#include <time.h>					// benchmarking in C

#define __PER_CHANNEL__		1
#define __TREAT_AS_SINGLE__	0

#define _PAUSE_PLAY_ 32 // SPACE
#define _CLOSE_VIDEO_ 27 // ESCAPE


// bounding box color
const static cv::Scalar colors[] =
		  {
			CV_RGB(255, 255, 0),	// 1
			CV_RGB(0, 128, 255),	// 2
			CV_RGB(0, 255, 255),	// 3
			CV_RGB(0, 255, 0),		// 4
			CV_RGB(255, 128, 0),	// 5
			CV_RGB(255, 255, 0),	// 6
			CV_RGB(255, 0, 0),		// 7
			CV_RGB(255, 0, 255)		// 8
		  };


std::string dataset_test;
std::string trainXML;

using namespace std;
using namespace cv;

typedef struct {
	double tConvert;
	double tDetect;
	double tHistNorm;
}T_Benchmarking__;

/* Implementation of human visual system corresponding RGB spectrum */
cv::Mat BGR2GRAY_human(cv::Mat M) {
	// Safety first
	if (M.channels() == 1) {
		return cv::Mat();
	}
	
	cv::Mat O(M.rows, M.cols, CV_8UC1, cv::Scalar::all(0));
	float B,G,R, buf;

	// access per pixel, assumed M has 3 channels, opencv use BGR order
	for(int i=0; i < M.rows; i++) {  // per rows
		for(int j = 0; j < M.cols; j++) {  // per columns
			B = static_cast<float>(M.data[M.step[0]*i + M.step[1]*j + 0]);
			G = static_cast<float>(M.data[M.step[0]*i + M.step[1]*j + 1]);
			R = static_cast<float>(M.data[M.step[0]*i + M.step[1]*j + 2]);
			buf = 0.21f*(R) + 0.71f*(G) + 0.08f*(B);
			O.at<uchar>(i,j) = static_cast<uchar>(buf);
		}
	}
	return O;
}

/* Using OpenCV Timer Class */
class TIMER
{
public:
	// Default constructor, empty arg
	TIMER() {
		t_awal  = (0.0);
		t_akhir = (0.);
	}

	// Start timer
	void Start() {
		t_awal = (double)cv::getTickCount();
	}

	// Stop timers
	void Stop() {
		t_akhir = ((double)cv::getTickCount()-t_awal)/cv::getTickFrequency();
	}

	// get process time
	double GetTimeDelta() const {
		return t_akhir;
	}
	~TIMER() { }

private:
	double t_awal, t_akhir;

};

/* Pure C Timer Class */
class TIMER_C {
public:
	TIMER_C () {}
	~TIMER_C () {}

	void Start() {
		start = clock();
	}
	void Stop() {
		finish = clock();
		delta = (double)(finish - start)/(double)CLOCKS_PER_SEC;
	}
	double GetTimeDelta() const {
		return delta;
	}

private:
	clock_t start, finish;
	double delta;
};


/* LOG_FILE class */
class LOG_FILE
{
public:
	// constructor
	LOG_FILE(std::string filename) {
		fName = filename;

		tDelta = 0.0;
		logFile.open(fName.c_str(), ios::app );	// append
//		logFile.close();
	}

	// Start logging
	template<class _Tp1, class _Tp2>	// maybe we need this, eg: with specified 
	void Log(_Tp1 msg1, _Tp2 msg2)	{
		logFile << msg1 << "\t" << msg2 << endl;
	}

	// insert msg 
	void AddMsg(const std::string msg)	{
		if(!logFile.is_open())
			return;
		logFile << "-------------------------------------------------------------------" << endl;
		logFile << msg << endl;
		logFile << "-------------------------------------------------------------------" << endl;
	}

	// LOGGING finished!!!
	void StopLog() {
		if(logFile.is_open())
			logFile.close();
		logFile.close();
	}
	// destructor
	~LOG_FILE() {}

protected:
private:
	std::string fName;
	double tDelta;
	std::ofstream logFile;

};

static void ReadListFilenames( const string& filename/*, string& dirName*/, vector<string>& trainFilenames )
{
    trainFilenames.clear();

    ifstream file( filename.c_str() );
    if ( !file.is_open() )
        return;
    while( !file.eof() )
    {
        string str; getline( file, str );
        if( str.empty() ) break;
        trainFilenames.push_back(str);
    }
    file.close();
}

/* PreProcessingStage
 * 1.	Equalize histogram
 * 2.	Smooth image
 * 		Options:	__PER_CHANNEL__ 	<-- per-channel execution -->
 * 					__TREAT_AS_SINGLE__	<-- all channel combined then executed -->
 * 		OutputArray is single channel image, as needed by Detect()!
 */
void PreProcessingStage(cv::Mat& inputArray, cv::Mat& outputArray, int option)
{
	int nChannel = inputArray.channels(); // num of channel
	if(nChannel == 1) // single channel image, no need to split!
	{
		cv::equalizeHist(inputArray, outputArray);
		cv::GaussianBlur(outputArray, outputArray, cv::Size(3,3), 1.5, 0., cv::BORDER_CONSTANT);
	} else // multi channel image!
	{
		if(option == __PER_CHANNEL__)
		{
			cv::Mat buf;
			std::vector<cv::Mat> splitBuf;
			std::vector<cv::Mat>::iterator it;
			cv::split(inputArray, splitBuf);	// citra input di split dalam komponen satu warna RGB

			/*
				Dilakukan operasi equalisasi histogram pada tiap channel
			*/
			for(it = splitBuf.begin(); it != splitBuf.end(); ++it)
			{
				cv::equalizeHist(*it, *it);
				cv::GaussianBlur(*it, *it, cv::Size(3,3), 1.5, 0.0, cv::BORDER_CONSTANT);
			}
			cv::merge(splitBuf, outputArray);
			cv::cvtColor(outputArray, outputArray, CV_BGR2GRAY);
		} else if(option == __TREAT_AS_SINGLE__)
		{
			cv::cvtColor(inputArray, inputArray, CV_BGR2GRAY);
			cv::equalizeHist(inputArray, inputArray);
			cv::GaussianBlur(inputArray, outputArray, cv::Size(3,3), 1.5, 0., cv::BORDER_CONSTANT);
		}
	}
}

/* Detection Step */
void Detect(cv::Mat& inFrame, double& scale, cv::CascadeClassifier& cascade, bool drawBoundingBox)
{
	int i = 0;
	double t = 0.0;
	vector<cv::Rect> obj; // the object (cars in this case) bounding box!

	cv::Mat grayFrame, smallFrame(cvRound(inFrame.rows/scale), cvRound(inFrame.cols/scale), CV_8UC1);
	
	if(inFrame.channels() > 1)
	{
		cvtColor(inFrame, grayFrame, CV_BGR2GRAY);
	}

	// implicit processing stage
	cv::Mat *hist = new cv::Mat;
	cv::equalizeHist(grayFrame, grayFrame);


	cv::resize(
			grayFrame, // input array
			smallFrame, // output array destination
			smallFrame.size(), // array size
			0,
			0,
			cv::INTER_LINEAR);
	cv::equalizeHist(smallFrame, smallFrame);

	cascade.detectMultiScale
			(
			smallFrame, 					// source frame
			obj,							// OBJECT container! This variable must store as many object exist inside source frame
			1.1,							// scale factor from data training!
			2,								// minimum neighbours!, at least 2 object exist!
			0								// flags??
//			 | CV_HAAR_FIND_BIGGEST_OBJECT,
//			 | CV_HAAR_DO_ROUGH_SEARCH,
			| CV_HAAR_SCALE_IMAGE,			// pyramid scaling
			cv::Size(20, 20),				// minimum object size!
			cv::Size(smallFrame.size())
			);

	// find object vector and draw bounding rect!
	// this drawing job is time consuming too...
	if(drawBoundingBox == true)
	{
		for(vector<cv::Rect>::const_iterator r = obj.begin(); r != obj.end(); ++r, i++)
		{
			cv::Mat smallFrameROI;
			vector<cv::Rect> nestedObj;
			cv::Point center; // center point!
			cv::Scalar warna = colors[0];
	//		cv::Scalar warna(255, 0, 0); // BLUE
			int radius;

			center.x = cvRound((r->x +
								r->width*0.5)*scale);
			center.y = cvRound((r->y +
								r->height*0.5)*scale);
			// play safely, no warning
			radius = static_cast<int>(cvRound(r->width + r->height)*0.25*scale);

			cv::rectangle
			(
				inFrame,
				cv::Point(cvRound(r->x*scale), cvRound(r->y*scale)),
				cv::Point(cvRound((r->x+r->width-1)*scale), cvRound((r->y+r->height-1)*scale)),
				warna,
				2,
				8,
				0
			);
		}
	}
}

/* Detect and return a set of Rect() */
std::vector<cv::Rect> DetectReturnRect(cv::Mat& inFrame, double &scale, cv::CascadeClassifier &cascade) {
	int i = 0;
	double t = 0.0;
	vector<cv::Rect> obj; // the object (cars in this case) bounding box!

	cv::Mat grayFrame, smallFrame(cvRound(inFrame.rows/scale), cvRound(inFrame.cols/scale), CV_8UC1);
	
	if(inFrame.channels() > 1)
	{
		/*
		* implement human approximation instead of built in opencv rgb to single
		* channel image conversion, some said this way we could achieve faster processing
		*/
		grayFrame = BGR2GRAY_human(inFrame);
	}

	// implicit processing stage
	cv::Mat *hist = new cv::Mat;
	cv::equalizeHist(grayFrame, grayFrame);

	
	cv::resize(
				grayFrame, // input array
				smallFrame, // output array destination
				smallFrame.size(), // array size
				0,
				0,
				cv::INTER_LINEAR);		// use INTER LINEAR scaling!
	
	cascade.detectMultiScale
			(
			smallFrame, 					// source frame
			obj,							// OBJECT container! This variable must store as many object exist inside source frame
			1.1,							// scale factor from data training!
			2,								// minimum neighbours!, at least 2 object exist!
			0								// find all object
			| CV_HAAR_SCALE_IMAGE,			// pyramid scaling
			cv::Size(20, 20),				// minimum object size!
			cv::Size(smallFrame.size())
			);

	return obj;
}

/* Detect and return a set of Rect() , using TIMER class to benchmark processes */
std::vector<cv::Rect> DetectReturnRect(cv::Mat& inFrame, double &scale, cv::CascadeClassifier &cascade, T_Benchmarking__& bench) {
	int i = 0;
	double t = 0.0;
	vector<cv::Rect> obj; // the object (cars in this case) bounding box!
	TIMER konversi, normHist, detection;
	cv::Mat grayFrame, smallFrame(cvRound(inFrame.rows/scale), cvRound(inFrame.cols/scale), CV_8UC1);
	
	if(inFrame.channels() > 1)
	{
		/*
		* implement human approximation instead of built in opencv rgb to single
		* channel image conversion, some said this way we could achieve a faster processing
		*/
		konversi.Start();
		grayFrame = BGR2GRAY_human(inFrame);
		konversi.Stop();
	}

	// implicit processing stage
	cv::Mat *hist = new cv::Mat;
	normHist.Start();
	cv::equalizeHist(grayFrame, grayFrame);
	normHist.Stop();

	cv::resize(
				grayFrame, // input array
				smallFrame, // output array destination
				smallFrame.size(), // array size
				0,
				0,
				cv::INTER_LINEAR);		// use INTER LINEAR scaling!
	
	detection.Start();
	cascade.detectMultiScale
			(
			smallFrame, 					// source frame
			obj,							// OBJECT container! This variable must store as many object exist inside source frame
			1.1,							// scale factor from data training!
			2,								// minimum neighbours!, at least 2 object exist!
			0								// find all object
			| CV_HAAR_SCALE_IMAGE,			// pyramid scaling
			cv::Size(20, 20),				// minimum object size!
			cv::Size(smallFrame.size())
			);
	detection.Stop();

	bench.tConvert = konversi.GetTimeDelta();
	bench.tDetect = detection.GetTimeDelta();
	bench.tHistNorm = normHist.GetTimeDelta();

	return obj;
}

cv::Mat DrawObjects(cv::Mat inputImage, std::vector<cv::Rect> &objects, double &scale) {
	int i = 0;
	cv::Mat outputImage;
	outputImage = inputImage.clone();
	for(vector<cv::Rect>::const_iterator r = objects.begin(); r != objects.end(); ++r, i++)
	{
		cv::Point center; // center point!
		cv::Scalar warna = colors[0];
		
		int radius;

		center.x = cvRound((r->x +
			r->width*0.5)*scale);
		center.y = cvRound((r->y +
			r->height*0.5)*scale);
		// play safely, no warning
		radius = static_cast<int>(cvRound(r->width + r->height)*0.25*scale);

		cv::rectangle
			(
			outputImage,									// canvas where the rects will be drawn onto
			cv::Point(cvRound(r->x*scale),					// top-left point, x
					  cvRound(r->y*scale)),					// top-left point, y
			cv::Point(cvRound((r->x+r->width-1)*scale),		// bottom-right point, x
					  cvRound((r->y+r->height-1)*scale)),	// bottom-right point, y
			warna,											// obvious
			2,												// border thickness
			8,												// line style
			0);
	}

	return outputImage;
}


/* Save image with compression parameters */
void SaveMyImage(const string fname, cv::Mat array, int __COMPRESSION__ = CV_IMWRITE_JPEG_QUALITY, int value = 95)
{
	std::vector<int> comp;
	comp.push_back(__COMPRESSION__);
	comp.push_back(value);

	cv::imwrite(fname, array, comp);
}


/* Convert Numeric into String */
template <typename T>
std::string Number2Str(T number) {
	std::ostringstream oss;
	oss << number;
	return oss.str();
}

/* Get Video Information */
void GetVideoInfo(cv::VideoCapture &cap) {
	int w = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_WIDTH));
	int h = static_cast<int>(cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	double fps = static_cast<double>(cap.get(CV_CAP_PROP_FPS));

	printf("Width: %i\nHeight: %i\nFPS: %f\n", w, h, fps);
}

#endif /* HEADER_FINAL_HPP_ */
