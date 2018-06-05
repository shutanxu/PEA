/*
 * tokenSeparator.h
 *
 *  Created on: Oct 18, 2016
 *      Author: stan
 */

#ifndef SRC_TOKENSEPARATOR_H_
#define SRC_TOKENSEPARATOR_H_

#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<ctype.h>
#include<algorithm>

using namespace std;

template<typename T, typename P>
T remove_if(T beg, T end, P pred)
{
    T dest = beg;
    for (T itr = beg;itr != end; ++itr)
        if (!pred(*itr))
            *(dest++) = *itr;
    return dest;
}

vector<string>
tokenBySpace( const string& str );

int
string2int( const string& str );

//! Break a string (supplied as the second argument) into tokens, returned
//! in the first argument. Tokens are determined by the delimiters supplied
//! (defaults to whitespace (i.e., spaces, tabs, newlines)
bool tokenize(std::vector<std::string> &vcr, const char *buf,
                    const char *delimstr);

vector<string>
tokenByTab(const string& str);

vector<string>
tokenByDash( const string& str );

vector<string>
tokenByComma( const string& str , bool removeSpace=true);

vector<string>
tokenByNewLine( const string& str );

vector<string>
tokenByColon( const string& str );


#endif /* SRC_TOKENSEPARATOR_H_ */
