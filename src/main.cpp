/*
 * main.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: Peter Borkuti
 */

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <ctime>
#include <stdio.h>

#ifdef DEBUG
#  define DEBUG(x) do { std::cerr << #x << ":" << x << std::endl; } while (0)
#else
#  define DEBUG(x) do {} while (0)
#endif

using namespace cv;

int const MIN_SELECTABLE_AREA = 100;

Scalar const STORED = Scalar(0, 0, 255);
Scalar const SELECTION = Scalar(255, 0, 0);
Scalar const EDIT = Scalar(0, 255, 0);
Scalar const DETECTED = Scalar(255, 255, 0);

#include "selection.hpp"
#include "selections.hpp"
#include "filelist.hpp"
#include "cli.hpp"

struct MouseEvent {
	MouseEvent() {
		event = -1;
		buttonState = 0;
		newEvent = false;
		selectionStart = false;
		selectionDone = false;
		rightButton = false;
		name = "Default";
		maxRect = cv::Rect(0, 0, 0, 0);
	}

	MouseEvent(String n, int width, int height) {
		event = -1;
		buttonState = 0;
		newEvent = false;
		selectionStart = false;
		selectionDone = false;
		rightButton = false;
		name = n;
		maxRect = Rect(0, 0, width, height);
	}

	String name;
	Point pt;
	int event;
	int buttonState;
	bool newEvent;
	Point pt0;
	Rect maxRect;
	Rect rect;
	bool selectionDone;
	bool selectionStart;
	bool rightButton;
	void clearSelection() {
		selectionStart = false;
		selectionDone = false;
	}
	void activate() {
		selectionDone = true;
		newEvent = true;
	}
	std::ostream& operator<<(std::ostream &ss) {
		ss << name << " - event: " << event << " buttonState: " << buttonState
				<< ", selectionDone:" << selectionDone << " x:" << pt.x << "y:"
				<< pt.y << std::endl;
		if (selectionDone) {
			ss << "selected region: (" << rect.x << "," << rect.y << ") ("
					<< rect.width << "," << rect.height << ")" << std::endl;
		}

		return ss;
	}
};

static void onMouse(int event, int x, int y, int flags, void* userdata) {
	MouseEvent* data = (MouseEvent*) userdata;
	if ((data->maxRect.width > 0) && !data->maxRect.contains(Point(x, y))) {
		data->clearSelection();
		return;
	}
	data->event = event;
	data->pt = Point(x, y);
	data->buttonState = flags;
	data->newEvent = true;
	if (event == EVENT_RBUTTONDOWN) {
		data->rightButton = true;
	}
	if (event == EVENT_LBUTTONDOWN) {
		data->pt0 = data->pt;
		data->selectionDone = false;
		data->selectionStart = true;

	}
	if (data->selectionStart) {
		data->rect = Rect(data->pt0, data->pt);
	}

	if (event == EVENT_LBUTTONUP) {
		data->selectionStart = false;
		data->rect = Rect(data->pt0, data->pt);
		if (data->rect.area() > MIN_SELECTABLE_AREA) {
			data->selectionDone = true;
		}
	}
}

void putTextInBox(Mat &img, std::string s, Point p) {
	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;
	int baseline = 0;
	String text = s;

	Size textSize = getTextSize(text, fontFace, fontScale, thickness,
			&baseline);
	Point textOrg(p);
	rectangle(img, textOrg,
			textOrg + Point(textSize.width, textSize.height + 2 * baseline),
			Scalar(0, 0, 0), CV_FILLED);
	putText(img, text, textOrg + Point(0, textSize.height + baseline), fontFace,
			fontScale, Scalar::all(255), thickness, 8);
}

void putTextAboveBox(Mat &img, std::string s, Point p) {
	int fontFace = FONT_HERSHEY_PLAIN;
	double fontScale = 1;
	int thickness = 1;
	int baseline = 0;
	String text = s;

	Size textSize = getTextSize(text, fontFace, fontScale, thickness,
			&baseline);
	Point textOrg(p);
	rectangle(img, textOrg,
			textOrg + Point(textSize.width, -(textSize.height + 2 * baseline)),
			Scalar(0, 0, 0), CV_FILLED);
	putText(img, text, textOrg + Point(0, -baseline), fontFace, fontScale,
			Scalar::all(255), thickness, 8);
}

void putType(Mat &img, Selection sel) {
	putTextAboveBox(img, sel.getKey(", "), sel.rect.tl());
}

void putTitle(Mat &img, FileList fl) {
	putTextInBox(img, fl.getTitle(), Point(0, 0));
}

void drawSelections(String windowMain, String windowSelection, Mat img,
		Mat selImg, Selections selections, MouseEvent eventMain,
		MouseEvent eventSelection, bool &redrawMain, bool &redrawSelection,
		FileList fl) {

	if (redrawMain || (eventMain.newEvent && eventMain.selectionStart)) {
		Mat tmp;
		img.copyTo(tmp);

		if (fl.isDetection()) {
			Rectangles detected = fl.getDetectedRectangles();
			for (uint i = 0; i < detected.size(); i++) {
				rectangle(tmp, detected[i], DETECTED);
			}
		}

		for (uint i = 0; i < selections.size(); i++) {
			rectangle(tmp, selections.getRect(i),
					(selections.idx == i) ? EDIT : STORED);
			if (selections.idx == i) {
				putType(tmp, selections.getSelection(i));
			}
		}

		if (eventMain.selectionStart && (eventMain.rect.area() > 0)) {
			rectangle(tmp, eventMain.rect, SELECTION);
		}

		putTitle(tmp, fl);
		imshow(windowMain, tmp);
		redrawMain = false;
	}

	if (redrawSelection
			|| (eventSelection.newEvent && eventSelection.selectionStart)) {
		Mat tmp;
		selImg.copyTo(tmp);

		rectangle(tmp, eventSelection.rect, SELECTION);
		imshow(windowSelection, tmp);
		redrawSelection = false;
	}
}

void selectionOnSelectedWindow(MouseEvent &selectedEvent, Rect& selSelection,
		bool &isSelected, double selRatio, Rect mainSelection) {

	if (selectedEvent.newEvent && selectedEvent.selectionDone) {
		//resizing and shifting the rectangle, because user selects
		//in a cut and magnified region
		Rect m = selectedEvent.rect;
		Rect rect = Rect(m.x / selRatio, m.y / selRatio, m.width / selRatio,
				m.height / selRatio);
		rect += Point(mainSelection.x, mainSelection.y);
		selSelection = rect;
		isSelected = true;
		selectedEvent.clearSelection();
	}
}

void selectionOnMainWindow(String wSelected, Mat img, Mat &sel,
		MouseEvent &mainEvent, double &selRatio, Rect &mainSelection) {

	if (mainEvent.newEvent && mainEvent.selectionDone
			&& mainEvent.rect.area() > 0) {
		mainSelection = mainEvent.rect;
		Mat roi(img, mainEvent.rect);
		roi.copyTo(sel);
		double ratiox = (double) img.cols / (double) sel.cols;
		double ratioy = (double) img.rows / (double) sel.rows;
		selRatio = MIN(ratiox, ratioy);
		resize(sel, sel, Size(), selRatio, selRatio);
		imshow(wSelected, sel);
		mainEvent.clearSelection();
	}
}

void save(std::string fileName, FileList fl) {
	std::ofstream oFile;
	char buf[50];
	//archive existing saved data
	std::time_t t = std::time(0);  // t is an integer type
	std::sprintf(buf, "%s-%ld", fileName.c_str(), t);
	rename(fileName.c_str(), buf);

	//saving data
	oFile.open(fileName.c_str());
	oFile << fl;
	oFile.flush();
	oFile.close();
}

void load(std::string fileName, FileList &fl) {
	std::ifstream iFile;
	iFile.open(fileName.c_str());
	if (iFile.good()) {
		iFile >> fl;
	}
	iFile.close();
}

bool keyManager(Selections &sel, bool &redrawMain, bool &redrawSelection,
		Mat &img, FileList &fileList, MouseEvent &mainEvent,
		std::string outputFile) {
	char c = waitKey(10);

	if (c == -1) {
		return true;
	}

	std::cout << c << std::endl;

	while (waitKey(1) != -1) {
	};

	if (c == 'q' || c == 27) {
		return false;
	}

	if (c == 'D') { //clear selections
		sel.clear();
		redrawMain = true;
	}

	if (c == 'l') { //load selections
		fileList.setSelections(sel);
		load(outputFile, fileList);
		fileList.getSelections(sel);
		img = fileList.getImage();
		redrawMain = true;
	}

	if (c == 'w') { //write files
		fileList.setSelections(sel);
		save(outputFile, fileList);
	}

	if (c == 'S') { //Right arrow
		img = fileList.getNext(sel, 1, img);
		mainEvent.activate();
		redrawMain = true;
		redrawSelection = true;
	}

	if (c == 'R') { //UP arrow
		img = fileList.getNext(sel, 10, img);
		mainEvent.activate();
		redrawMain = true;
		redrawSelection = true;
	}

	if (c == 'Q') { //Left arrow
		img = fileList.getPrev(sel, 1);
		mainEvent.activate();
		redrawMain = true;
		redrawSelection = true;
	}

	if (c == 'T') { //Down arrow
		img = fileList.getPrev(sel, 10);
		mainEvent.activate();
		redrawMain = true;
		redrawSelection = true;
	}

	if (c == 'b') {
		fileList.toggleBackground();
		redrawMain = true;
	}

	if (c == 't') {
		sel.incType();
		redrawMain = true;
	}

	if (c == 's') {
		sel.incSubtype();
		redrawMain = true;
	}

	if (c == 'd') {
		sel.erase();
		redrawMain = true;
	}

	return true;
}

int main(int argc, const char** argv) {
	//cascade file for object autodetection
	std::string cascadeFile = "";
	//file glob for images
	std::string filenamePattern = "";
	//name of output file
	std::string outputFile = "";
	bool isTracking = true;

	CLI cli;
	if (!cli.parse(argc, argv, filenamePattern, cascadeFile, isTracking,
			outputFile)) {
		return 0;
	}

	FileList fileList(filenamePattern, cascadeFile);
	load(outputFile, fileList);

	String wMain = "Select a rectangle with the Mouse for magnifying";
	String wSelected = "Select region";

	bool redrawMain = true;
	bool redrawSelection = false;

	Selections selections;
	fileList.getSelections(selections);

	Mat img = fileList.getActual(selections);
	Mat sel;

	//the values below need for back-counting the selected region on wSelected
	//into wMain
	Rect mainSelection(0, 0, img.cols, img.rows);	//selection on wMain
	Rect selSelection;	//selection on wSelected
	double selRatio = 1.0;	//magnifying ratio of selected ROI
	bool isSelected = false; // true, if user selected a region in wSelected

	namedWindow(wMain, WINDOW_AUTOSIZE);
	namedWindow(wSelected, WINDOW_AUTOSIZE);
	img.copyTo(sel);

	imshow(wMain, img);
	imshow(wSelected, sel);

	MouseEvent mouseMainEvent("main", img.cols, img.rows);
	MouseEvent mouseSelectedEvent("selected", 0, 0);

	setMouseCallback(wMain, onMouse, &mouseMainEvent);
	setMouseCallback(wSelected, onMouse, &mouseSelectedEvent);

	for (;;) {
		if (!keyManager(selections, redrawMain, redrawSelection, img, fileList,
				mouseMainEvent, outputFile)) {
			break;
		}

		selectionOnSelectedWindow(mouseSelectedEvent, selSelection, isSelected,
				selRatio, mainSelection);

		if ((mouseMainEvent.newEvent || mouseSelectedEvent.rightButton)
				&& isSelected) {
			//only the last selection on wSelected will be stored
			isSelected = false;
			selections.add(Selection(selSelection));
			redrawMain = true;
			mouseSelectedEvent.rightButton = false;
		} else if (mouseMainEvent.newEvent) {
			redrawMain = selections.setSelected(mouseMainEvent.pt)
					|| redrawMain;
		}

		drawSelections(wMain, wSelected, img, sel, selections, mouseMainEvent,
				mouseSelectedEvent, redrawMain, redrawSelection, fileList);

		selectionOnMainWindow(wSelected, img, sel, mouseMainEvent, selRatio,
				mainSelection);

		mouseMainEvent.newEvent = false;
		mouseSelectedEvent.newEvent = false;
	}

	return 0;
}
