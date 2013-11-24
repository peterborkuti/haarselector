/*
 * cli.cpp
 *
 *  Created on: Nov 23, 2013
 *      Author: Peter Borkuti
 */

#include "opencv2/core/core.hpp"
#include "cli.hpp"
#include <iostream>

CLI::CLI() {
	keys =
	{
		"{ c | cascade  | ./train/cascade.xml| trained cascade xml file for object detection}"
		"{ h | help     | false              | print this help}"
		"{ i | images   | ./pics/*.png       | glob expression for images}"
		"{ o | output   | pictures.txt       | output file name}"
		"{ t | tracking | false              | automatic tracking if there is no selection }"
		"{ v | version  | false              | version information}"
	};
}

bool CLI::parse(int argc, const char** argv, std::string &images,
		std::string cascade_file, bool &isTracking, std::string &output) {
	cv::CommandLineParser parser(argc, argv, keys);

	if (parser.get<bool>("help")) {
		parser.printParams();

		std::cout << std::endl;
		std::cout << "more info:https://github.com/peterborkuti/haarselector" <<
				std::endl;
		std::cout << "author: Péter Borkuti, 2013" << std::endl;

		return false;
	}

	if (parser.get<bool>("version")) {
		std::cout << cv::getBuildInformation() << std::endl;
		return false;
	}

	images = parser.get<std::string>("images");
	cascade_file = parser.get<std::string>("cascade");
	isTracking = parser.get<bool>("tracking");
	output = parser.get<std::string>("output");

	std::cout << "images:" << images << std::endl;
	std::cout << "cascade:" << cascade_file << std::endl;
	std::cout << "isTracking:" << isTracking << std::endl;
	std::cout << "output:" << output << std::endl;

	return true;

}

