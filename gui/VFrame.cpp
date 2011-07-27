/*
 * VFrame.cpp
 *
 *  Created on: 20/04/2011
 *      Author: arturo
 */

#include "VFrame.h"

namespace gui{


VFrame::VFrame()
:width(0)
,vSpacing(0)
,leftMargin(0)
,elementHeight(0)
,keepAspectRatio(true)
{

}

void VFrame::setPosition(const ofPoint & _pos){
	pos = _pos;
	repositionWidgets();
}

void VFrame::setWidth(float _width){
	width = _width;
	repositionWidgets();
}

void VFrame::setVSpacing(float _vSpacing){
	vSpacing = _vSpacing;
	repositionWidgets();
}


void VFrame::setLeftMargin(float margin){
	leftMargin = margin;
}

void VFrame::setElementHeight(float height){
	elementHeight = height;
}

void VFrame::repositionWidgets(){
	ofPoint nextPos(pos);
	nextPos.x += leftMargin;
	nextPos.y += vSpacing;
	for(int i=0;i<(int)widgets.size();i++){
		Widget * widget = widgets[i].get();

		if(keepAspectRatio){
			float ratio = widget->getAspectRatio();

			if(elementHeight>0 && elementHeight*ratio<width){
				float width = elementHeight*ratio;
				widget->setRect(ofRectangle(nextPos.x,nextPos.y,width,elementHeight));
			}else{
				float height = width/ratio;
				widget->setRect(ofRectangle(nextPos.x,nextPos.y,width,height));
			}
		}else{
			widget->setRect(ofRectangle(nextPos.x,nextPos.y,width,elementHeight>0?elementHeight:widget->getRect().height));
		}

		nextPos.y += widget->getRect().height + vSpacing;
	}
}

void VFrame::setKeepAspectRatio(bool _keepAspectRatio){
	keepAspectRatio = _keepAspectRatio;
}

}
