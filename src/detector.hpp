/*
 * detector.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: Peter Borkuti
 */

#ifndef DETECTOR_HPP_
#define DETECTOR_HPP_

#include <vector>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

typedef std::vector<cv::Rect> Rectangles;

class Detector {
private:
	cv::CascadeClassifier cascade;
	bool _detect;
public:
	Detector();
	bool load(std::string cascade_file);
	Rectangles detect(cv::Mat img);
	bool isDetect();
};

#endif /* DETECTOR_HPP_ */
