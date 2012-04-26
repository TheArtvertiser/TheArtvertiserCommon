/*
 * ThreadedDetector.cpp
 *
 *  Created on: 24/04/2012
 *      Author: arturo
 */

#include "ThreadedDetector.h"
#include "PersistanceEngine.h"

ThreadedDetector::ThreadedDetector() {
	// TODO Auto-generated constructor stub

}

ThreadedDetector::~ThreadedDetector() {
	// TODO Auto-generated destructor stub
}


void ThreadedDetector::setup(string _srcFolder, string _moveFinishedTo){
	moveFinishedTo = ofFilePath::addTrailingSlash(_moveFinishedTo);
	srcFolder = ofFilePath::addTrailingSlash(_srcFolder);

	//TODO: save artverts xml in the corresponding folder
	vector<Artvert> artverts = Artvert::listAll(srcFolder);
	for(int i=0; i<(int)artverts.size(); i++){
		if(artverts[i].checkIntegrity()){
			if(!Artvert(artverts[i].getUID(),_moveFinishedTo).isReady()){
				ofLogVerbose("artvertiserServer") << "loading already uploaded " << artverts[i].getUID();
				adverts.push(artverts[i]);
			}
		}else{
			ofLogError("artvertiserServer") << "found corrupt or incomplete advert " << artverts[i].getUID() << " deleting";
			artverts[i].remove();
		}
	}

	artverts = Artvert::listAll(_moveFinishedTo);
	for(int i=0; i<(int)artverts.size(); i++){
		if(!artverts[i].checkIntegrity()){
			ofLogError("artvertiserServer") << "found corrupt or incomplete advert " << artverts[i].getUID() << " deleting";
			artverts[i].remove();
		}else{
			ofFile icon(_moveFinishedTo+artverts[i].getUID()+".icon.jpg");
			if(!icon.exists()){
				ofImage iconImg(artverts[i].getCompressedImage());
				iconImg.resize(160,120);
				iconImg.saveImage(_moveFinishedTo+artverts[i].getUID()+".icon.jpg");
			}
		}
	}
}

void ThreadedDetector::update(){
	if(detector.getState()==Detector::Init){
		analizeNext();
	}
	if(detector.getState()==Detector::Finished){
		locationDB.addLocation(currentAdvert);
		if(!currentAdvert.hasAlias()){
			currentAdvert.getMD5File().moveTo(moveFinishedTo+currentAdvert.getMD5File().getFileName()+".notready");

			ofLogError("artvertiserServer") << "finished analysis copying images to server folder" << endl;
			ofFile md5(currentAdvert.getMD5File().path(),ofFile::WriteOnly);
			md5 << currentAdvert.generateMD5();
			md5.close();

			ofImage iconImg(currentAdvert.getCompressedImage());
			iconImg.resize(160,120);
			iconImg.saveImage(moveFinishedTo+currentAdvert.getUID()+".icon.jpg");

			currentAdvert.getMD5File().moveTo(moveFinishedTo+currentAdvert.getMD5File().getFileName());
			currentAdvert.getModel().moveTo(moveFinishedTo+currentAdvert.getModel().getFileName());
			currentAdvert.getLocationFile().moveTo(moveFinishedTo+currentAdvert.getLocationFile().getFileName());
			currentAdvert.getCompressedImage().copyTo(moveFinishedTo+currentAdvert.getCompressedImage().getFileName());
			currentAdvert.getROIFile().moveTo(moveFinishedTo+currentAdvert.getROIFile().getFileName());
			currentAdvert.getDetectorData().moveTo(moveFinishedTo+currentAdvert.getDetectorData().getFileName());
			currentAdvert.getTrackerData().moveTo(moveFinishedTo+currentAdvert.getTrackerData().getFileName());
		}
		analizeNext();
	}
}


void ThreadedDetector::analizeNext(){
	mutex.lock();
	if(!adverts.empty()){
		currentAdvert = adverts.front();
		adverts.pop();

		if(currentAdvert.isReady()){
			mutex.unlock();
			return;
		}

		ofLogVerbose("artvertiserServer","ArtvertiserServer: got new artvert: " + currentAdvert.getUID());
        ofLogVerbose("artvertiserServer","loading " + currentAdvert.getCompressedImage().getAbsolutePath() );
        //currentImg.clear();
		currentImg.loadImage(currentAdvert.getCompressedImage());
        //currentImg.loadImage("/Users/damian/Desktop/test.jpg");
        //currentImg.loadImage("/Users/damian/code/ofgit/apps/artvertiser/TheArtvertiserServer/bin/data/uploads/20111119_144037535.jpg");
        //currentImg.loadImage(currentAdvert.getCompressedImage().getAbsolutePath());

		ofImage bmp;
		bmp.setUseTexture(false);
		bmp.loadImage(currentAdvert.getCompressedImage());
		bmp.saveImage(currentAdvert.getModel());

		/*ofLogVerbose("artvertiserServer") << "got " + currentAdvert.getUID() << endl;
		vector<Artvert> artverts = Artvert::listAll(moveFinishedTo);
		for(int i=0;i<(int)artverts.size();i++){
			if(currentAdvert.getUID() == artverts[i].getUID() || artverts[i].hasAlias() || !artverts[i].isReady()) continue;
			ofImage img;
			img.loadImage(artverts[i].getModel());
			detector.setup(artverts[i].getModel().path(),img.getWidth(),img.getHeight(),artverts[i].getROI(),true);
			detector.newFrame(bmp.getPixelsRef());
			if(detector.isDetected() || detector.isTracked()){
				redirections[currentAdvert] = artverts[i];
				currentAdvert.setAliasUID(artverts[i].getUID());
				currentAdvert.save();
				PersistanceEngine::save();
				detector.close();
				cerr << "redirecting " << currentAdvert.getUID() << "->" << artverts[i].getUID() << endl;
				mutex.unlock();
				return;
			}
		}*/

		detector.setupTrainOnly(currentAdvert.getModel().path());
	}else{
		if(detector.getState()!=Detector::Init) detector.close();
	}
	mutex.unlock();
}



void ThreadedDetector::lock(){
	mutex.lock();
}

void ThreadedDetector::unlock(){
	mutex.unlock();
}

LocationDB & ThreadedDetector::getLocationDB(){
	return locationDB;
}

void ThreadedDetector::addAdvertForTraining(Artvert & artvert){
	mutex.lock();
	adverts.push(artvert);
	mutex.unlock();
}

queue<Artvert> ThreadedDetector::getPending(){
	return adverts;
}

Detector::State ThreadedDetector::getState(){
	return detector.getState();
}

Artvert ThreadedDetector::getCurrentArtvert(){
	return currentAdvert;
}

ofImage & ThreadedDetector::getCurrentImage(){
	Poco::ScopedLock<ofMutex> lock(mutex);
	return currentImg;

}
