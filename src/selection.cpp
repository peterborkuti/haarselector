/*
 * selection.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: Peter Borkuti
 */

#include "selection.hpp"
#include <stdio.h>
#include <algorithm>

const int TYPES_NUM = 4;
const std::string TYPES[TYPES_NUM] = { "CAR", "MOTOR", "VAN", "OTHER" };
const int SUBTYPES_NUM = 2;
const std::string SUBTYPES[SUBTYPES_NUM] = { "FRONT", "BACK" };
enum {
	CAR = 0, MOTOR = 1, VAN = 2, OTHER = 3
};

enum {
	FRONT = 0, BACK = 1
};

Selection::Selection() {
	rect = cv::Rect();
	type = CAR;
	subtype = FRONT;
}

Selection::Selection(cv::Rect r) {
	rect = r;
	type = CAR;
	subtype = FRONT;
}

Selection::Selection(std::string stype, std::string ssubtype,
		std::vector<std::string> rectangle) {
	type = Selection::getValue(stype, TYPES);
	subtype = Selection::getValue(ssubtype, SUBTYPES);
	rect = cv::Rect(atoi(rectangle[0].c_str()), atoi(rectangle[1].c_str()),
			atoi(rectangle[2].c_str()), atoi(rectangle[3].c_str()));
}

int Selection::getValue(const std::string s, const std::string arr[]) {
	int ret = -1;
	for (uint i = 0; i < arr->size(); i++) {
		if (arr[i] == s) {
			ret = i;
			break;
		}
	}

	return ret;
}

std::string Selection::toString() {
	char tmp[30];
	sprintf(tmp, "%d %d %d %d", rect.x, rect.y, rect.width, rect.height);
	return std::string(tmp);
}

std::string Selection::toString(int type_, int subtype_) {
	char tmp[30];
	if (type == type_ && subtype == subtype_) {
		sprintf(tmp, "%d %d %d %d", rect.x, rect.y, rect.width, rect.height);
		return std::string(tmp);
	}

	return std::string("");
}

void Selection::incType() {
	type++;
	if (type >= TYPES_NUM) {
		type = 0;
	}
}

void Selection::incSubtype() {
	subtype++;
	if (subtype >= SUBTYPES_NUM) {
		subtype = 0;
	}
}

std::string Selection::getKey() {
	return Selection::getKey("_");
}

std::string Selection::getKey(std::string sep) {
	return TYPES[type] + sep + SUBTYPES[subtype];
}

std::ostream& operator<<(std::ostream &strm, Selection sel) {
	strm << sel.getKey(",") << " (" << sel.rect.x << "," << sel.rect.y << ","
			<< sel.rect.width << "," << sel.rect.height << ")" << std::endl;

	return strm;
}

