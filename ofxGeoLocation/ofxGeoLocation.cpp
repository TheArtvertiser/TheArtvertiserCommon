/*
 * Geo.cpp
 *
 *  Created on: 28/04/2011
 *      Author: arturo
 */

#include "ofxGeoLocation.h"
#include "Poco/ScopedLock.h"
#ifdef TARGET_ANDROID
#include "ofxAndroidGPS.h"
#endif

ofxGeoLocation::ofxGeoLocation()
:imageChanged(false)
,zoom(15)
,init(true)
,width(320)
,height(240)
,started(false)
{
	image.setUseTexture(false);
	texImage.loadImage("geo/blank.png");
	circularPB.setRadius(20);
	ofRegisterURLNotification(this);
}

ofxGeoLocation::~ofxGeoLocation() {

}


void ofxGeoLocation::start(){
#ifdef TARGET_ANDROID
	ofAddListener(ofxAndroidGPS::locationChangedE,this,&ofxGeoLocation::locationChanged);
	ofxAndroidGPS::startGPS();
#endif
	queryMap();
	queryAddress();
	started = true;
}

void ofxGeoLocation::stop(){
#ifdef TARGET_ANDROID
	ofxAndroidGPS::stopGPS();
	ofRemoveListener(ofxAndroidGPS::locationChangedE,this,&ofxGeoLocation::locationChanged);
#endif
	image.clear();
	image.setUseTexture(false);
	image.loadImage("geo/blank.png");
	imageChanged = true;
	ofRemoveAllURLRequests();
}


bool ofxGeoLocation::isStarted(){
	return started;
}

void ofxGeoLocation::setLocation(ofxLocation & location){
	Poco::ScopedLock<ofMutex> slock(mutex);
	lastLocation = prevLocation = location;
	currentAddress = "";
	currentCity = "";
	currentCountry = "";
	image.clear();
	image.setUseTexture(false);
	image.loadImage("geo/blank.png");
	imageChanged = true;
	queryAddress();
	queryMap();
}

string ofxGeoLocation::getAddress(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return currentAddress;
}

string ofxGeoLocation::getCountry(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return currentCountry;

}

string ofxGeoLocation::getCity(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return currentCity;
}

ofxLocation ofxGeoLocation::getLocation(){
	return lastLocation;
}

ofPixels ofxGeoLocation::getLastLocationPixels(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return image.getPixelsRef();
}

void ofxGeoLocation::urlResponse(ofHttpResponse & response){
	if(response.request.name=="ofxGeo.Map"){
		if(response.status==200){
			ofLog(OF_LOG_VERBOSE, "got map");
			mutex.lock();
			image.loadImage(response.data);
			imageChanged = true;
			mutex.unlock();
		}else{
			ofLog(OF_LOG_ERROR,"%d %s",response.status,response.error.c_str());
		}
		bQueryMap = false;
	}

	if(response.request.name=="ofxGeo.Address"){
		ofLog(OF_LOG_VERBOSE, "got address");
		xml.loadFromBuffer(response.data.getText());
		string road = xml.getValue("reversegeocode:addressparts:road","");
		string city = xml.getValue("reversegeocode:addressparts:city","");
		string country = xml.getValue("reversegeocode:addressparts:country","");

		mutex.lock();
		currentAddress = road;
		currentCity = city;
		currentCountry = country;
		mutex.unlock();
		bQueryAddress = false;
	}
}

void ofxGeoLocation::queryAddress(){
	urlLoader.getAsync("http://nominatim.openstreetmap.org/reverse?lat="+ofToString(lastLocation.latitude) + "&lon=" +ofToString(lastLocation.longitude)+"&zoom=0","ofxGeo.Address");
	bQueryAddress = true;
}

void ofxGeoLocation::queryMap(){
	urlLoader.getAsync("http://pafciu17.dev.openstreetmap.org/?module=map&center="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+"&zoom="+ofToString(zoom)+"&type=mapnik&width="+ofToString(width)+"&height="+ofToString(height)+"&points="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+",pointImagePattern:greenP","ofxGeo.Map");
	bQueryMap = true;
}


void ofxGeoLocation::locationChanged(ofxLocation & _location){
	lastLocation = _location;
	if(init || distance(lastLocation,prevLocation)>0.01){
		init = false;
		queryAddress();
		queryMap();
		prevLocation = lastLocation;
	}
}

void ofxGeoLocation::update(){
	mutex.lock();
	if(imageChanged){
		texImage = image;
		imageChanged = false;
		mutex.unlock();
		texImage.setUseTexture(true);
		texImage.reloadTexture();
	}else{
		mutex.unlock();
	}

	if(!texImage.bAllocated() || bQueryMap){
		circularPB.update();
	}
}

void ofxGeoLocation::drawLastLocationImage(float x, float y){
	if(texImage.bAllocated()){
		texImage.draw(x,y,width,height);
	}
	if(!texImage.bAllocated() || bQueryMap){
		circularPB.setPosition(ofPoint(x+width*0.5,y+height*0.5));
		circularPB.draw();
	}
}

void ofxGeoLocation::setZoom(int _zoom){
	zoom = _zoom;
	queryMap();
}

int ofxGeoLocation::getZoom(){
	return zoom;
}

void ofxGeoLocation::increaseZoom(){
	zoom++;
	queryMap();
}

void ofxGeoLocation::decreaseZoom(){
	zoom--;
	queryMap();
}


double ofxGeoLocation::distance(double lon1, double lat1, double lon2, double lat2){
	// http://www.movable-type.co.uk/scripts/latlong.html
	static double R = 6371; // earth radius in km
	return acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2) * cos(lon2-lon1)) * R;
}

double ofxGeoLocation::distance(const ofxLocation & loc1, const ofxLocation & loc2){
	return distance(loc1.longitude, loc1.latitude, loc2.longitude, loc2.latitude);
}


void ofxGeoLocation::draw(float x,float y){
	drawLastLocationImage(x,y);
}
void ofxGeoLocation::draw(float x,float y,float w, float h){
	drawLastLocationImage(x,y);
}

float ofxGeoLocation::getHeight(){
	return height;
}

float ofxGeoLocation::getWidth(){
	return width;
}

void ofxGeoLocation::setSize(float _width, float _height){
	width = _width;
	height = _height;
}

ofBuffer ofxGeoLocation::getImageFor(const ofxLocation & location){
	return ofLoadURL("http://pafciu17.dev.openstreetmap.org/?module=map&center="+ofToString(location.longitude)+","+ofToString(location.latitude)+"&zoom="+ofToString(zoom)+"&type=mapnik&width="+ofToString(width)+"&height="+ofToString(height)+"&points="+ofToString(location.longitude)+","+ofToString(location.latitude)+",pointImagePattern:greenP").data;
}

ofxXmlSettings ofxGeoLocation::getAddressFor(const ofxLocation & location){
	ofxXmlSettings xml;
	xml.loadFromBuffer(ofLoadURL("http://nominatim.openstreetmap.org/reverse?lat="+ofToString(location.latitude) + "&lon=" +ofToString(location.longitude)+"&zoom=0").data.getText());
	return xml;
}
