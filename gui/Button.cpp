/*
 * Button.cpp
 *
 *  Created on: 18/04/2011
 *      Author: arturo
 */

#include "Button.h"
#include "ofGraphics.h"

namespace gui{
Button::Button(const ofRectangle & rect)
:rect(rect)
,state(Enabled)
,refreshIcon(false)
{

}

Button::Button(const string & text,const ofColor & color)
:label(text,color)
,rect(label.getRect())
,state(Enabled)
,refreshIcon(false)
{

}

void Button::enableEvents(){
	ofRegisterMouseEvents(this);
}

void Button::disableEvents(){
	ofUnregisterMouseEvents(this);
}

void Button::update(){
	if(refreshIcon){
		icon.setUseTexture(true);
		focusedIcon.setUseTexture(true);
		pressedIcon.setUseTexture(true);
		icon.reloadTexture();
		focusedIcon.reloadTexture();
		pressedIcon.reloadTexture();
		refreshIcon = false;
	}
	label.update();
}

void Button::draw(){
	ofPushStyle();
	ofSetColor(255,255,255);
	ofNoFill();
	switch(state){
	case Focused:
		if(focusedIcon.bAllocated()){
			focusedIcon.draw(rect);
		}else if(icon.bAllocated()){
			icon.draw(rect);
		}else{
			ofRect(rect);
		}
		break;
	case Pressed:
		if(pressedIcon.bAllocated()){
			pressedIcon.draw(rect);
		}else if(icon.bAllocated()){
			icon.draw(rect);
		}else{
			ofRect(rect);
		}
		break;
	default:
		if(icon.bAllocated()){
			icon.draw(rect);
		}else{
			ofRect(rect);
		}
		break;

	}
	label.draw();
	ofPopStyle();
}

void Button::mousePressed(ofMouseEventArgs & mouse){
	if(rect.inside(ofPoint(mouse.x,mouse.y))){
		updateState(CursorPressed);
	}
}

void Button::mouseDragged(ofMouseEventArgs & mouse){
	if(rect.inside(ofPoint(mouse.x,mouse.y))){
		updateState(CursorIn);
	}else{
		updateState(CursorOut);
	}
}

void Button::mouseReleased(ofMouseEventArgs & mouse){
	updateState(CursorReleased);
}

void Button::mouseMoved(ofMouseEventArgs & mouse){
	if(rect.inside(ofPoint(mouse.x,mouse.y))){
		updateState(CursorIn);
	}else{
		updateState(CursorOut);
	}
}

void Button::updateState(Transition transition){
	//cout << "Button::updateState from state " << state << " by transition " << transition << " to state ";
	switch(transition){
	case CursorPressed:
		if(state==Enabled || state==Focused){
			state=Pressed;
		}
		break;
	case CursorReleased:
		if(state==Pressed){
			state=Enabled;
			bool pressed = true;
			ofNotifyEvent(pressedE,pressed,this);
		}else if(state==OutPressed){
			state=Enabled;
		}
		break;
	case CursorIn:
		if(state==Enabled){
			state=Focused;
		}
		break;
	case CursorOut:
		if(state==Focused){
			state=Enabled;
		}else if(state==Pressed){
			state=OutPressed;
		}
		break;
	}
	//cout << state << endl;
}

void Button::setRect(const ofRectangle & _rect){
	rect = _rect;
	label.setRect(rect);
}

ofRectangle Button::getRect(){
	return rect;
}

float Button::getAspectRatio(){
	if(icon.bAllocated()){
		return icon.getWidth()/icon.getHeight();
	}else if(label.getText()!=""){
		return label.getAspectRatio();
	}else{
		return 1;
	}
}

void Button::setIcon(ofImage & _icon){
	icon = _icon;
	rect.width = icon.getWidth();
	rect.height = icon.getHeight();
	refreshIcon = true;
}

void Button::setFocusedIcon(ofImage & icon){
	focusedIcon = icon;
	refreshIcon = true;
}

void Button::setPressedIcon(ofImage & icon){
	pressedIcon = icon;
	refreshIcon = true;
}

void Button::setText(const string & text){
	label.setText(text);
	rect = label.getRect();
}

string Button::getText(){
	return label.getText();
}

void Button::setFont(ofPtr<ofTrueTypeFont> font){
	label.setFont(font);
}

void Button::setPosition(const ofPoint & pos){
	rect.x = pos.x;
	rect.y = pos.y;
	label.setPosition(pos);
}

Button::State Button::getState(){
	return state;
}

}
