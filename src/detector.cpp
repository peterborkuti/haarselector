/*
 * detector.cpp
 *
 *  Created on: Nov 22, 2013
 *      Author: peter
 */

#include "opencv2/core/core.hpp"
#include "opencv2/video/tracking.hpp"
#include "detector.hpp"
#include <iostream>

bool Detector::load(std::string cascade_file) {
	_detect = false;
	if (!cascade_file.empty()) {
		_detect = cascade.load(cascade_file);
	}
	return _detect;
}

Detector::Detector() {
	_detect = false;
}

bool Detector::isDetect() {
	return _detect;
}
Rectangles Detector::detect(cv::Mat img) {
	Rectangles objects;
	cv::Mat imgGray;

	if (_detect) {
		std::cout << "start detection";
		cv::cvtColor(img, imgGray, CV_BGR2GRAY);
		equalizeHist(imgGray, imgGray);

		//-- Detect faces
		cascade.detectMultiScale(imgGray, objects, 1.1, 2,
				0 | CV_HAAR_SCALE_IMAGE, cv::Size(20, 20));
		std::cout << " - detected objects:" << objects.size() << std::endl;
	}

	return objects;
}
