/*
 * comparator.h
 *
 *  Created on: Oct 18, 2016
 *      Author: stan
 */

#ifndef SRC_COMPARATOR_H_
#define SRC_COMPARATOR_H_

#include<iostream>
#include<cmath>
#include<cstdio>
#include<cstdlib>
#include<vector>
#include<algorithm>
#include<fstream>
#include<time.h>
#include<sstream>

using namespace std;

template<typename _Tp, typename _Tp2>
bool comparatorPairFirst( const pair<_Tp, _Tp2>& p1, const pair<_Tp, _Tp2>& p2 ){
	return p1.first>p2.first;
}

template<typename _Tp, typename _Tp2>
bool comparatorPairSecond( const pair<_Tp, _Tp2>& p1, const pair<_Tp, _Tp2>& p2 ){
	return p1.second>p2.second;
}

template<typename _Tp, typename _Tp2>
bool comparatorPairSecondAbsolute( const pair<_Tp, _Tp2>& p1, const pair<_Tp, _Tp2>& p2 ){
	return abs(p1.second)>abs(p2.second);
}


#endif /* SRC_COMPARATOR_H_ */
