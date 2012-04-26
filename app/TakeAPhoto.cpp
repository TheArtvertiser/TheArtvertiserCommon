/*
 Copyright 2011 Arturo Castro <arturo@arturocastro.net>,

 Distributed under the terms of the GNU General Public License v3.

 This file is part of The Artvertiser for Android.

 The Artvertiser for Android is free software: you can redistribute
 it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation, either version 3 of
 the License, or (at your option) any later version.

 The Artvertiser for Android is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with The Artvertiser for Android.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TakeAPhoto.h"

#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "Artvert.h"

TakeAPhoto::TakeAPhoto()
:imgSource(NULL)
,state(Init)
,cameraButton(new gui::Button)
,yesButton(new gui::Button)
,noButton(new gui::Button)
,exitButton(new gui::Button)
,scale(1)
,videoWidth(0)
,videoHeight(0)
,pixelsCopied(false)
,address(new gui::Label)
{

}

void TakeAPhoto::setGeo(ofPtr<ofxGeoLocation> & _geo){
	geo = _geo;
}

void TakeAPhoto::setup(ofBaseImage & _imgSource, bool _metadataOnly){
	state = Init;
	imgSource = &_imgSource;
	metadataOnly = _metadataOnly;

	quad.resize(4);
	quad[0].set(20,20);
	quad[1].set(620,20);
	quad[2].set(620,460);
	quad[3].set(20,460);

	warp.setMinDistance(ofGetWidth()*ofGetHeight()*.00015);

	photoIcon.loadImage("icons/camera.png");
	cameraButton->setIcon(photoIcon);
	ofAddListener(cameraButton->pressedE,this,&TakeAPhoto::takePhoto);

	yesIcon.loadImage("icons/yes.png");
	yesButton->setIcon(yesIcon);
	ofAddListener(yesButton->pressedE,this,&TakeAPhoto::yesPressed);

	noIcon.loadImage("icons/no.png");
	noButton->setIcon(noIcon);
	ofAddListener(noButton->pressedE,this,&TakeAPhoto::noPressed);

	exitIcon.loadImage("icons/exit.png");
	exitButton->setIcon(exitIcon);
	ofAddListener(exitButton->pressedE,this,&TakeAPhoto::exitPressed);

	ofResizeEventArgs window;
	window.width = ofGetWidth();
	window.height = ofGetHeight();
	windowResized(window);

	borderFrame.setWidth(yesIcon.getWidth()*0.5);
	borderFrame.setVSpacing(yesIcon.getHeight()*0.1);
	borderFrame.addWidget(cameraButton);
	borderFrame.disableEvents();


	photo.setAnchorPercent(0.5,0.5);


	ofAddListener(ofEvents().windowResized,this,&TakeAPhoto::windowResized);

	if(geo) {
		geoPanel.setRectCompressed(ofRectangle(0,0,380,40));
		geoPanel.setRectExpanded(ofRectangle(0,0,380,320));
		geoPanel.setLeftMargin(20);
		geoPanel.expand();
		geoPanel.addWidget(address);
		geoPanel.addDrawable(geo);
		geoPanel.setVSpacing(15);
		geoPanel.enableEvents();
	}

}

void TakeAPhoto::start(){
	updateState(Start);
}

void TakeAPhoto::stop(){
	updateState(Stop);
}

void TakeAPhoto::initWarp(){
	if(!imgSource) return;
	scale = ofGetHeight()/imgSource->getHeight();
	videoWidth = imgSource->getWidth()*scale;
	videoHeight = imgSource->getHeight()*scale;

	warp.setInitialQuad(quad,ofPoint(ofGetWidth()*0.5-videoWidth*0.5,ofGetHeight()*0.5-videoHeight*0.5),ofPoint(scale,scale));
}

void TakeAPhoto::windowResized(ofResizeEventArgs & window){
	if(!imgSource || imgSource->getWidth()==0 || imgSource->getHeight()==0) return;
	initWarp();

	int borderX = window.width*0.5 + videoWidth*0.5 + yesIcon.getWidth()*0.1;
	int borderY = yesIcon.getWidth()*0.1;
	borderFrame.setPosition(ofPoint(borderX,borderY));
}


void TakeAPhoto::updateState(Transition transition){
	if(transition==Stop){
		ofLogVerbose("TakeAPhoto","stopping");
		imgSource->resetAnchor();
		borderFrame.disableEvents();
		state = Init;
		if(geo) geo->stop();
		ofRemoveListener(ofEvents().touchDoubleTap,this,&TakeAPhoto::touchDoubleTap);
		bool yes;
		ofNotifyEvent(exitE,yes,this);
		return;
	}

	switch(state){
	case Init:
		if(transition==Start){
			ofLogVerbose("TakeAPhoto","starting");
			borderFrame.clear();
			borderFrame.addWidget(cameraButton);
			borderFrame.addWidget(exitButton);
			borderFrame.enableEvents();
			imgSource->setAnchorPercent(0.5,0.5);

			if(metadataOnly){
				photoPixels = imgSource->getPixelsRef();
				state = PhotoTaken;
				pixelsCopied = true;
			}else{
				state = TakingPhoto;
			}
			if(geo) {
				geo->setSize(320,240);
				geo->start();
			}

			if(geo) geoPanel.compress();
			ofAddListener(ofEvents().touchDoubleTap,this,&TakeAPhoto::touchDoubleTap);
		}
		break;

	case TakingPhoto:
		if(transition==PhotoPressed){
			ofLogVerbose("TakeAPhoto","photo pressed");
			borderFrame.clear();
			state = PhotoTaken;
		}
		break;

	case PhotoTaken:
		if(transition==UpdatedImage){
			ofLogVerbose("TakeAPhoto","photo stored in memory");
			warp.enableEvents();
			initWarp();
			borderFrame.clear();
			borderFrame.addWidget(yesButton);
			borderFrame.addWidget(noButton);
			borderFrame.addWidget(exitButton);
			state = SelectingQuad;
		}
		break;

	case SelectingQuad:
		if(transition==YesPressed){
			warp.disableEvents();
			borderFrame.clear();
			borderFrame.addWidget(cameraButton);
			borderFrame.addWidget(exitButton);
			state = EnteringName;
		}
		if(transition==NoPressed){
			ofLogVerbose("TakeAPhoto","photo cancelled");
			if(!metadataOnly){
				warp.disableEvents();
				borderFrame.clear();
				borderFrame.addWidget(cameraButton);
				borderFrame.addWidget(exitButton);
				state = TakingPhoto;
			}else{
				state = PhotoTaken;
				pixelsCopied = true;
			}
		}
		break;

	case EnteringName:
		if(transition==TextAccepted){
			state = TakingPhoto;

			ofDirectory("adverts").create();

			string filename = ofGetTimestampString("%Y%m%d_%H%M%S%i");

			ofLogVerbose("TakeAPhoto","advert complete " + filename);
			Artvert advert(filename,"adverts/");
			advert.setAdvertName(advertName);

			photo.saveImage(advert.getCompressedImage(),OF_IMAGE_QUALITY_BEST);
			photo.saveImage(advert.getModel(),OF_IMAGE_QUALITY_BEST);

			ofLogVerbose("TakeAPhoto","image saved");

			ofFile roi = advert.getROIFile();
			roi.changeMode(ofFile::WriteOnly);
			for(int i=0;i<4;i++){
				roi << (int)warp.getQuad()[i].x << " " << (int)warp.getQuad()[i].y << endl;
			}
			roi.close();

			ofLogVerbose("TakeAPhoto","roi saved");

			ofFile location = advert.getLocationFile();
			location.changeMode(ofFile::WriteOnly);
			ofxLocation loc;
			if(geo) loc = geo->getLocation();
			location << loc;
			location.close();

			ofLogVerbose("TakeAPhoto") << "location stored: " << loc;

			ofFile md5 = advert.getMD5File();
			md5.changeMode(ofFile::WriteOnly);
			string md5sig = advert.generateMD5();
			md5 << md5sig;
			md5.close();

			ofLogVerbose("TakeAPhoto") << "md5 stored: " << md5sig;

			ofNotifyEvent(newPhotoE,advert,this);
		}
		if(transition==TextCanceled){
			ofLogVerbose("TakeAPhoto","photo cancelled");
			if(!metadataOnly){
				state = TakingPhoto;
			}else{
				state = PhotoTaken;
				pixelsCopied = true;
			}
		}
		break;
	}

}

void TakeAPhoto::update(){
	
#ifdef TARGET_OSX
	if ( ((ofVideoGrabber*)imgSource)->isFrameNew() )
	{
		newFrame( imgSource->getPixelsRef() );
	}
#endif
	

	if(videoWidth==0 || videoHeight==0){
		ofResizeEventArgs window;
		window.width = ofGetWidth();
		window.height = ofGetHeight();
		windowResized(window);
	}
	borderFrame.update();
	
	

	if(state==PhotoTaken && pixelsCopied){
		photo = photoPixels;
		pixelsCopied = false;
		updateState(UpdatedImage);
	}

	if(state==EnteringName){
		advertName = ofSystemTextBoxDialog("enter name of the advert");
		if(advertName!=""){
			updateState(TextAccepted);
		}else{
			updateState(TextCanceled);
		}
	}

	if(geo){
		geo->update();
		address->setText(geo->getAddress());
		geoPanel.repositionWidgets();
		geoPanel.update();
	}

}

void TakeAPhoto::draw(){
	ofPushStyle();
	ofSetColor(255);
	borderFrame.draw();
	if(state==SelectingQuad){
		ofSetColor(255);
		photo.draw(ofGetWidth()*0.5,ofGetHeight()*0.5,videoWidth,videoHeight);
		warp.draw();
	}else{
		ofSetColor(255);
		if(imgSource) imgSource->draw(ofGetWidth()*0.5,ofGetHeight()*0.5,videoWidth,videoHeight);

		//ofxLocation location = geo->getLocation();
		//ofDrawBitmapString(ofToString(location.altitude) + " " + ofToString(location.latitude) + " " + ofToString(location.longitude) + " " + ofToString(location.bearing),20,20);
		//ofDrawBitmapString(geo->getAddress(),20,20);
		ofSetColor(255,255,255,200);
		//geo->drawLastLocationImage(20,40);
		if(geo) geoPanel.draw();
		ofPopStyle();
	}


}

TakeAPhoto::State TakeAPhoto::getState(){
	return state;
}

ofImage & TakeAPhoto::getPhoto(){
	return photo;
}

void TakeAPhoto::takePhoto(bool & pressed){
	updateState(PhotoPressed);
}

void TakeAPhoto::yesPressed(bool & pressed){
	updateState(YesPressed);
}

void TakeAPhoto::noPressed(bool & pressed){
	updateState(NoPressed);
}

void TakeAPhoto::exitPressed(bool & pressed){
	updateState(Stop);
}

void TakeAPhoto::newFrame(ofPixels & newFrame){
	if(state==TakingPhoto){
		photoPixels = newFrame;
		pixelsCopied = true;
	}
}

void TakeAPhoto::touchDoubleTap(ofTouchEventArgs & touch){
	geo->increaseZoom();
}
