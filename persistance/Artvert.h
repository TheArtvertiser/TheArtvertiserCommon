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
#include "ofxGeoLocation.h"

#define DEFAULT_ARTVERT_FOLDER "adverts/"

class Artvert {
public:
	Artvert();
	Artvert(string uid,string folder=DEFAULT_ARTVERT_FOLDER);

	string getUID() const;
	void setUID(const string & uid);

	string getAliasUID() const;
	void setAliasUID(const string & uid);
	bool hasAlias() const;

	Artvert getAlias();

	bool isReady();

	ofFile & getCompressedImage();
	ofFile & getModel();
	ofFile & getROIFile();
	ofFile & getDetectorData();
	ofFile & getTrackerData();
	ofFile & getLocationFile();

	const ofFile & getCompressedImage() const;
	const ofFile & getModel() const;
	const ofFile & getROIFile() const;
	const ofFile & getDetectorData() const;
	const ofFile & getTrackerData() const;
	const ofFile & getLocationFile() const;

	vector<ofFile> getArtverts();

	vector<ofPoint> getROI();
	ofxLocation getLocation();

	void save();

	static vector<Artvert> listAll(string folder=DEFAULT_ARTVERT_FOLDER);

	//this allows to compare artverts by their paths, also provides sorting and use as key in stl containers
	bool operator==(const Artvert & file) const;
	bool operator!=(const Artvert & file) const;
	bool operator<(const Artvert & file) const;
	bool operator<=(const Artvert & file) const;
	bool operator>(const Artvert & file) const;
	bool operator>=(const Artvert & file) const;

private:
	string uid;
	string aliasUID;
	string folder;
	ofFile compressedImage;
	ofFile model;
	ofFile roiFile;
	ofFile detectorData;
	ofFile trackerData;
	ofFile locationData;
	vector<ofPoint> roi;
};


#endif /* ARTVERT_H_ */
