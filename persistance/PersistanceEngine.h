/*
 * PersistanceEngine.h
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#ifndef PERSISTANCEENGINE_H_
#define PERSISTANCEENGINE_H_

#include "ofxXmlSettings.h"

class PersistanceEngine {
private:
	PersistanceEngine();

public:
	static ofxXmlSettings & artverts();
	static void save();
};

#endif /* PERSISTANCEENGINE_H_ */
