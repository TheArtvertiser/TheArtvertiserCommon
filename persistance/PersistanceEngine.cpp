/*
 * PersistanceEngine.cpp
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#include "PersistanceEngine.h"

PersistanceEngine::PersistanceEngine() {
	// TODO Auto-generated constructor stub

}

ofxXmlSettings & PersistanceEngine::artverts(){
	static ofxXmlSettings * xml = new ofxXmlSettings("artverts.xml");
	return *xml;
}

void PersistanceEngine::save(){
	artverts().saveFile();
}
