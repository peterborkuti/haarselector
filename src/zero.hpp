/*
 * zero.hpp
 *
 *  Created on: Nov 6, 2013
 *      Author: Peter Borkuti
 */

#ifndef ZERO_HPP_
#define ZERO_HPP_

#include <vector>
#include <map>
#include <string>

typedef std::vector<std::string> VecString;
typedef std::map<std::string, VecString> MapSelections;
typedef std::pair<std::string, VecString> PairSVS;

std::string& operator<<(std::string &strm, VecString vs);
std::ostream& operator<<(std::ostream &strm, VecString vs);

#endif /* ZERO_HPP_ */
