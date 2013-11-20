/*
 * filelist.hpp
 *
 *  Created on: Nov 6, 2013
 *      Author: Peter Borkuti
 */

#ifndef FILELIST_HPP_
#define FILELIST_HPP_

#include "opencv2/core/core.hpp"
#include <vector>
#include <string>
#include "selections.hpp"

#include "tracker.hpp"

class FileData {
public:
	Selections selections;
	bool background;
	uint fileNameIndex;
	FileData();
	FileData(uint idx);
	FileData(Selections sel, bool backgrd);
	FileData(Selections sel, bool backgrd, uint idx);
};

typedef std::pair<std::string, FileData> PairMapFileData;
typedef std::map<std::string, FileData> MapFileData;
typedef std::map<std::basic_string<char>, FileData>::iterator MapFileDataIt;

class FileList {
private:
	uint idx;
	std::string pattern;
	VecString files;
	MapFileData mapSelections;
	Tracker tracker;

	void setPointers();
	FileData getFileData(uint idx);
	FileData getFileData();

public:
	FileList(std::string filePattern);
	bool isFileDataForFile(std::string fname);
	void addSelections(std::string fileName, Selections sels);
	void setAsBackground();
	void setAsBackground(std::string fileName);
	void setActive(std::string fileName);
	void toggleBackground();
	void setSelections(Selections sels);
	void getSelections(Selections &sels);
	cv::Mat getImage();
	cv::Mat getActual(Selections &sels);
	cv::Mat getNext(Selections &sels, const uint pcs, cv::Mat imgOld);
	cv::Mat getPrev(Selections &sels, const uint pcs);
	std::string getTitle();
	friend std::ostream& operator<<(std::ostream &strm, FileList &fl);
	friend std::istream& operator>>(std::istream &strm, FileList &fl);

};

#endif /* FILELIST_HPP_ */
