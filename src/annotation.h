/*
 * annotation.h
 *
 *  Created on: Oct 12, 2016
 *      Author: stan
 */

#ifndef SRC_ANNOTATION_H_
#define SRC_ANNOTATION_H_

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
#include<cmath>
#include<time.h>
#include<algorithm>
#include"tokenSeparator.h"
#include"comparator.h"
#include"basicMath/statistics.h"

using namespace std;

struct testResult{
	testResult( const string& n, const float& v, const float& p ):geneName(n),
			value(v),pValue(p){}

	string	geneName;
	float	value;
	float 	pValue;
};

template<typename _Tp>
bool comparatorSecondResult( const pair<_Tp, testResult>& p1, const pair<_Tp, testResult>& p2 ){
//	return (p1.second.value) > (p2.second.value);
	return (p1.second.pValue) < (p2.second.pValue);
}

struct AnnotationResult{
	AnnotationResult( const string& p, const vector<string>& l,
			const int& pn, const int& an, const float& e,
			const float& pv, const float& qv):pathway(p),geneListVec(l),
			pathwayGeneNum(pn),annotatedGeneNum(an), es(e), p_value(pv),
			q_value(qv){}
	AnnotationResult( const string& p, const vector<string>& l,
			const int& pn, const int& an, const float& e,
			const float& pv, const float& qv, const vector<testResult>&	t ):pathway(p),geneListVec(l),
			pathwayGeneNum(pn),annotatedGeneNum(an), es(e), p_value(pv),
			q_value(qv), testVec(t){}

	string					pathway;
	vector<string>			geneListVec;
	int						pathwayGeneNum;
	int						annotatedGeneNum;
	float					es;		//enrichment score
	float					p_value;
	float					q_value;
	vector<testResult>		testVec;
};

bool
comparatorAnnotationResultES(const AnnotationResult& ar1, const AnnotationResult& ar2);

class Annotation{

public:
	Annotation();
	Annotation( const string& fileName ){ readFile(fileName); };
	void						readFile( const string& fileName );
	vector< vector<float> >		getDataMatrix( vector<string> functionGeneList );
	vector<string>				getAnnotatedGenes( vector<string> functionGeneList );
	vector<string>				getGeneNameVec(){ return geneNameVec; }
	vector<string >				getSampleNameVec(){ return sampleNameVec; }
	vector<string>				getSampleClassVec(){ return sampleClassNameVec; }
	vector< vector<float> >		getDataMatrix(){ return dataMatrix; };
	void						readGeneListFile( const string& fileName );
	double						enrichmentScore( vector<string> functionGeneList );
	vector<testResult>			getTestVec( vector<string> functionGeneList );
	vector<AnnotationResult>	getAnnotationResult(){ return resultVec; }
	void	run();

private:
//	double 	tTest( vector<double> v1, vector<double> v2 );

private:
	vector<string>						geneNameVec;
	vector<string>						sampleNameVec;
	vector<string>						sampleClassNameVec;
	vector<vector<float> >				dataMatrix;
	vector<string >						dataContext;

	vector<pair<string, testResult> >	geneCorrelationVec;
	vector<vector<string> >				geneListVec;
	vector<pair<string, float> >			enrichmentScoreVec;
	vector< vector<size_t> >			sampleClassIndexVec;
	vector<AnnotationResult>			resultVec;
	int									annotatedGeneNum;
};

#endif /* SRC_ANNOTATION_H_ */



