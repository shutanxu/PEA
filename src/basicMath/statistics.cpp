/*
 * statistics.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: stan
 */

#include"statistics.h"

StudentTest::StudentTest(vector<double> v1, vector<double> v2){
	datVec1 = v1;
	datVec2 = v2;

	stat = computeStat( datVec1, datVec2 );
	pValue = computePvalue();
}

double
StudentTest::computeStat( vector<double> v1, vector<double> v2 ){
	if( v1.empty() || v2.empty() ){
		cerr<<"!!! error. can not compute t-test, null vector"<<endl;
		return 0;
	}

	//------average
	double ave1 = 0;
	double ave2 = 0;
	size_t 	v1Length= v1.size();
	size_t 	v2Length= v2.size();
	for( size_t i=0; i<v1Length; i++ ){
		ave1 += v1[i];
	}
	for( size_t i=0; i<v2Length; i++ ){
		ave2 += v2[i];
	}
	ave1 = ave1 / double(v1Length);
	ave2 = ave2 / double(v2Length);

	//------variance
	double var1 = 0;
	double var2 = 0;
	for( size_t i=0; i<v1Length; i++ ){
		var1 += ( v1[i] - ave1 )*( v1[i] - ave1 );
	}
	for( size_t i=0; i<v2Length; i++ ){
		var2 += ( v2[i] - ave2 )*( v2[i] - ave2 );
	}
	var1 = sqrt( var1/double(v1Length) );
	var2 = sqrt( var2/double(v2Length) );

	//------estimator of pooled standard deviation
	double sp = 0;
	sp = sqrt(  ((v1Length-1)*var1 + (v2Length-1)*var2 )/( v1Length+v2Length-2 ));

	//------ t-value
	double t = 0;
	t = (ave1-ave2)/( sp * sqrt( 1/double(v1Length) + 1/double(v2Length) ) );
	return t;
}

double
StudentTest::computePvalue( ){
	vector<double> v1,v2;
	vector<double> combVec;
	for( size_t i=0; i<datVec1.size(); i++ ){
		combVec.push_back( datVec1[i] );
	}
	for( size_t i=0; i<datVec2.size(); i++ ){
		combVec.push_back( datVec2[i] );
	}
	int pCount = 0;
	int count = 100;
	for( size_t i=0; i<count; i++ ){
		random_shuffle( combVec.begin(), combVec.end() );

		vector<double> vv1(combVec.begin(), combVec.begin()+datVec1.size());
		vector<double> vv2(combVec.begin()+datVec1.size(), combVec.end());
		v1 = vv1;
		v2 = vv2;

		double val = computeStat( v1, v2 );
		if( ( val > 0 && stat>0 && val > stat ) || ( val<0 && stat<0 && val<stat ) ){
			pCount++;
		}
	}

	return (double)pCount/(double)count;
}






