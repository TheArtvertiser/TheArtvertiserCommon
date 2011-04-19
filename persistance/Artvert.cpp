/*
 * Artvert.cpp
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#include "Artvert.h"
#include "PersistanceEngine.h"


Artvert::Artvert()
:folder(ofFilePath::addTrailingSlash(DEFAULT_ARTVERT_FOLDER))
{

}

Artvert::Artvert(string uid,string folder)
:uid(uid)
,folder(ofFilePath::addTrailingSlash(folder))
,compressedImage(folder + uid+".jpg")
,model(folder + uid+".bmp")
,roiFile(folder + uid+".bmp.roi")
,detectorData(folder + uid+".bmp.detector_data")
,trackerData(folder + uid+".bmp.tracker_data")
,roi(4)
{
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
	}
}

string Artvert::getUID() const{
	return uid;
}

void Artvert::setUID(const string & _uid){
	uid = _uid;
	compressedImage.open(folder + uid+".jpg");
	model.open(folder + uid+".bmp");
	roiFile.open(folder + uid+".bmp.roi");
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
	}
	detectorData.open(folder + uid+".bmp.detector_data");
	trackerData.open(folder + uid+".bmp.tracker_data");
}

bool Artvert::isReady(){
	return uid!="" && model.exists() && roiFile.exists() && detectorData.exists() && trackerData.exists();
}

ofFile & Artvert::getCompressedImage(){
	return compressedImage;
}

ofFile & Artvert::getModel(){
	return model;
}

ofFile & Artvert::getROIFile(){
	return roiFile;
}

ofFile & Artvert::getDetectorData(){
	return detectorData;
}

ofFile & Artvert::getTrackerData(){
	return trackerData;
}

const ofFile & Artvert::getCompressedImage() const{
	return compressedImage;
}

const ofFile & Artvert::getModel() const{
	return model;
}

const ofFile & Artvert::getROIFile() const{
	return roiFile;
}

const ofFile & Artvert::getDetectorData() const{
	return detectorData;
}

const ofFile & Artvert::getTrackerData() const{
	return trackerData;
}

vector<ofPoint> Artvert::getROI(){
	if(!roi.empty()) return roi;

	if(roiFile.exists()){
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		roiFile.seekg(0,ios_base::beg);
		return roi;
	}

	return vector<ofPoint>(4);

}

void Artvert::save(){
	int artvert = PersistanceEngine::artverts().addTag("artvert");
	PersistanceEngine::artverts().addAttribute("artvert","uid",uid,artvert);
}

vector<Artvert> Artvert::listAll(string folder){
	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numArtverts = xml.getNumTags("artvert");
	vector<Artvert> artverts(numArtverts,Artvert("",folder));

	for(int i=0; i<numArtverts; i++){
		artverts[i].setUID(xml.getAttribute("artvert","uid","",i));
	}

	return artverts;
}
