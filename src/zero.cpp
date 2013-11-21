/*
 * zero.cpp
 *
 *  Created on: Nov 15, 2013
 *      Author: Peter Borkuti
 */
#include "zero.hpp"
#include <ostream>
#include <sstream>

std::string& operator<<(std::string &strm, VecString vs) {
	strm += "[";
	for (unsigned int i = 0; i < vs.size(); i++) {
		strm += vs[i];
		if (i < vs.size() - 1) {
			strm += ",";
		}
	}
	strm += "]";

	return strm;
}

std::ostream& operator<<(std::ostream &strm, VecString vs) {
	std::string s;
	s << vs;
	strm << s << std::endl;
	return strm;
}

