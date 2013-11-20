/** @file tracker.cpp
 * @author Peter Borkuti
 */

/**
 * Tracking selected rectangrles.
 *
 * input: 	original image, original selections, new image
 * output: selections, which are the tracked version of original selections
 *
 * when tracking a selections, the output selection position and sizes can be
 * different of the original selection.
 *
 *
 */

#include <iostream>
#include <cmath>
#include <opencv2/video/tracking.hpp>
#include "tracker.hpp"

/**
 * Tracking selected rectangles.
 *
 */
bool Tracker::trackSelections(cv::Mat imgOrig, Selections sels,
		cv::Mat img, Selections &trackedSels) {
	Selections newSels;
	bool trackingSuccess = false;

	cv::cvtColor(imgOrig, greyOrigImg, CV_BGR2GRAY);
	cv::cvtColor(img, greyNewImg, CV_BGR2GRAY);

	for (uint i = 0; i < sels.size(); i++) {
		std::cout << "selection:" << sels.selections[i] << std::endl;
		cv::Rect trackedRect;
		bool success = this->trackSelection(sels.selections[i].rect, trackedRect);
		if (success) {
			newSels.add(Selection(trackedRect));
		}
	}

	if (newSels.size() > 0) {
		trackedSels = newSels;
		trackingSuccess = true;
	}

	return trackingSuccess;
}

/**
 * Tracking one selection
 *
 * It returns with a trecked selection
 *
 * @param sel [in] selection
 */
bool Tracker::trackSelection(cv::Rect rect, cv::Rect &trackedRect) {
	cv::Rect newSel;
	bool isSuccess = false;

	Points2f features = this->detectFeaturePoints(rect);
	Points2f filtered = this->filterPoints(features);

	Points2f trackedPoints;
	isSuccess = this->trackPoints(filtered, trackedPoints);

	if (isSuccess) {
		trackedRect = this->computeSelection(rect, filtered, trackedPoints);
	}

	return isSuccess;
}


Points2f Tracker::detectFeaturePoints(cv::Rect rect) {
	Points2f features;
	cv::Mat greyOrigROI = greyOrigImg(rect);
	double diag = sqrt(rect.x * rect.y);
	uint numOfMaxFeatures = 4;
	double minDistance = diag;
	features.clear();

	while ((minDistance > 1) && (features.size() < numOfMaxFeatures)) {
		cv::goodFeaturesToTrack(greyOrigROI, features, numOfMaxFeatures, 0.01,
					minDistance);
		if (features.size() < numOfMaxFeatures) {
			features.clear();
		}
		minDistance /= 2;
	}

	std::cout << "features.size:" << features.size() << std::endl;
	for (uint i = 0; i < features.size(); i++) {
		std::cout << i << ". " << features[i].x << "," << features[i].y << std::endl;
		features[i] += cv::Point2f((float)rect.x, (float)rect.y);
		std::cout << i << ". " << features[i].x << "," << features[i].y << std::endl;
	}
	return features;
}

bool Tracker::trackPoints(Points2f pointsToTrack, Points2f &trackedPoints) {

    Points2f cornersB;
    Points2f retPoints;
    std::vector<uchar> status;
    std::vector<float> error;

	bool isTracked = true;

	std::cout << "calcOpticalFlowPyrLK:" << pointsToTrack.size()<< std::endl;
	cv::calcOpticalFlowPyrLK(
		greyOrigImg, greyNewImg, // 2 consecutive images
		pointsToTrack, // input point positions in first image
		cornersB, // output point positions in the 2nd image
		status, // tracking success
		error); // tracking error

	for (uint i = 0; i < status.size(); i++){
		if (!status[i]) {
			isTracked = false;
			break;
		}
	}

	if (isTracked) {
		trackedPoints = cornersB;
	}

	return isTracked;
}

Points2f Tracker::filterPoints(Points2f pointsToFilter) {
	Points2f points = pointsToFilter;

	return points;
}

/**
 * Counts back from bounding box to selection rectangle.
 *
 * OpenCV tracks points, not rectangles. So after I
 * tracking feature points in the original selection,
 * I have to create a new selection (a rectangle) based on
 * the original selections and the tracked points.
 *
 * Usually the objects on images
 * a, moving, which is a translation
 * b, come to closer/go to farther, which is a resizing
 */
cv::Rect Tracker::computeSelection(cv::Rect originalRect,
		Points2f originalPoints, Points2f trackedPoints) {

		cv::Rect origbRect = cv::boundingRect(originalPoints);

		cv::Rect newbRect = cv::boundingRect(trackedPoints);
		float resizeX = (float)newbRect.width / (float)origbRect.width;
		float resizeY = (float)newbRect.height / (float)origbRect.height;
		float dx = (float)newbRect.x - origbRect.x;
		float dy = (float)newbRect.y - origbRect.y;


		float newX = (float)originalRect.x + dx;
		float newY = (float)originalRect.y + dy;

		float newW = ((float)originalRect.width) * resizeX;
		float newH = ((float)originalRect.height) * resizeY;

		cv::Rect newRect((int)newX, (int)newY, (int)newW, (int)newH);

		return newRect;
}
