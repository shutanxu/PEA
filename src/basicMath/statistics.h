/*
 * statistics.h
 *
 *  Created on: Nov 15, 2016
 *      Author: stan
 */

#ifndef SRC_BASICMATH_STATISTICS_H_
#define SRC_BASICMATH_STATISTICS_H_

#include<vector>
#include<algorithm>
#include<iostream>
#include<stdlib.h>
#include<math.h>

using namespace std;

class StudentTest{
public:
	StudentTest(vector<double> v1, vector<double> v2);
	double getStat(){ return stat; }
	double getPvalue(){ return pValue; }

private:
	double 	computeStat( vector<double> v1, vector<double> v2 );
	double 	computePvalue();

private:
	vector<double>	datVec1;
	vector<double>	datVec2;
	double	mean1;
	double	mean2;
	double	deviation1;
	double	deviation2;
	double	stat;
	double	pValue;
	double	qValue;
};

#endif /* SRC_BASICMATH_STATISTICS_H_ */
