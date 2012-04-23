/*
 * ofxMD5.h
 *
 *  Created on: 02/05/2011
 *      Author: arturo
 */

#ifndef OFXMD5_H_
#define OFXMD5_H_

extern "C"{
	#include "md5-1.3.0/md5.h"
}
#include "ofConstants.h"
#include "ofFileUtils.h"


class ofxMD5 {
public:
	ofxMD5();
	virtual ~ofxMD5();

	string getMD5(const ofBuffer & buffer);
	string getMD5(ofFile & file);
	string getMD5(vector<char> & str);
	string getMD5(const string & str);
};


#endif /* OFXMD5_H_ */
