/*
 * Geo.h
 *
 *  Created on: 28/04/2011
 *      Author: arturo
 */

#ifndef GEO_H_
#define GEO_H_

#include "ofConstants.h"
#include "ofxAndroidGPS.h"
#include "ofxXmlSettings.h"
#include "CircularPB.h"
#include "ofBaseTypes.h"

class Geo: public ofBaseDraws{
public:
	Geo();
	~Geo();

	void start();
	void stop();

	void setLocation(ofxLocation & location);

	string getAddress();
	ofxLocation getLocation();
	ofPixels getLastLocationPixels();

	void update();
	void drawLastLocationImage(float x, float y);

	void setZoom(int zoom);
	int getZoom();
	void increaseZoom();
	void decreaseZoom();

	double distance(double long1, double lat1, double long2, double lat2);
	double distance(const ofxLocation & loc1, const ofxLocation & loc2);

	void urlResponse(ofHttpResponse & response);

	ofBuffer getImageFor(const ofxLocation & location);
	string getAddressFor(const ofxLocation & location);


	// from ofBaseDraws
	void draw(float x,float y);
	void draw(float x,float y,float w, float h);

	float getHeight();
	float getWidth();

private:
	void locationChanged(ofxLocation & location);

	void queryAddress();
	void queryMap();

	ofxLocation lastLocation;
	ofxLocation prevLocation;
	string currentAddress;
	ofxXmlSettings xml;
	ofMutex mutex;
	ofImage image;
	ofImage texImage;
	bool imageChanged;
	int zoom;
	bool init;
	gui::CircularPB circularPB;
	bool bQueryMap, bQueryAddress;
};

#endif /* GEO_H_ */
