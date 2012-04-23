/*
 * Artvertiser.h
 *
 *  Created on: 16/03/2011
 *      Author: arturo
 */

#ifndef ARTVERTISER_H_
#define ARTVERTISER_H_

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxFern.h"

class Detector: public ofThread {
public:
	Detector();
	virtual ~Detector();

	void close();
	void setup(string model, int width, int height, const vector<ofPoint> & srcQuad, bool lock=false);
	void setup(string model, ofVideoGrabber & video, const vector<ofPoint> & srcQuad, bool lock=false, int _overrideWidth= -1, int _overrideHeight = -1 );
	void setupTrainOnly(string model);
	void newFrame(ofPixels & pixels);

	ofMatrix4x4 getHomography();
	bool isDetected();
	bool isTracked();

	enum State{
		Init,
		Training,
		Initializing,
		Finished,
		Running
	};

	void setSrcQuad(vector<ofPoint> srcQuad);

	State getState(){return state;}
	float getFps(){return fps;}
	ofxFern fern;
protected:
	void threadedFunction();

private:
	void init();
	ofVideoGrabber * video;
	int overrideWidth, overrideHeight;
	int width,height;
	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage img, img640;
	vector<ofPoint> srcQuad;
	string model;
	ofMatrix4x4 homography;
	ofMatrix4x4 overrideScale;
	ofMutex mutex;
	float fps;
	State state;
	int counter;
	bool isNewFrame;

	int fps_time;
	int frames;
	int time;
	bool trainOnly;
};

#endif /* ARTVERTISER_H_ */
