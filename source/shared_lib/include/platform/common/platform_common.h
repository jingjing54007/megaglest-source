// ==============================================================
//	This file is part of Glest Shared Library (www.glest.org)
//
//	Copyright (C) 2005 Matthias Braun <matze@braunis.de>
//
//	You can redistribute this code and/or modify it under
//	the terms of the GNU General Public License as published
//	by the Free Software Foundation; either version 2 of the
//	License, or (at your option) any later version
// ==============================================================
#ifndef _SHARED_PLATFORM_PLATFORMCOMMON_H_
#define _SHARED_PLATFORM_PLATFORMCOMMON_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <stdexcept>
#include <list>

#include "data_types.h"
#include "checksum.h"
#include <utility>
#include <SDL.h>
#include <map>
//#include <chrono>
#include "leak_dumper.h"

#if (defined WIN32) && !(defined snprintf)
#define snprintf _snprintf
#endif

using std::string;
using std::vector;
using std::list;
using std::exception;

using Shared::Platform::int64;
using Shared::Util::Checksum;

namespace Shared { namespace PlatformCommon {

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CODE_AT_LINE __FILE__ ":" TOSTRING(__LINE__)
#define CODE_AT_LINE_X(x) __FILE__ ":" TOSTRING(__LINE__) ":" TOSTRING(x)

static const int IGNORE_CMD_RESULT_VALUE = -999999;

// keycode constants (unfortunately designed after DirectInput and therefore not
// very specific)
// They also have to fit into a char. The positive numbers seem to be equal
// to ascii, for the rest we have to find sensefull mappings from SDL (which is
// alot more fine grained like left/right control instead of just control...)
const char vkAdd = -1;
const char vkSubtract = -2;
const char vkAlt = -3;
const char vkControl = -4;
const char vkShift = -5;
const char vkEscape = -6;
const char vkUp = -7;
const char vkLeft = -8;
const char vkRight = -9;
const char vkDown = -10;
const char vkReturn = -11;
const char vkBack = -12;
const char vkTab = -13;
const char vkF1 = -14;
const char vkF2 = -15;
const char vkF3 = -16;
const char vkF4 = -17;
const char vkF5 = -18;
const char vkF6 = -19;
const char vkF7 = -20;
const char vkF8 = -21;
const char vkF9 = -22;
const char vkF10 = -23;
const char vkF11 = -24;
const char vkF12 = -25;
const char vkDelete = -26;
const char vkPrint = -27;
const char vkPause = -29;

class ShellCommandOutputCallbackInterface {
public:
	virtual ~ShellCommandOutputCallbackInterface() {}

	virtual void * getShellCommandOutput_UserData(string cmd) = 0;
    virtual void ShellCommandOutput_CallbackEvent(string cmd,char *output,void *userdata) = 0;
};

//typedef std::chrono::time_point<std::chrono::system_clock> system_time_point;
tm threadsafe_localtime(const time_t &time);
// extracting std::time_t from std:chrono for "now"
time_t systemtime_now();


// =====================================================
//	class PerformanceTimer
// =====================================================

class PerformanceTimer {
private:
	Uint32 lastTicks;
	Uint32 updateTicks;

	int times;			// number of consecutive times
	int maxTimes;		// maximum number consecutive times

public:
	void init(float fps, int maxTimes= -1);

	bool isTime();
	void reset();
};

// =====================================================
//	class Chrono
// =====================================================

class Chrono {
private:
	Uint32 startCount;
	Uint32 accumCount;
	Uint32 freq;
	bool stopped;

	Uint32 lastStartCount;
	Uint32 lastTickCount;
	int64  lastResult;
	int64 lastMultiplier;
	bool lastStopped;

public:
	Chrono(bool autoStart=false);
	void start();
	void stop();
	void reset();
	int64 getMicros();
	int64 getMillis();
	int64 getSeconds();

	bool isStarted() const;
    static int64 getCurTicks();
    static int64 getCurMillis();

private:
	int64 queryCounter(int64 multiplier);
};

// =====================================================
//	class ModeInfo
// =====================================================
class ModeInfo {
public:
	int width;
	int height;
	int depth;

	ModeInfo(int width, int height, int depth);

	bool operator< (const ModeInfo &j) const {
		if(this->width < j.width) {
			return true;
		}
		else if(this->width == j.width && this->height < j.height) {
			return true;
		}
		else if(this->width == j.width &&
				this->height == j.height &&
				this->depth < j.depth) {
			return true;
		}

		return false;
	}
	string getString() const;
};

// =====================================================
//	Misc
// =====================================================
void Tokenize(const string& str,vector<string>& tokens,const string& delimiters = " ");
bool isdir(const char *path);
bool fileExists(const string &path);
inline bool folderExists(const string &path) { return isdir(path.c_str()); }

void findDirs(string path, vector<string> &results, bool errorOnNotFound,bool keepDuplicates);
void findDirs(const vector<string> &paths, vector<string> &results, bool errorOnNotFound=false,bool keepDuplicates=false);
void findAll(const vector<string> &paths, const string &fileFilter, vector<string> &results, bool cutExtension=false, bool errorOnNotFound=true,bool keepDuplicates=false);
void findAll(const string &path, vector<string> &results, bool cutExtension=false, bool errorOnNotFound=true);
vector<string> getFolderTreeContentsListRecursively(const string &path, const string &filterFileExt, bool includeFolders=false, vector<string> *recursiveMap=NULL);

string getGameVersion();
string getGameGITVersion();
void setGameVersion(string version);
void setGameGITVersion(string git);

string getCRCCacheFilePath();
void setCRCCacheFilePath(string path);

std::pair<string,string> getFolderTreeContentsCheckSumCacheKey(vector<string> paths, string pathSearchString, const string &filterFileExt);
void clearFolderTreeContentsCheckSum(vector<string> paths, string pathSearchString, const string &filterFileExt);
uint32 getFolderTreeContentsCheckSumRecursively(vector<string> paths, string pathSearchString, const string &filterFileExt, Checksum *recursiveChecksum,bool forceNoCache=false);
time_t getFolderTreeContentsCheckSumRecursivelyLastGenerated(vector<string> paths, string pathSearchString, const string &filterFileExt);

std::pair<string,string> getFolderTreeContentsCheckSumCacheKey(const string &path, const string &filterFileExt);
void clearFolderTreeContentsCheckSum(const string &path, const string &filterFileExt);
uint32 getFolderTreeContentsCheckSumRecursively(const string &path, const string &filterFileExt, Checksum *recursiveChecksum,bool forceNoCache=false);

std::pair<string,string> getFolderTreeContentsCheckSumListCacheKey(vector<string> paths, string pathSearchString, const string &filterFileExt);
void clearFolderTreeContentsCheckSumList(vector<string> paths, string pathSearchString, const string &filterFileExt);
vector<std::pair<string,uint32> > getFolderTreeContentsCheckSumListRecursively(vector<string> paths, string pathSearchString, const string &filterFileExt, vector<std::pair<string,uint32> > *recursiveMap);

std::pair<string,string> getFolderTreeContentsCheckSumListCacheKey(const string &path, const string &filterFileExt);
void clearFolderTreeContentsCheckSumList(const string &path, const string &filterFileExt);
vector<std::pair<string,uint32> > getFolderTreeContentsCheckSumListRecursively(const string &path, const string &filterFileExt, vector<std::pair<string,uint32> > *recursiveMap);

void createDirectoryPaths(string  Path);
string extractFileFromDirectoryPath(string filename);
string extractDirectoryPathFromFile(string filename);
string extractLastDirectoryFromPath(string Path);
string extractExtension(const string& filename);

void getFullscreenVideoModes(vector<ModeInfo> *modeinfos,bool isFullscreen);
void getFullscreenVideoInfo(int &colorBits,int &screenWidth,int &screenHeight,bool isFullscreen);
void changeVideoModeFullScreen(bool value);
void restoreVideoMode(SDL_Window *sdlWindow,bool exitingApp=false);

bool StartsWith(const std::string &str, const std::string &key);
bool EndsWith(const string &str, const string& key);

void endPathWithSlash(string &path, bool requireOSSlash=false);
void trimPathWithStartingSlash(string &path);
void updatePathClimbingParts(string &path,bool processPreviousDirTokenCheck=true);
string formatPath(string path);

string replaceAllHTMLEntities(string& context);
string replaceAll(string& context, const string& from, const string& to);
vector<char> replaceAllBetweenTokens(vector<char>& context, const string &startToken, const string &endToken, const string &newText, bool removeTokens=true);
string replaceAllBetweenTokens(string& context, const string &startToken, const string &endToken, const string &newText, bool removeTokens=true);
bool removeFile(string file);
bool renameFile(string oldFile, string newFile);
void removeFolder(const string &path);
off_t getFileSize(string filename);
bool searchAndReplaceTextInFile(string fileName, string findText, string replaceText, bool simulateOnly);
void copyFileTo(string fromFileName, string toFileName);

//int getScreenW();
//int getScreenH();

void sleep(int millis);

bool isCursorShowing();
void showCursor(bool b);
bool isKeyDown(int virtualKey);
//bool isKeyDown(SDLKey key);
string getCommandLine();

string getUserHome();

#define SPACES " "

inline string trim_at_delim (const string & s, const string &t)  {
  string d (s);
  string::size_type i(d.find(t));
  //printf("Searching for [%s] in [%s] got " MG_SIZE_T_SPECIFIER "\n",t.c_str(),d.c_str(),i);

  if (i == string::npos) {
    return d;
  }
  else {
	  d = d.erase (i) ;
	  //printf("returning [%s]\n",d.c_str());
	  return d;
  }
}

inline string trim_right (const string & s, const string & t = SPACES)  {
  string d (s);
  string::size_type i (d.find_last_not_of (t));
  if (i == string::npos)
    return "";
  else
   return d.erase (d.find_last_not_of (t) + 1) ;
}  // end of trim_right

inline string trim_left (const string & s, const string & t = SPACES) {
  string d (s);
  return d.erase (0, s.find_first_not_of (t)) ;
}  // end of trim_left

inline string trim (const string & s, const string & t = SPACES) {
  string d (s);
  return trim_left (trim_right (d, t), t) ;
}  // end of trim

string getFullFileArchiveExtractCommand(string fileArchiveExtractCommand,
		string fileArchiveExtractCommandParameters, string outputpath, string archivename);
string getFullFileArchiveCompressCommand(string fileArchiveCompressCommand,
		string fileArchiveCompressCommandParameters, string archivename, string archivefiles);

bool executeShellCommand(string cmd,int expectedResult=IGNORE_CMD_RESULT_VALUE,ShellCommandOutputCallbackInterface *cb=NULL);
string executable_path(string exeName,bool includeExeNameInPath=false);

void saveDataToFile(string filename, string data);

bool valid_utf8_file(const char* file_name);

string getFileTextContents(string path);

string safeCharPtrCopy(const char *ptr, int maxLength=-1);

class ValueCheckerVault {

protected:
	std::map<const void *,uint32> vaultList;

	void addItemToVault(const void *ptr,int value);
	void checkItemInVault(const void *ptr,int value) const;

public:

	ValueCheckerVault() {
		vaultList.clear();
	}
};


}}//end namespace

#endif
