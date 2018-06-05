/*
 * tokenSeparator.cpp
 *
 *  Created on: Oct 18, 2016
 *      Author: stan
 */

#include"tokenSeparator.h"


vector<string>
tokenBySpace( const string& str ){
	if( str.empty() ){
		string						s("");
		vector<string>		vs;
		vs.push_back( s );
		return vs;
	}
	string buf ="";  // Have a buffer string

	stringstream ss(str);  // Insert the string into a stream
	vector<string> tokens;  // Create vector to hold our words
	while (ss >> buf){
		tokens.push_back(buf);
	}
	if( tokens.empty() ){
		string						s("");
		vector<string>		vs;
		vs.push_back( s );
		return vs;
	}
	buf ="";
	buf.clear();
	return tokens;
}

int
string2int( const string& str ){
	int b;
	stringstream 	ss(str);
	ss>>b;
	return b;
}

//! Break a string (supplied as the second argument) into tokens, returned
//! in the first argument. Tokens are determined by the delimiters supplied
//! (defaults to whitespace (i.e., spaces, tabs, newlines)
bool tokenize(std::vector<std::string> &vcr, const char *buf,
                    const char *delimstr)
{
  vcr.clear();
  if (!buf || !delimstr)
    return false;

  string s(buf);
  s += delimstr[0]; //forces last token to be parsed
  size_t startpos=0,endpos=0;

  for (;;)
    {
      startpos = s.find_first_not_of(delimstr,startpos);
      endpos = s.find_first_of(delimstr,startpos);

      if (endpos <= s.size() && startpos <= s.size())
        vcr.push_back(s.substr(startpos,endpos-startpos));
      else
        break;

      startpos = endpos+1;
    }

  return(true);
}

vector<string>
tokenByTab(const string& str){
    vector<string>  vs;

    if(str.empty()){
        string          s("");
        vs.push_back(s);
        return vs;
    }

    stringstream ss(str);
    string token;
    while(std::getline(ss, token, '\t')){
        vs.push_back(token);
    }
    if(vs.empty()){
        string s("");
        vs.push_back(s);
        return vs;
    }
    return vs;
}

vector<string>
tokenByDash( const string& str ){
    vector<string>  vs;

    if(str.empty()){
        string          s("");
        vs.push_back(s);
        return vs;
    }

    stringstream ss(str);
    string token;
    while(std::getline(ss, token, '_')){
        vs.push_back(token);
    }
    if(vs.empty()){
        string s("");
        vs.push_back(s);
        return vs;
    }
    return vs;
}

vector<string>
tokenByComma( const string& str, bool removeSpace ){
    vector<string>  vs;

    if(str.empty()){
        string          s("");
        vs.push_back(s);
        return vs;
    }

    stringstream ss(str);
    string token;
    while(std::getline(ss, token, ',')){

    	if(removeSpace){
    		token.erase(std::remove(token.begin(), token.end(), ' '), token.end());
    	}
        vs.push_back(token);
    }
    if(vs.empty()){
        string s("");
        vs.push_back(s);
        return vs;
    }
    return vs;
}

vector<string>
tokenByNewLine( const string& str ){
    vector<string>  vs;

    if(str.empty()){
        string          s("");
        vs.push_back(s);
        return vs;
    }

    stringstream ss(str);
    string token;
    while(std::getline(ss, token, '\n')){
        vs.push_back(token);
    }
    if(vs.empty()){
        string s("");
        vs.push_back(s);
        return vs;
    }
    return vs;
}

vector<string>
tokenByColon( const string& str ){
    vector<string>  vs;

    if(str.empty()){
        string          s("");
        vs.push_back(s);
        return vs;
    }

    stringstream ss(str);
    string token;
    while(std::getline(ss, token, ':')){
        vs.push_back(token);
    }
    if(vs.empty()){
        string s("");
        vs.push_back(s);
        return vs;
    }
    return vs;
}
