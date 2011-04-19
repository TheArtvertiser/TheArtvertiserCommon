/*
 * Artvert.h
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#ifndef ARTVERT_H_
#define ARTVERT_H_

#include "ofConstants.h"
#include "ofFileUtils.h"
#include "ofPoint.h"

#define DEFAULT_ARTVERT_FOLDER "adverts/"

class Artvert {
public:
	Artvert();
	Artvert(string uid,string folder=DEFAULT_ARTVERT_FOLDER);

	string getUID() const;
	void setUID(const string & uid);

	bool isReady();

	ofFile & getCompressedImage();
	ofFile & getModel();
	ofFile & getROIFile();
	ofFile & getDetectorData();
	ofFile & getTrackerData();

	const ofFile & getCompressedImage() const;
	const ofFile & getModel() const;
	const ofFile & getROIFile() const;
	const ofFile & getDetectorData() const;
	const ofFile & getTrackerData() const;

	vector<ofPoint> getROI();

	void save();

	static vector<Artvert> listAll(string folder=DEFAULT_ARTVERT_FOLDER);

private:
	string uid;
	string folder;
	ofFile compressedImage;
	ofFile model;
	ofFile roiFile;
	ofFile detectorData;
	ofFile trackerData;
	vector<ofPoint> roi;
};


#endif /* ARTVERT_H_ */
