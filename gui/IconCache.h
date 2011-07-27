/*
 * IconCache.h
 *
 *  Created on: 22/04/2011
 *      Author: arturo
 */

#ifndef ICONCACHE_H_
#define ICONCACHE_H_

#include <list>
#include <map>

#include "ofImage.h"
#include "ofTrueTypeFont.h"

#include "ofxThreadedImageLoader.h"

namespace gui{
class IconCache {
public:

	ofPtr<ofImage> loadImage(string filename);
	ofPtr<ofImage> loadImage(ofFile file);

	ofPtr<ofImage> getResource(string name);
	void storeResource(ofPtr<ofImage> image, string name);

	ofPtr<ofTrueTypeFont> getFont(string path,int size);

private:
	list<ofPtr<ofImage> > icons;
	map<string,list<ofPtr<ofImage> >::iterator > icons_index;

	list<ofPtr<ofImage> > resources;
	map<string,list<ofPtr<ofImage> >::iterator > resources_index;

	ofxThreadedImageLoader loader;

	list<ofPtr<ofTrueTypeFont> > fonts;
	map<string,list<ofPtr<ofTrueTypeFont> >::iterator > fonts_index;
};
}

#endif /* ICONCACHE_H_ */
