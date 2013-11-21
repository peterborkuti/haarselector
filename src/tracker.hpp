/** @file tracker.hpp
 * @ Created on: Nov 19, 2013
 * @author Peter Borkuti
 */

#ifndef TRACKER_HPP_
#define TRACKER_HPP_

typedef std::vector<cv::Point> Points;
typedef std::vector<cv::Point2f> Points2f;

#include "selections.hpp"

/**
 * Tracking selected rectangles.
 *
 * input: 	original image, original selections, new image
 * output: selections, which are the tracked version of original selections
 *
 * when tracking a selections, the output selection position and sizes can be
 * different of the original selection.
 *
 *
 */
class Tracker {
public:
	bool trackSelections(cv::Mat imgOld, Selections sels, cv::Mat img,
			Selections &trackedSels);
private:
	cv::Mat greyOrigImg;
	cv::Mat greyNewImg;

	bool trackSelection(cv::Rect rect, cv::Rect &trackedRect);
	Points2f detectFeaturePoints(cv::Rect rect);
	bool trackPoints(Points2f pointsToTrack, Points2f &trackedPoints);
	Points2f filterPoints(Points2f pointsToFilter);
	cv::Rect computeSelection(cv::Rect originalRect, Points2f originalPoints,
			Points2f trackedPoints);
};

#endif /* TRACKER_HPP_ */
