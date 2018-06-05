/*
 * geneAnnotation.h
 *
 *  Created on: Aug 23, 2016
 *      Author: stan
 */

#ifndef SRC_GENEANNOTATION_H_
#define SRC_GENEANNOTATION_H_

#include<iostream>
#include<stdlib.h>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<fstream>
#include<gsl/gsl_statistics.h>
#include<dirent.h>
#include<ctime>
#include<cmath>
#include"tokenSeparator.h"

using namespace std;

vector< pair<string, float> >
tTestSort( const vector<string> geneNameVec, const vector<vector<float> > inputMatrix,
		const vector<size_t> sampleIndex1, const vector<size_t> sampleIndex2 );
float
geneAnnotation( vector<pair<string, float> > geneRank,
		vector<string> functionalGeneSet );

float
permutation( const vector<string> geneNameVec,
		const vector<vector<float> > inputMatrix,
		const vector<size_t> sampleIndex1,
		const vector<size_t> sampleIndex2,
		const vector<string> functionalGeneSet  );

void
test_tTestSort();

#endif /* SRC_GENEANNOTATION_H_ */


