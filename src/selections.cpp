/*
 * selections.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: Peter Borkuti
 */

#include "selections.hpp"
#include <stdio.h>
#include <iostream>
#include "zero.hpp"

std::ostream& operator<<(std::ostream &strm, VecSelection sels) {
	strm << "Selections : [" << std::endl;
	for (uint i = 0; i < sels.size(); i++) {
		strm << i << "." << sels[i];
	}
	strm << "]" << std::endl;

	return strm;
}

std::ostream& operator<<(std::ostream &strm, Selections sel) {
	strm << sel.selections;
	return strm;
}

Selections::Selections() {
	empty = true;
	idx = 0;
}

Selections::Selections(std::string type_, std::string subtype_,
		VecString recs) {
	idx = 0;
	for (uint i = 0; (i + 3) < recs.size(); i += 4) {
		VecString rec(recs.begin() + i, recs.begin() + i + 4);
		selections.push_back(Selection(type_, subtype_, rec));
	}
	empty = selections.size() == 0;
}

void Selections::clear() {
	selections.clear();
}

void Selections::add(Selections sels) {
	for (uint i = 0; i < sels.selections.size(); i++) {
		Selections::add(sels.selections[i]);
	}
	empty = selections.size() == 0;
}

void Selections::add(Selection sel) {
	selections.push_back(sel);
	idx = selections.size() - 1;
	empty = false;
#if LOG_STORE
	printf("selection stored\n");
#endif
}
void Selections::incType() {
	if (!empty) {
		selections[idx].incType();
	}
}
void Selections::incSubtype() {
	if (!empty) {
		selections[idx].incSubtype();
	}
}
void Selections::erase() {
	if (!empty) {
		VecSelection::iterator it = selections.begin();
		std::advance(it, idx);
		selections.erase(it);
#if LOG_STORE
		printf("deleted selection %d\n", idx);
#endif
		if (selections.size() > 0) {
			idx = selections.size() - 1;
		} else {
			empty = true;
			idx = 0;
		}
	}
}
bool Selections::setSelected(cv::Point p) {
	//search for the smallest selection which contains the mouse pointer
	//that selection will be editable
	bool isSelectedChanged = false;
	int minArea = INT_MAX;
	uint minIdx = UINT_MAX;
#if LOG_SELECTION
	printf("Numb of selections: %d\n", (int)selections.size());
#endif
	for (uint i = 0; i < selections.size(); i++) {
		Selection s = selections[i];
		if (s.rect.contains(p)) {
			int area = s.rect.area();
			if (area < minArea) {
				minArea = area;
				minIdx = i;
			}
		}
	}
	if (minIdx != UINT_MAX && idx != minIdx) {
		idx = minIdx;
		isSelectedChanged = true;
#if LOG_SELECTION
		printf("selected selection: %d\n", idx);
#endif
	}

	return isSelectedChanged;
}
cv::Rect Selections::getRect(uint i) {
	if (!empty && i < selections.size()) {
		Selection s = selections[i];
		return s.rect;
	}
	return cv::Rect();
}

Selection Selections::getSelection(uint i) {
	if (!empty && i < selections.size()) {
		return selections[i];
	}
	return Selection();
}
uint Selections::size() {
	return selections.size();
}

MapSelections Selections::toString() {
	MapSelections m;
	if (selections.size() == 0) {
		VecString v;
		m.insert(PairSVS("background", v));
		return m;
	}
	for (uint i = 0; i < selections.size(); i++) {
		Selection s = selections[i];
		std::string key = s.getKey();
		std::string value = s.toString();
		if (m.count(key) == 0) {
			VecString v;
			v.push_back(value);
			m.insert(PairSVS(key, v));
		} else {
			m.find(key)->second.push_back(value);
		}
	}

	return m;
}

