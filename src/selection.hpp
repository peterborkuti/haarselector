/*
 * selection.hpp
 *
 *  Created on: Nov 5, 2013
 *      Author: Peter Borkuti
 */

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "opencv2/core/core.hpp"
#include <ostream>

class Selection {
public:
	int type;
	int subtype;
	cv::Rect rect;
	Selection();
	Selection(cv::Rect r);
	Selection(int _type, int _subtype, cv::Rect r);
	Selection(std::string stype, std::string ssubtype,
			std::vector<std::string> rect);
	int getValue(const std::string s, const std::string arr[]);
	std::string toString();
	std::string toString(int type_, int subtype_);
	void incType();
	void incSubtype();
	std::string getKey();
	std::string getKey(std::string separator);
	friend std::ostream& operator<<(std::ostream &strm, Selection sel);
};

typedef std::vector<Selection> VecSelection;

#endif /* SELECTION_HPP_ */
