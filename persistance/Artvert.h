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
#include "ofxMD5.h"

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

	ofFile getCompressedImage() const;
	ofFile getModel() const;
	ofFile getROIFile() const;
	ofFile getDetectorData() const;
	ofFile getTrackerData() const;
	ofFile getLocationFile() const;
	ofFile getMD5File() const;

	vector<ofFile> getArtverts();

	vector<ofPoint> getROI();
	ofxLocation getLocation();

	bool checkIntegrity();
	string getStoredMD5();
	string generateMD5();



	void remove();
	void removeAnalisys();

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
	string compressedImage;
	string model;
	string roiPath;
	string detectorData;
	string trackerData;
	string locationData;
	string md5Path;
	vector<ofPoint> roi;
};


#endif /* ARTVERT_H_ */
