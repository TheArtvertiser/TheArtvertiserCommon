/*
 * ofxMD5.cpp
 *
 *  Created on: 02/05/2011
 *      Author: arturo
 */

#include "ofxMD5/ofxMD5.h"

/*
 * Print a signature
 */
static	void	print_sig(const unsigned char *sig)
{
  const unsigned char	*sig_p;

  for (sig_p = sig; sig_p < sig + MD5_SIZE; sig_p++) {
    (void)printf("%02x", *sig_p);
  }
}

ofxMD5::ofxMD5() {
	// TODO Auto-generated constructor stub

}

ofxMD5::~ofxMD5() {
	// TODO Auto-generated destructor stub
}

string ofxMD5::getMD5(const ofBuffer & buffer){
	md5_t		md5;
	char		aux_buffer[1024];
	unsigned char signature[MD5_SIZE];

	md5_init(&md5);
	md5_process(&md5, buffer.getBinaryBuffer(), buffer.size());
	md5_finish(&md5, signature);

	memset(aux_buffer,0,sizeof(aux_buffer));
	md5_sig_to_string(signature, aux_buffer, sizeof(aux_buffer));

	return aux_buffer;
}

string ofxMD5::getMD5(ofFile & file){
	md5_t		md5;
	char		aux_buffer[4096];

	unsigned char signature[MD5_SIZE];

	file.changeMode(ofFile::ReadOnly);
	file.seekg(0,ios_base::beg);

	md5_init(&md5);

	file.read(aux_buffer, 4096);
	std::streamsize n = file.gcount();
	while (n > 0){
		// we resize to size+1 initialized to 0 to have a 0 at the end for strings
		md5_process(&md5, aux_buffer, n);
		if (file){
			file.read(aux_buffer, 4096);
			n = file.gcount();
		}
		else n = 0;
	}

	md5_finish(&md5, signature);

	file.seekg(0,ios_base::beg);

	memset(aux_buffer,0,sizeof(aux_buffer));
	md5_sig_to_string(signature, aux_buffer, sizeof(aux_buffer));

	return aux_buffer;
}

string ofxMD5::getMD5(const string & str){
	md5_t		md5;
	char		aux_buffer[1024];

	unsigned char signature[MD5_SIZE];

	md5_init(&md5);
	md5_process(&md5, str.c_str(), str.size());
	md5_finish(&md5, signature);

	memset(aux_buffer,0,sizeof(aux_buffer));
	md5_sig_to_string(signature, aux_buffer, sizeof(aux_buffer));

	return aux_buffer;
}

string ofxMD5::getMD5(vector<char> & str){
	md5_t		md5;
	char		aux_buffer[1024];
	unsigned char signature[MD5_SIZE];

	md5_init(&md5);
	md5_process(&md5, &str[0], str.size());
	md5_finish(&md5, signature);

	memset(aux_buffer,0,sizeof(aux_buffer));
	md5_sig_to_string(signature, aux_buffer, sizeof(aux_buffer));

	return aux_buffer;

}

