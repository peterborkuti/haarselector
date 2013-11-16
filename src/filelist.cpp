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
}

FileData::FileData(Selections sel, bool backgrd) {
	selections = sel;
	background = backgrd;
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
		FileData data;
		mapSelections.insert(PairMapFileData(files[i], data));
	}

}

void FileList::setSelections(Selections sels) {
	MapFileDataIt it = mapSelections.find(files[idx]);
	bool b = it->second.background;
	mapSelections.erase(it);
	mapSelections.insert(PairMapFileData(files[idx], FileData(sels, b)));
}

void FileList::addSelections(std::string fileName, Selections sels) {
	assert(mapSelections.count(fileName) > 0);

	MapFileDataIt it = mapSelections.find(fileName);
	bool b = it->second.background;
	Selections s = it->second.selections;
	std::cout << "0:" << sels;
	std::cout << "1:" << s;
	s.add(sels);
	std::cout << "2:" << s;
	mapSelections.erase(it);
	mapSelections.insert(PairMapFileData(fileName, FileData(s, b)));
}

void FileList::setAsBackground() {
	FileList::setAsBackground(files[idx]);
}

void FileList::toggleBackground() {
	MapFileDataIt it = mapSelections.find(files[idx]);
	it->second.background = !(it->second.background);
}

void FileList::setAsBackground(std::string fileName) {
	MapFileDataIt it = mapSelections.find(fileName);
	it->second.background = true;
}

std::string FileList::getTitle() {
	std::string s = files[idx];
	FileData d = FileList::getFileData(idx);
	char tmp[30];
	sprintf(tmp, " (%4d, %4d)", idx, this->files.size());
	s += tmp;
	if (d.background) {
		s += " background";
	}

	return s;
}

cv::Mat FileList::getNext(Selections &sels, const uint pcs) {
	cv::Mat img;
	if (files.size() > 0) {
		FileList::setSelections(sels);
		idx += pcs;
		if (idx >= files.size()) {
			idx = 0; //circular
		}
		img = cv::imread(files[idx]);
		FileData d = FileList::getFileData(idx);
		sels = d.selections;
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

	std::cout << "FileList<<";
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
	// read obj from stream
	//if( /* no valid object of T found in stream */ )
	//  file.setstate(std::ios::failbit);
	return file;
}

void FileList::load(std::string fileName) {
	std::ifstream file(fileName.c_str());

	while (file.good()) {
		std::string line;
		getline(file, line);

		if (line == "") {
			std::cout << "empty line" << std::endl;
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
		if (!this->isFileDataForFile(fname)) {
			std::cerr << fname << " - file does not exists, skip." << std::endl;
			continue;
		}

		std::stringstream type_subtype(words[0]);
		std::string type, subtype;

		std::getline(type_subtype, type, '_');

		if (type == "background") {
			this->setAsBackground(fname);
			continue;
		}

		std::getline(type_subtype, subtype, '_');

		VecString recs(words.begin() + 3, words.end());

		Selections sels(type, subtype, recs);

		this->addSelections(fname, sels);
	}
}

