/* See other files here for the LICENCE that applies here. */
/*
Utils provides various utilities and general-purpose functions that
we find helpful in coding this project.
*/

#ifndef INCLUDE_OT_NEWCLI_UTILS
#define INCLUDE_OT_NEWCLI_UTILS

#include "lib_common1.hpp"
#ifdef __unix
	#include <unistd.h>
#endif

#ifndef CFG_WITH_TERMCOLORS
	#error "You requested to turn off terminal colors (CFG_WITH_TERMCOLORS), however currently they are hardcoded (this option to turn them off is not yet implemented)."
#endif

#define MAKE_CLASS_NAME(NAME) private: static std::string GetObjectName() { return #NAME; } 
#define MAKE_STRUCT_NAME(NAME) private: static std::string GetObjectName() { return #NAME; } public: 

namespace nOT {

namespace nUtils {

class myexception : public std::runtime_error {
	public:
		myexception(const char * what);
		myexception(const std::string &what);
		virtual ~myexception();
		virtual void Report() const;
};

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_1; // <=== namespaces

// ======================================================================================
// text trimming
std::string & ltrim(std::string &s);
std::string & rtrim(std::string &s);
std::string & trim(std::string &s);

// ======================================================================================
// string conversions
template <class T>
std::string ToStr(const T & obj) {
	std::ostringstream oss;
	oss << obj;
	return oss.str();
}

struct cNullstream : std::ostream {
		cNullstream();
};
extern cNullstream g_nullstream; // a stream that does nothing (eats/discards data)

// ========== debug ==========

// _dbg_ignore is moved to global namespace (on purpose)

#define _dbg3(X) do { if (_dbg_ignore< 20) gCurrentLogger.write_stream( 20) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0)
#define _dbg2(X) do { if (_dbg_ignore< 30) gCurrentLogger.write_stream( 30) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0)
#define _dbg1(X) do { if (_dbg_ignore< 40) gCurrentLogger.write_stream( 40) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // details
#define _info(X) do { if (_dbg_ignore< 50) gCurrentLogger.write_stream( 50) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // more boring info
#define _note(X) do { if (_dbg_ignore< 70) gCurrentLogger.write_stream( 70) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // info
#define _fact(X) do { if (_dbg_ignore< 75) gCurrentLogger.write_stream( 75) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // interesting event
#define _mark(X) do { if (_dbg_ignore< 80) gCurrentLogger.write_stream( 80) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // marked action 
#define _warn(X) do { if (_dbg_ignore< 90) gCurrentLogger.write_stream( 90) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // some problem
#define _erro(X) do { if (_dbg_ignore<100) gCurrentLogger.write_stream(100) << OT_CODE_STAMP << ' ' << X << gCurrentLogger.endline(); } while(0) // error - report

const char* DbgShortenCodeFileName(const char *s);

std::string cSpaceFromEscape(const std::string &s);

// ========== logger ==========

class cLogger {
	public:
		cLogger();
		~cLogger(){ cout << "Logger destructor" << endl; }
		std::ostream & write_stream(int level);
		void setOutStream();
		std::string icon(int level) const;
		std::string endline() const;
		void setDebugLevel(int level);
	protected:
		unique_ptr<std::ofstream> outfile;
		std::ostream * mStream; // pointing only
		int mLevel;
};



// ====================================================================
// vector debug

template <class T>
std::string vectorToStr(const T & v) {
	std::ostringstream oss;
	for(auto rec: v) {
		oss << rec <<",";
		}
	return oss.str();
}

template <class T>
void DisplayVector(std::ostream & out, const std::vector<T> &v, const std::string &delim=" ") {
	std::copy( v.begin(), v.end(), std::ostream_iterator<T>(out, delim.c_str()) );
}

template <class T>
void EndlDisplayVector(std::ostream & out, const std::vector<T> &v, const std::string &delim=" ") {
	out << std::endl;
	DisplayVector(out,v,delim);
}

template <class T>
void DisplayVectorEndl(std::ostream & out, const std::vector<T> &v, const std::string &delim=" ") {
	DisplayVector(out,v,delim);
	out << std::endl;
}

template <class T>
void DbgDisplayVector(const std::vector<T> &v, const std::string &delim=" ") {
	std::cerr << "[";
	std::copy( v.begin(), v.end(), std::ostream_iterator<T>(std::cerr, delim.c_str()) );
	std::cerr << "]";
}


template <class T, class T2>
void DisplayMap(std::ostream & out, const std::map<T, T2> &m, const std::string &delim=" ") {
	for(auto var : m) {
		out << var.first << delim << var.second << endl;
 	}
}

template <class T, class T2>
void EndlDisplayMap(std::ostream & out, const std::map<T, T2> &m, const std::string &delim=" ") {
	out << endl;
	for(auto var : m) {
		out << var.first << delim << var.second << endl;
 	}
}

template <class T, class T2>
void DbgDisplayMap(const std::map<T, T2> &m, const std::string &delim=" ") {
	for(auto var : m) {
		std::cerr << var.first << delim << var.second << endl;
 	}
}


template <class T>
void DbgDisplayVectorEndl(const std::vector<T> &v, const std::string &delim=" ") {
	DbgDisplayVector(v,delim);
	std::cerr << std::endl;
}

void DisplayStringEndl(std::ostream & out, const std::string text);

bool CheckIfBegins(const std::string & beggining, const std::string & all);
bool CheckIfEnds (std::string const & ending, std::string const & all);
std::string SpaceFromEscape(const std::string &s);
std::string EscapeFromSpace(const std::string &s);
vector<string> WordsThatMatch(const std::string & sofar, const vector<string> & possib);
char GetLastChar(const std::string & str);
std::string GetLastCharIf(const std::string & str); // TODO unicode?
std::string EscapeString(const std::string &s);


template <class T>
std::string DbgVector(const std::vector<T> &v, const std::string &delim="|") {
	std::ostringstream oss;
	oss << "[";
	bool first=true;
	for(auto vElement : v) { if (!first) oss<<delim; first=false; oss <<vElement ; }
	oss << "]";
	//std::copy( v.begin(), v.end(), std::ostream_iterator<T>(oss, delim.c_str()) );
	return oss.str();
}

template <class T>
std::ostream & operator<<(std::ostream & os, const map< T, vector<T> > & obj){
	os << "[";
	for(auto const & elem : obj) {
		os << " [" << elem.first << "=" << DbgVector(elem.second) << "] ";
	}
	os << "]";
  return os;
}

template <class T, class T2>
std::string DbgMap(const map<T, T2> & map) {
	std::ostringstream oss;
		oss << map;
	return oss.str();
}

// ====================================================================
// assert

// ASRT - assert. Name like ASSERT() was too long, and ASS() was just... no.
// Use it like this: ASRT( x>y );  with the semicolon at end, a clever trick forces this syntax :)
#define ASRT(x) do { if (!(x)) Assert(false, OT_CODE_STAMP); } while(0)

void Assert(bool result, const std::string &stamp);

// ====================================================================
// advanced string

const std::string GetMultiline(string endLine = "~");
vector<string> SplitString(const string & str);

const bool checkPrefix(const string & str, char prefix = '^');
// ====================================================================
// operation on files

class cConfigManager {
public:
	bool Load(const string & fileName, map<string, string> & configMap);
	void Save(const string & fileName, const map<string, string> & configMap);
};

extern cConfigManager configManager;

//TODO class cEnvUtils -> opening editor for multiline text input
class cEnvUtils {
	int fd;
	string mFilename;

	void GetTmpTextFile();
	void CloseFile();
	void OpenEditor();
	const string ReadFromTmpFile();
public:
	const string Compose();
};
// ====================================================================

namespace nOper { // nOT::nUtils::nOper
// cool shortcut operators, like vector + vecotr operator working same as string (appending)
// isolated to namespace because it's unorthodox ide to implement this

using namespace std;

// TODO use && and move?
template <class T>
vector<T> operator+(const vector<T> &a, const vector<T> &b) {
	vector<T> ret = a;
	ret.insert( ret.end() , b.begin(), b.end() );
	return ret;
}

template <class T>
vector<T> operator+(const T &a, const vector<T> &b) {
	vector<T> ret(1,a);
	ret.insert( ret.end() , b.begin(), b.end() );
	return ret;
}

template <class T>
vector<T> operator+(const vector<T> &a, const T &b) {
	vector<T> b_vector(1,a);
	return a + b_vector;
}

template <class T>
vector<T>& operator+=(vector<T> &a, const vector<T> &b) {
	a.insert( a.end() , b.begin(), b.end() );
	return a;
}

} // nOT::nUtils::nOper

// ====================================================================

int RangesFindPosition(const vector<int> &R, int pos);

}; // namespace nUtils 

}; // namespace nOT


// global namespace
extern nOT::nUtils::cLogger gCurrentLogger;

std::string GetObjectName();

const extern int _dbg_ignore; // the global _dbg_ignore, but local code (blocks, classes etc) should shadow it to override debug compile-time setting for given block/class
// or to make it runtime by providing a class normal member and editing it in runtime

#define OT_CODE_STAMP ( nOT::nUtils::ToStr("[") + nOT::nUtils::DbgShortenCodeFileName(__FILE__) + nOT::nUtils::ToStr("+") + nOT::nUtils::ToStr(__LINE__) + nOT::nUtils::ToStr(" ") + (GetObjectName()) + nOT::nUtils::ToStr("::") + nOT::nUtils::ToStr(__FUNCTION__) + nOT::nUtils::ToStr("]"))




#endif

