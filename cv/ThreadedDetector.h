/*
 * ThreadedDetector.h
 *
 *  Created on: 24/04/2012
 *      Author: arturo
 */

#ifndef THREADEDDETECTOR_H_
#define THREADEDDETECTOR_H_

#include "ofConstants.h"

#include <queue>
#include <set>
#include <map>

#include "ofxOpenCv.h"
#include "Detector.h"
#include "Artvert.h"
#include "LocationDB.h"

class ThreadedDetector {
public:
	ThreadedDetector();
	virtual ~ThreadedDetector();

	void setup(string srcFolder, string moveFinishedTo="www");
	void update();
	void analizeNext();

	void lock();
	void unlock();
	LocationDB & getLocationDB();
	void addAdvertForTraining(Artvert & artvert);
	queue<Artvert> getPending();
	Detector::State getState();
	Artvert getCurrentArtvert();
	ofImage & getCurrentImage();


private:
	Detector detector;

	queue<Artvert> adverts;
	map<Artvert,Artvert> redirections;
	ofMutex mutex;

	Artvert currentAdvert;
	ofImage currentImg;
    LocationDB locationDB;
    string moveFinishedTo;
    string srcFolder;

};

#endif /* THREADEDDETECTOR_H_ */
