/*
 * Artvertiser.cpp
 *
 *  Created on: 16/03/2011
 *      Author: arturo
 */

#include "Detector.h"
#include "homography.h"
#ifdef TARGET_ANDROID
#include "ofxAndroidVideoGrabber.h"
#endif

Detector::Detector() {
	state = Init;
	counter = 0;
	fps=0;
	isNewFrame = false;

	fps_time=0;
	frames=0;

	overrideWidth = -1;
	overrideScale.makeIdentityMatrix();
}

Detector::~Detector() {
	// TODO Auto-generated destructor stub
}

void Detector::close(){
	state = Init;
}

void Detector::setup(string _model, int _width, int _height, const vector<ofPoint> & _srcQuad, bool lock){
	video = NULL;
	model = _model;

	srcQuad = _srcQuad;

	fern.setUseTexture(false);
	fern.setMode('5');
	fern.setMode('0');

	width = _width;
	height = _height;

	colorImg.allocate(width,height);
	img.allocate(width,height);
	img.setUseTexture(false);
	colorImg.setUseTexture(false);


	trainOnly = false;
	state = Initializing;
	if(!lock) startThread(true,false);
	else init();
}

void Detector::setup(string _model, ofVideoGrabber & _video, const vector<ofPoint> & _srcQuad, bool lock, int _overrideWidth, int _overrideHeight ){
	model = _model;
	video = &_video;
	srcQuad = _srcQuad;

	fern.setUseTexture(false);
	fern.setMode('5');
	fern.setMode('0');

	overrideWidth = _overrideWidth;
	overrideHeight = _overrideHeight;
	
	if ( overrideWidth > 0 )
	{
		width = overrideWidth;
		height = overrideHeight;
		overrideScale.makeScaleMatrix( (float)width/overrideWidth, (float)height/overrideHeight, 1.0f );
	}
	else
	{
		width = video->getWidth();
		height = video->getHeight();
		overrideScale.makeIdentityMatrix();
	}
	ofLogNotice( "Detector") << "running detection at " << width << "x" << height << " from camera " << video->getWidth() << "x" << video->getHeight();


	colorImg.allocate(width, height );
	img.allocate( width, height );
	//img640.allocate(640,480);
	img.setUseTexture(false);
	colorImg.setUseTexture(false);
	//img640.setUseTexture(false);

#ifdef TARGET_ANDROID
	ofxAndroidVideoGrabber * grabber = (ofxAndroidVideoGrabber*) video->getGrabber().get();
	ofAddListener(grabber->newFrameE,this,&Detector::newFrame);
#elif defined (TARGET_LINUX)
	ofGstVideoGrabber * grabber = (ofGstVideoGrabber*) video->getGrabber().get();
	ofGstVideoUtils * videoUtils = grabber->getGstVideoUtils();
	ofAddListener(videoUtils->bufferEvent,this,&Detector::newFrame);
#endif

	trainOnly = false;
	state = Initializing;
	if(!lock) startThread(true,false);
	else init();
}

void Detector::setupTrainOnly(string _model){
	model = _model;
	fern.setUseTexture(false);
	fern.setMode('5');
	fern.setMode('0');

	ofImage img;
	img.setUseTexture(false);
	img.loadImage(model);
	width = img.getWidth();
	height = img.getHeight();

	trainOnly = true;
	state = Training;
	startThread(true,false);
}

void Detector::newFrame(ofPixels & pixels){
	if(state!=Running || !pixels.getPixels()) return;
	if(pixels.getImageType()==OF_IMAGE_COLOR){
		if ( overrideWidth > 0 )
		{
			ofPixels tmpPixels = pixels;
			tmpPixels.resize( overrideWidth, overrideHeight );
			colorImg = tmpPixels.getPixels();
		}
		else
		{
			colorImg = pixels.getPixels();
		}
		img = colorImg;
		fern.update(img);
		//img640.scaleIntoMe(img);
	}else{
		if ( overrideWidth > 0 )
		{
			ofPixels tmpPixels = pixels;
			tmpPixels.resize( overrideWidth, overrideHeight );
			fern.update(tmpPixels);
		}
		else
		{
			fern.update( pixels );
		}
		//img640.scaleIntoMe(img,CV_INTER_LINEAR);
	}
	isNewFrame = true;

	mutex.lock();
	findOpenCvHomography(&srcQuad[0],&fern.getLastQuad()[0],homography.getPtr());
	mutex.unlock();


	int curr_time = ofGetElapsedTimeMillis();
	frames++;
	if(curr_time - fps_time >=1000){
		fps=float(frames*1000)/(curr_time - fps_time);
		fps_time=curr_time;
		frames=0;
	}
}

void Detector::init(){
	fern.initTracker(model,width,height);
	if(trainOnly) state=Finished;
	else state = Running;
	fps_time = ofGetElapsedTimeMillis();
}

void Detector::threadedFunction(){
	init();
	stopThread();
	/*while(isThreadRunning()){
		while(!isNewFrame) ofSleepMillis(10);
		mutex.lock();
		img = colorImg;
		isNewFrame = false;
		mutex.unlock();
		fern.update(img);
		mutex.lock();
		findOpenCvHomography(&srcQuad[0],&fern.getLastQuad()[0],homography.getPtr());
		mutex.unlock();
		ofSleepMillis(2);
	}*/
}

ofMatrix4x4 Detector::getHomography(){
	Poco::ScopedLock<ofMutex> l(mutex);
	return overrideScale*homography;
}

bool Detector::isDetected(){
	return fern.state == ofxFern::Detected;
}

bool Detector::isTracked(){
	return fern.state == ofxFern::Tracked;

}

void Detector::setSrcQuad(vector<ofPoint> _srcQuad){
	srcQuad = _srcQuad;
}
