/*
 * Label.cpp
 *
 *  Created on: 29/04/2011
 *      Author: arturo
 */

#include "Label.h"
#include "ofGraphics.h"

namespace gui{

Label::Label(const string & text,const ofColor & color) {
	setText(text);
	setColor(color);
}

Label::~Label() {
	// TODO Auto-generated destructor stub
}

void Label::setPosition(const ofPoint & pos){
	position = pos;
}

void Label::setRect(const ofRectangle & rect){
	position.x = rect.x;
	position.y = rect.y;
}

ofRectangle Label::getRect(){
	return getStringBoundingBox(text,position.x,position.y);
}

float Label::getAspectRatio(){
	return getRect().width/getRect().height;
}

void Label::update(){

}

void Label::draw(){
	ofPushStyle();
	ofSetColor(color);
	drawString(text,position.x,position.y+getRect().height);
	ofPopStyle();
}

void Label::enableEvents(){

}

void Label::disableEvents(){

}

void Label::setText(const string & _text){
	text = _text;
}

string Label::getText(){
	return text;
}

void Label::setColor(const ofColor & _color){
	color = _color;
}

ofColor Label::getColor(){
	return color;
}

void Label::setFont(ofPtr<ofTrueTypeFont> font){
	ttf = font;
}

void Label::drawString(string text, float x, float y){
	if(ttf && ttf->isLoaded()){
		ttf->drawString(text,x,y);
	}else{
		ofDrawBitmapString(text,x,y);
	}
}

ofRectangle Label::getStringBoundingBox(string text, float x, float y){
	if(ttf && ttf->isLoaded()){
		return ttf->getStringBoundingBox(text,x,y);
	}else{
		return ofRectangle(x,y,text.size()*10,10);
	}
}

}
