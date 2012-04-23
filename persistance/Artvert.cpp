/*
 * Artvert.cpp
 *
 *  Created on: 19/04/2011
 *      Author: arturo
 */

#include "Artvert.h"
#include "PersistanceEngine.h"


Artvert::Artvert()
:folder(ofFilePath::addTrailingSlash(DEFAULT_ARTVERT_FOLDER))
{

}

Artvert::Artvert(string _uid,string folder)
:uid(_uid)
,folder(ofFilePath::addTrailingSlash(folder))
,compressedImage(folder + uid+".jpg")
,model(folder + uid+".bmp")
,roiPath(folder + uid+".bmp.roi")
,detectorData(folder + uid+".bmp.detector_data")
,trackerData(folder + uid+".bmp.tracker_data")
,locationData(folder + uid+".bmp.location")
,md5Path(folder + uid + ".bmp.md5")
//be careful when adding more files (update set also)
,roi(4)
{
	ofFile roiFile(roiPath);
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
	}

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");
	
	ofLogVerbose("Artvert", "uid "+uid+": loading "+ofToString(numAlias) );

	for(int i=0; i<numAlias; i++)
	{
		if(uid==xml.getAttribute("artvert","uid","",i))
		{
			setAliasUID(xml.getAttribute("artvert","alias","",i));
			advertName = xml.getAttribute( "artvert", "advertName", advertName, i );
			break;
		}
		
	}
	
	updateArtworks();

	
}

void Artvert::updateArtworks()
{
	
	ofLogVerbose("Artvert", "uid "+uid+": found advertName "+advertName );
	
	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");
	
	for(int i=0; i<numAlias; i++){
		if(uid==xml.getAttribute("artvert","uid","",i)){
			xml.pushTag("artvert",i);
			int numArtworks = xml.getNumTags("artwork");
			for ( int j=0; j<numArtworks; j++ )
			{
				xml.pushTag("artwork", j );
				string path = xml.getValue( "file", "" );
				ofFile file(path);
				title[file] = xml.getValue( "title", "untitled" );
				artist[file] = xml.getValue( "artist", "unknown" );
				ofLogVerbose( "Artvert", ""+path+": "+title[file]+" by "+artist[file] );
				
				xml.popTag();
			}
			xml.popTag();
			break;
		}
	}
	
	vector<ofFile> artworkFiles = getArtverts();
	for ( int i=0; i<artworkFiles.size(); i++ )
	{
		if ( title.find( artworkFiles[i] ) == title.end() )
		{
			ofLog( OF_LOG_WARNING, "artvert %s: can't file artwork entry for artvert file %s, assuming default", 
				  uid.c_str(), artworkFiles[i].path().c_str() );
		}
	}
	

}

string Artvert::getUID() const{
	return uid;
}

void Artvert::setUID(const string & _uid){
	uid = _uid;
	compressedImage = folder + uid+".jpg";
	model = folder + uid+".bmp";
	roiPath = folder + uid+".bmp.roi";

	ofFile roiFile(roiPath);
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
	}
	detectorData = folder + uid+".bmp.detector_data";
	trackerData = folder + uid+".bmp.tracker_data";
	locationData = folder + uid+".bmp.location";
	md5Path = folder + uid + ".bmp.md5";

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");

	for(int i=0; i<numAlias; i++){
		if(uid==xml.getAttribute("artvert","uid","",i)){
			setAliasUID(xml.getAttribute("artvert","alias","",i));
			advertName = xml.getAttribute( "artvert", "advertName", advertName, i );
			
			break;
			
		}
	}
	
	updateArtworks();
	
}


string Artvert::getAdvertName(){
	return advertName;
}

void Artvert::setAdvertName(const string & _name){
	advertName = _name;
}

string Artvert::getAliasUID() const{
	return aliasUID;
}

void Artvert::setAliasUID(const string & uid){
	aliasUID = uid;
}

Artvert Artvert::getAlias(){
	return Artvert(aliasUID,folder);
}

bool Artvert::hasAlias() const{
	return aliasUID!="";
}

bool Artvert::isReady(){
	bool ret = uid!="" && ofFile(model,ofFile::Reference).exists() &&
			ofFile(roiPath,ofFile::Reference).exists() &&
			ofFile(detectorData,ofFile::Reference).exists() &&
			ofFile(trackerData,ofFile::Reference).exists();

	if(ret){
		return true;
	}else if(hasAlias()){
		return getAlias().isReady();
	}else{
		return false;
	}
}

ofFile Artvert::getMD5File() const{
	return ofFile(md5Path);
}

ofFile Artvert::getLocationFile() const{
	return ofFile(locationData);
}

ofxLocation Artvert::getLocation(){
	ofxLocation location = {0,0,0,0,0};
	ofFile locationFile(locationData,ofFile::Reference);
	if(locationFile.exists()){
		locationFile >> location;
	}else{
		ofLog(OF_LOG_ERROR,"Artvert: error trying to open location file " + locationFile.getAbsolutePath());
	}

	return location;
}

ofFile Artvert::getCompressedImage() const{
	return ofFile(compressedImage);
}

ofFile Artvert::getModel() const{
	return ofFile(model);
}

ofFile Artvert::getROIFile() const{
	return ofFile(roiPath);
}

ofFile Artvert::getDetectorData() const{
	return ofFile(detectorData);
}

ofFile Artvert::getTrackerData() const{
	return ofFile(trackerData);
}

vector<ofPoint> Artvert::getROI(){
	if(!roi.empty()) return roi;

	ofFile roiFile(roiPath);
	if(roiFile.exists()){
		roi.resize(4);
		for(int i=0;i<4;i++){
			roiFile >> roi[i].x >> roi[i].y;
		}
		return roi;
	}

	return vector<ofPoint>(4);

}

vector<ofFile> Artvert::getArtverts(){
	ofDirectory default_artverts_dir("artverts/");
	default_artverts_dir.allowExt("jpg");
	default_artverts_dir.allowExt("png");
	default_artverts_dir.allowExt("bmp");
	default_artverts_dir.listDir();
	vector<ofFile> artverts = default_artverts_dir.getFiles();

	ofDirectory artverts_dir("artverts/" + getUID());
	artverts_dir.allowExt("jpg");
	artverts_dir.allowExt("png");
	artverts_dir.listDir();
	vector<ofFile> own_artverts = artverts_dir.getFiles();
	artverts.insert(artverts.end(),own_artverts.begin(),own_artverts.end());

	return artverts;
}

void Artvert::save(){
	if(aliasUID!="")
	{
		ofLogWarning("Artvert","artvert "+uid+" not saving: has aliasUID");
		return;
	}
	int artvert = PersistanceEngine::artverts().addTag("artvert");
	PersistanceEngine::artverts().addAttribute("artvert","uid",uid,artvert);
	PersistanceEngine::artverts().addAttribute("artvert","alias",aliasUID,artvert);
	PersistanceEngine::artverts().addAttribute("artvert","advertName", advertName, artvert );
	PersistanceEngine::save();
	ofLogNotice("Artvert","artvert "+uid+" saved");
}

vector<Artvert> Artvert::listAll(string folder){
	cout << "listing: " <<  folder << endl;
	ofDirectory dir;
	dir.allowExt("jpg");
	int numArtverts = dir.listDir(folder);
	cout << "numArtverts: " << numArtverts << endl;
	vector<Artvert> artverts(numArtverts,Artvert("",folder));
	for(int i=0;i<numArtverts;i++){
		string uid = dir.getFile(i,ofFile::Reference).getBaseName();
		artverts[i].setUID(uid);
	}

	ofxXmlSettings & xml = PersistanceEngine::artverts();
	int numAlias = xml.getNumTags("artvert");

	for(int i=0; i<numAlias; i++){
		for(int j=0;j<numArtverts;j++){
			if(artverts[j].getUID()==xml.getAttribute("artvert","uid","",i)){
				artverts[j].setAliasUID(xml.getAttribute("artvert","alias","",i));
				break;
			}
		}
	}

	return artverts;
}

bool Artvert::operator==(const Artvert & artvert) const{
	return getUID()==artvert.getUID();
}

bool Artvert::operator!=(const Artvert & artvert) const{
	return getUID()!=artvert.getUID();
}

bool Artvert::operator<(const Artvert & artvert) const{
	return getUID()<artvert.getUID();
}

bool Artvert::operator<=(const Artvert & artvert) const{
	return getUID()<=artvert.getUID();
}

bool Artvert::operator>(const Artvert & artvert) const{
	return getUID()>artvert.getUID();
}

bool Artvert::operator>=(const Artvert & artvert) const{
	return getUID()>=artvert.getUID();
}

bool Artvert::checkIntegrity(){
	if( !ofFile(md5Path,ofFile::Reference).exists() ||
			!ofFile(compressedImage,ofFile::Reference).exists() ||
			!ofFile(roiPath,ofFile::Reference).exists() ||
			!ofFile(locationData,ofFile::Reference).exists() ){

		ofLogWarning("Artvert") << "basic files don't exist";
		return false;
	}else if(isReady() &&
				(!ofFile(detectorData,ofFile::Reference).exists() || !ofFile(trackerData,ofFile::Reference).exists()) ){

		ofLogWarning("Artvert") << "detector files don't exist";
		return false;
	}else{
		string storedmd5 = getStoredMD5();
		string testmd5 = generateMD5();

		return storedmd5 == testmd5;
	}
}

string Artvert::getStoredMD5(){
	string md5;
	ofFile md5File(md5Path);
	md5File >> md5;
	return md5;
}

string Artvert::generateMD5(){
	string testmd5;
	ofxMD5 md5;
	vector<char> filesSum;
	ofBuffer files;

	ofFile compressedImageFile(compressedImage);
	compressedImageFile >> files;
	compressedImageFile.close();
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	ofFile roiFile(roiPath);
	roiFile >> files;
	roiFile.close();
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	ofFile locationFile(locationData);
	locationFile >> files;
	locationFile.close();
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	if(!isReady()){
		testmd5 = md5.getMD5(filesSum);
		return testmd5;
	}

	ofFile detectorFile(detectorData);
	detectorFile >> files;
	detectorFile.close();
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	ofFile trackerFile(trackerData);
	trackerFile >> files;
	filesSum.insert(filesSum.end(),files.getBinaryBuffer(), files.getBinaryBuffer()+files.size());

	testmd5 = md5.getMD5(filesSum);
	return testmd5;
}

void Artvert::remove(){
	ofFile(compressedImage,ofFile::Reference).remove();
	ofFile(roiPath,ofFile::Reference).remove();
	ofFile(locationData,ofFile::Reference).remove();
	ofFile(detectorData,ofFile::Reference).remove();
	ofFile(trackerData,ofFile::Reference).remove();
	ofFile(model,ofFile::Reference).remove();
	ofFile(md5Path,ofFile::Reference).remove();
	//TODO: remove data from xml
}

void Artvert::removeAnalisys(){
	ofFile(detectorData,ofFile::Reference).remove();
	ofFile(trackerData,ofFile::Reference).remove();
	ofFile(md5Path,ofFile::Reference).remove();
	ofFile md5(md5Path,ofFile::WriteOnly);
	md5 << generateMD5();
	md5.close();

}
