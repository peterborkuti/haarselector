/*
 * cli.hpp
 *
 *  Created on: Nov 23, 2013
 *      Author: Peter Borkuti
 */

#ifndef CLI_HPP_
#define CLI_HPP_

class CLI {
private:
	const char* keys;
public:
	CLI();
	bool parse(int argc, const char** argv, std::string &images,
			std::string cascade_file, bool &isTracking, std::string &output);
};



#endif /* CLI_HPP_ */
