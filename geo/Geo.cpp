/*
 * Geo.cpp
 *
 *  Created on: 28/04/2011
 *      Author: arturo
 */

#include "Geo.h"
#include "Poco/ScopedLock.h"

Geo::Geo()
:imageChanged(false)
,zoom(15)
,init(true)
{
	image.setUseTexture(false);
	texImage.loadImage("geo/blank.png");
	circularPB.setRadius(20);
	ofRegisterURLNotification(this);
}

Geo::~Geo() {

}


void Geo::start(){
	ofAddListener(ofxAndroidGPS::locationChangedE,this,&Geo::locationChanged);
	ofxAndroidGPS::startGPS();
	queryMap();
	queryAddress();
}

void Geo::stop(){
	ofxAndroidGPS::stopGPS();
	ofRemoveListener(ofxAndroidGPS::locationChangedE,this,&Geo::locationChanged);
	image.clear();
	image.setUseTexture(false);
	image.loadImage("geo/blank.png");
	imageChanged = true;
	ofRemoveAllURLRequests();
}

void Geo::setLocation(ofxLocation & location){
	Poco::ScopedLock<ofMutex> slock(mutex);
	lastLocation = prevLocation = location;
	currentAddress = "";
	image.clear();
	image.setUseTexture(false);
	image.loadImage("geo/blank.png");
	imageChanged = true;
	queryAddress();
	queryMap();
}

string Geo::getAddress(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return currentAddress;
}

ofxLocation Geo::getLocation(){
	return lastLocation;
}

ofPixels Geo::getLastLocationPixels(){
	Poco::ScopedLock<ofMutex> slock(mutex);
	return image.getPixelsRef();
}

void Geo::urlResponse(ofHttpResponse & response){
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
		currentAddress = road + ", " + city + " (" + country +")";
		mutex.unlock();
		bQueryAddress = false;
	}
}

void Geo::queryAddress(){
	ofLoadURLAsync("http://nominatim.openstreetmap.org/reverse?lat="+ofToString(lastLocation.latitude) + "&lon=" +ofToString(lastLocation.longitude)+"&zoom=0","ofxGeo.Address");
	bQueryAddress = true;
}

void Geo::queryMap(){
	ofLoadURLAsync("http://pafciu17.dev.openstreetmap.org/?module=map&center="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+"&zoom="+ofToString(zoom)+"&type=mapnik&width=320&height=240&points="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+",pointImagePattern:greenP","ofxGeo.Map");
	bQueryMap = true;
}


void Geo::locationChanged(ofxLocation & _location){
	lastLocation = _location;
	if(init || distance(lastLocation,prevLocation)>0.01){
		init = false;
		queryAddress();
		queryMap();
		prevLocation = lastLocation;
	}
}

void Geo::update(){
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

void Geo::drawLastLocationImage(float x, float y){
	if(texImage.bAllocated()){
		texImage.draw(x,y);
	}
	if(!texImage.bAllocated() || bQueryMap){
		circularPB.setPosition(ofPoint(x+320*0.5,y+240*0.5));
		circularPB.draw();
	}
}

void Geo::setZoom(int _zoom){
	zoom = _zoom;
	queryMap();
}

int Geo::getZoom(){
	return zoom;
}

void Geo::increaseZoom(){
	zoom++;
	queryMap();
}

void Geo::decreaseZoom(){
	zoom--;
	queryMap();
}


double Geo::distance(double lon1, double lat1, double lon2, double lat2){
	// http://www.movable-type.co.uk/scripts/latlong.html
	static double R = 6371; // earth radius in km
	return acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2) * cos(lon2-lon1)) * R;
}

double Geo::distance(const ofxLocation & loc1, const ofxLocation & loc2){
	return distance(loc1.longitude, loc1.latitude, loc2.longitude, loc2.latitude);
}


void Geo::draw(float x,float y){
	drawLastLocationImage(x,y);
}
void Geo::draw(float x,float y,float w, float h){
	drawLastLocationImage(x,y);
}

float Geo::getHeight(){
	return 240;
}

float Geo::getWidth(){
	return 320;
}

ofBuffer Geo::getImageFor(const ofxLocation & location){
	return ofLoadURL("http://pafciu17.dev.openstreetmap.org/?module=map&center="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+"&zoom="+ofToString(zoom)+"&type=mapnik&width=320&height=240&points="+ofToString(lastLocation.longitude)+","+ofToString(lastLocation.latitude)+",pointImagePattern:greenP").data;
}

string Geo::getAddressFor(const ofxLocation & location){
	return ofLoadURL("http://nominatim.openstreetmap.org/reverse?lat="+ofToString(lastLocation.latitude) + "&lon=" +ofToString(lastLocation.longitude)+"&zoom=0").data;
}
