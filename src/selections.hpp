/*
 * selections.hpp
 *
 *  Created on: Nov 5, 2013
 *      Author: Peter Borkuti
 */

#ifndef SELECTIONS_HPP_
#define SELECTIONS_HPP_

#include "zero.hpp"
#include "selection.hpp"

class Selections {
public:
	VecSelection selections;
	bool empty;
	uint idx;

	Selections();
	Selections(std::string type_, std::string subtype_, VecString recs);
	void clear();
	void add(Selection sel);
	void add(Selections sels);
	void incType();
	void incSubtype();
	void erase();
	bool setSelected(cv::Point p);
	cv::Rect getRect(uint i);
	Selection getSelection(uint i);
	uint size();
	MapSelections toString();
	friend std::ostream& operator<<(std::ostream &strm, Selections sel);

};

typedef std::vector<Selections> VecSelections;

#endif /* SELECTIONS_HPP_ */
