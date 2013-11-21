/*
 * filelist.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: Peter Borkuti
 */
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <assert.h>

#include "filelist.hpp"

FileData::FileData() {
	background = false;
	selections.clear();
	fileNameIndex = 0;
}

FileData::FileData(uint i) {
	background = false;
	selections.clear();
	fileNameIndex = i;
}

FileData::FileData(Selections sel, bool backgrd) {
	selections = sel;
	background = backgrd;
	fileNameIndex = 0;
}

FileData::FileData(Selections sel, bool backgrd, uint idx) {
	selections = sel;
	background = backgrd;
	fileNameIndex = idx;
}

bool FileList::isFileDataForFile(std::string fname) {
	return (mapSelections.count(fname) > 0);
}

FileData FileList::getFileData(uint idx) {
	MapFileDataIt it = mapSelections.find(files[idx]);
	if (it == mapSelections.end()) {
		std::cerr << files[idx] << " not found in map" << std::endl;
		exit(EXIT_FAILURE);
	}
	return (it->second);
}

FileData FileList::getFileData() {
	return FileList::getFileData(idx);
}

FileList::FileList(std::string filePattern) {
	pattern = filePattern;
	cv::glob(pattern, files, true);

	if (files.size() > UINT_MAX) {
		std::cerr << "Too man files. Try to keep them below " <<
		UINT_MAX << std::endl;
	}

	if (files.size() == 0) {
		std::cerr << "There are no files like " << pattern << std::endl;
		exit(EXIT_FAILURE);
	}

	idx = 0;

	for (uint i = 0; i < files.size(); i++) {
		FileData data(i);
		mapSelections.insert(PairMapFileData(files[i], data));
	}

}

void FileList::setSelections(Selections sels) {
	MapFileDataIt it = mapSelections.find(files[idx]);
	bool b = it->second.background;
	mapSelections.erase(it);
	mapSelections.insert(PairMapFileData(files[idx], FileData(sels, b, idx)));
}

void FileList::addSelections(std::string fileName, Selections sels) {
	assert(mapSelections.count(fileName) > 0);

	MapFileDataIt it = mapSelections.find(fileName);
	bool b = it->second.background;
	uint i = it->second.fileNameIndex;
	Selections s = it->second.selections;
	s.add(sels);
	mapSelections.erase(it);
	mapSelections.insert(PairMapFileData(fileName, FileData(s, b, i)));
}

void FileList::setAsBackground() {
	FileList::setAsBackground(files[idx]);
}

void FileList::toggleBackground() {
	MapFileDataIt it = mapSelections.find(files[idx]);
	it->second.background = !(it->second.background);
}

void FileList::setActive(std::string fileName) {
	MapFileDataIt it = mapSelections.find(fileName);
	idx = it->second.fileNameIndex;
}

void FileList::setAsBackground(std::string fileName) {
	MapFileDataIt it = mapSelections.find(fileName);
	it->second.background = true;
}

std::string FileList::getTitle() {
	std::string s = files[idx];
	FileData d = FileList::getFileData(idx);
	char tmp[30];
	sprintf(tmp, " (%4d, %4ld)", idx, this->files.size());
	s += tmp;
	if (d.background) {
		s += " background";
	}

	return s;
}

cv::Mat FileList::getActual(Selections &sels) {
	cv::Mat tmp;

	return this->getNext(sels, 0, tmp);
}

cv::Mat FileList::getNext(Selections &sels, const uint pcs, cv::Mat imgOld) {
	cv::Mat img;
	uint idxOld = idx;
	bool isTrackingSuccess = false;
	Selections newSelections;

	if (files.size() > 0) {
		FileList::setSelections(sels);
		idx += pcs;
		if (idx >= files.size()) {
			idx = 0; //circular
		}
		img = cv::imread(files[idx]);
		FileData d = FileList::getFileData(idx);
		if ((idx == idxOld + 1) && !sels.empty && d.selections.empty) {
			isTrackingSuccess = tracker.trackSelections(imgOld, sels, img,
					newSelections);
		}
		if (!isTrackingSuccess || newSelections.empty) {
			sels = d.selections;
		} else {
			sels = newSelections;
		}
	}

	return img;
}

cv::Mat FileList::getPrev(Selections &sels, const uint pcs) {
	cv::Mat img;
	if (files.size() > 0) {
		FileList::setSelections(sels);
		idx -= pcs;
		if (idx >= files.size()) {
			idx = files.size() - 1; //circular
		}
		img = cv::imread(files[idx]);
		FileData d = FileList::getFileData(idx);
		sels = d.selections;
	}

	return img;
}

void FileList::getSelections(Selections &sels) {
	FileData d = FileList::getFileData(idx);
	sels = d.selections;
}

cv::Mat FileList::getImage() {
	cv::Mat img = cv::imread(files[idx]);
	return img;
}

std::ostream& operator<<(std::ostream &ss, FileList &fl) {

	for (uint i = 0; i < fl.files.size(); i++) {
		FileData d = fl.getFileData(i);

		if (d.background) {
			ss << "background " << fl.files[i] << std::endl;
		} else if (d.selections.size() > 0) {
			MapSelections m = d.selections.toString();

			for (MapSelections::iterator it = m.begin(); it != m.end(); ++it) {

				std::string key = it->first;
				VecString value = it->second;
				uint count = value.size();
				ss << key << " " << fl.files[i] << " " << count << " ";

				for (uint j = 0; j < value.size(); j++) {
					ss << value[j];
					if (j < value.size() - 1)
						ss << " ";
				}
				ss << std::endl;
			}
		}
	}

	return ss;
}

std::istream& operator>>(std::istream& file, FileList &fileList) {
	std::string lastSeenFileName;

	while (file.good()) {
		std::string line;
		getline(file, line);

		if (line == "") {
			std::cerr << "empty line" << std::endl;
			continue;
		}
		std::stringstream ss(line);
		std::string word;
		VecString words;
		while (std::getline(ss, word, ' ')) {
			words.push_back(word);
		}

		if (words.size() < 2) {
			std::cerr << words << " syntax error in file" << std::endl;
			continue;
		}

		std::string fname = words[1];
		std::cout << fname << std::endl;
		if (!fileList.isFileDataForFile(fname)) {
			std::cerr << fname << " - file does not exists, skip." << std::endl;
			continue;
		}

		lastSeenFileName = fname;
		std::stringstream type_subtype(words[0]);
		std::string type, subtype;

		std::getline(type_subtype, type, '_');

		if (type == "background") {
			fileList.setAsBackground(fname);
			continue;
		}

		std::getline(type_subtype, subtype, '_');

		VecString recs(words.begin() + 3, words.end());

		Selections sels(type, subtype, recs);

		fileList.addSelections(fname, sels);
	}

	fileList.setActive(lastSeenFileName);

	// read obj from stream
	//if( /* no valid object of T found in stream */ )
	//  file.setstate(std::ios::failbit);
	return file;
}

