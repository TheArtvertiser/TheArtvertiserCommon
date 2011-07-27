/*
 * Button.h
 *
 *  Created on: 18/04/2011
 *      Author: arturo
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "ofEvents.h"
#include "ofRectangle.h"
#include "ofPoint.h"
#include "ofImage.h"
#include "Widget.h"
#include "Label.h"
#include "ofColor.h"

namespace gui{
class Button: public Widget {
public:
	Button( const ofRectangle & rect=ofRectangle() );
	Button( const string & text,const ofColor & color=ofColor(0,0,0) );
	~Button(){ disableEvents(); }

	void setRect(const ofRectangle & rect);
	ofRectangle getRect();
	void setPosition(const ofPoint & pos);

	void setIcon(ofImage & icon);
	void setText(const string & text);
	void setFont(ofPtr<ofTrueTypeFont> font);
	void setFocusedIcon(ofImage & icon);
	void setPressedIcon(ofImage & icon);

	string getText();

	float getAspectRatio();

	void enableEvents();
	void disableEvents();

	void update();
	void draw();

	void mousePressed(ofMouseEventArgs & mouse);
	void mouseDragged(ofMouseEventArgs & mouse);
	void mouseReleased(ofMouseEventArgs & mouse);
	void mouseMoved(ofMouseEventArgs & mouse);

	ofEvent<bool> pressedE;

	enum State{
		Disabled,
		Enabled,
		Focused,
		Pressed,
		OutPressed
	};

	State getState();
private:
	enum Transition{
		CursorPressed,
		CursorReleased,
		CursorOut,
		CursorIn
	};

	void updateState(Transition transition);

	Label label;
	ofImage icon, focusedIcon, pressedIcon;
	ofRectangle rect;
	State state;
	bool refreshIcon;
};
}

#endif /* BUTTON_H_ */
