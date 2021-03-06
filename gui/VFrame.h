/*
 * VFrame.h
 *
 *  Created on: 20/04/2011
 *      Author: arturo
 */

#ifndef VFRAME_H_
#define VFRAME_H_

#include "Widget.h"
#include "Container.h"
#include "ofTypes.h"
#include "ofEvents.h"

namespace gui{
class VFrame: public Container {
public:
	VFrame();

	void setPosition(const ofPoint & pos);
	void setWidth(float width);
	void setVSpacing(float vSpacing);
	void repositionWidgets();
	void setLeftMargin(float margin);
	void setElementHeight(float height);

	void setKeepAspectRatio(bool keepAspectRatio);

private:

	ofPoint pos;
	float width;
	float vSpacing;
	float leftMargin;
	float elementHeight;
	bool keepAspectRatio;

};
}

#endif /* VFRAME_H_ */
