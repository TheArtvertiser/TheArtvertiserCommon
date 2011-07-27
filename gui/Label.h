/*
 * Label.h
 *
 *  Created on: 29/04/2011
 *      Author: arturo
 */

#ifndef LABEL_H_
#define LABEL_H_

#include "ofRectangle.h"
#include "ofPoint.h"
#include "ofColor.h"
#include "ofTrueTypeFont.h"
#include "ofTypes.h"

#include "Widget.h"


namespace gui{

class Label: public Widget {
public:
	Label(const string & text="",const ofColor & color=ofColor(0,0,0));
	~Label();

	void setPosition(const ofPoint & pos);
	void setRect(const ofRectangle & rect);
	ofRectangle getRect();
	float getAspectRatio();

	void update();
	void draw();

	void enableEvents();
	void disableEvents();

	void setText(const string & text);
	string getText();

	void setColor(const ofColor & color);
	ofColor getColor();

	void setFont(ofPtr<ofTrueTypeFont> font);

private:
	void drawString(string text, float x, float y);
	ofRectangle getStringBoundingBox(string text, float x, float y);

	ofPoint position;
	string text;
	ofColor color;

	ofPtr<ofTrueTypeFont> ttf;
};

}
#endif /* LABEL_H_ */
