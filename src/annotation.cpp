/*
 * annotation.cpp
 *
 *  Created on: Oct 12, 2016
 *      Author: stan
 */

#include"annotation.h"

using namespace std;

bool
comparatorAnnotationResultES(const AnnotationResult& ar1, const AnnotationResult& ar2){
	return ar1.es > ar2.es;
}

Annotation::Annotation(){
	dataContext.clear();
	geneNameVec.clear();
	sampleNameVec.clear();
	sampleClassNameVec.clear();
	geneCorrelationVec.clear();
	geneListVec.clear();
	enrichmentScoreVec.clear();
	sampleClassIndexVec.clear();
	resultVec.clear();
	annotatedGeneNum= 0 ;
};

void
Annotation::readFile( const string& fileName ){

	dataContext.clear();
	geneNameVec.clear();
	sampleNameVec.clear();
	sampleClassNameVec.clear();
	dataMatrix.clear();
	geneCorrelationVec.clear();
	geneListVec.clear();
	enrichmentScoreVec.clear();
	sampleClassIndexVec.clear();
	resultVec.clear();
	annotatedGeneNum= 0 ;

	string line;
	ifstream	iif;
	iif.open( fileName.c_str() );
	if( !iif ){
		cerr<<"Can not open "<<fileName<<endl;
		return;
	}else{
		string 	line;
		getline( iif,line );
		vector<string> vec = tokenBySpace(line);
		for( size_t i=1; i<vec.size(); i++ ){
			sampleClassNameVec.push_back( vec[i] );
		}

		vector<string>				classNameVec;

		//save class name
		for( size_t i=1; i<sampleClassNameVec.size(); i++ ){
			if( find( classNameVec.begin(), classNameVec.end(), sampleClassNameVec[i] )==
					classNameVec.end()){
				classNameVec.push_back( sampleClassNameVec[i] );
			}
		}

		//save samples indexes in each class
		vector< vector<size_t> >	temp( classNameVec.size(), vector<size_t>() );
		sampleClassIndexVec = temp;
		for( size_t i=1; i<sampleClassNameVec.size(); i++ ){
			size_t index = find( classNameVec.begin(), classNameVec.end(), sampleClassNameVec[i] )-classNameVec.begin();
			sampleClassIndexVec[index].push_back( i );
		}
		cout<<"class:"<<sampleClassNameVec.size()<<endl;

		getline( iif,line );
		vector<string>  tempVec = tokenBySpace(line);
		for( size_t i=1; i<tempVec.size(); i++ ){
			sampleNameVec.push_back( tempVec[i] );
		}

		while( getline( iif, line ) ){
			vector<string> vec = tokenByTab(line);
			string	geneName = vec[0];
			geneNameVec.push_back( vec[0] );
			dataContext.push_back(line);

			vector<float>	datVec;
			for( size_t i=0; i<vec.size(); i++ ){
				datVec.push_back( atof( vec[i].c_str() ) );
			}
			dataMatrix.push_back( datVec );
		}
	}
}

vector<testResult>
Annotation::getTestVec( vector<string> functionGeneList ){
	float val = 0;
	float maxVal = -10e10;
	size_t allGeneSize = geneCorrelationVec.size();
	size_t funcGeneSize = functionGeneList.size();

	vector<testResult>	testVec;

	for( size_t i=0; i<allGeneSize; i++ ){
		for( size_t j=0; j<funcGeneSize; j++ ){
			if( geneCorrelationVec[i].first == functionGeneList[j] ){
				testVec.push_back( geneCorrelationVec[i].second );
			}
		}
	}
	return testVec;
}

double
Annotation::enrichmentScore( vector<string> functionGeneList ){
	float val = 0;
	annotatedGeneNum = 0;
	float maxVal = -10e10;
	size_t allGeneSize = geneCorrelationVec.size();
	size_t funcGeneSize = functionGeneList.size();
	double 	N = allGeneSize;
	double	NR = 0;
	double 	NH = 0;
	bool	existFlag = false;
	for( size_t i=0; i<allGeneSize; i++ ){
		for( size_t j=0; j<funcGeneSize; j++ ){
			if( geneCorrelationVec[i].first == functionGeneList[j] ){
				NR += geneCorrelationVec[i].second.value;
				NH += 1.0;
				existFlag = true;
			}
		}
	}

	if( !existFlag ){
		cerr<<"!!! can not find properate gene name"<<endl;
		return 0;
	}

	for( size_t i=0; i<allGeneSize; i++ ){
		bool flag = false;
		for( size_t j=0; j<funcGeneSize; j++ ){
			if( geneCorrelationVec[i].first == functionGeneList[j] ){
				flag = true;
				annotatedGeneNum++;
				break;
			}
		}
		if( flag ){
			val += geneCorrelationVec[i].second.value/NR;
		}else{
			val -= 1/( N - NH );
		}
		maxVal = maxVal>val ? maxVal:val;
	}

	return maxVal;
}

/**
 * gene first
 */
vector< vector<float> >
Annotation::getDataMatrix( vector<string> functionGeneList ){
	vector< vector<float> > dataM;
	for( size_t i=0; i<dataContext.size(); i++ ){
		string line = dataContext[i];
		vector<string> 	vec = tokenByTab(line);
		vector<float>	datVec;
		string	geneName = vec[0];
		for( size_t j=0; j<functionGeneList.size(); j++ ){
			if( geneName == functionGeneList[j] ){
				for( size_t k=1; k<vec.size(); k++ ){
					datVec.push_back( atof(vec[k].c_str()) );
				}
				dataM.push_back( datVec );
				datVec.clear();
				break;
			}
		}
	}
	return dataM;
}

vector<string>
Annotation::getAnnotatedGenes( vector<string> functionGeneList ){
	vector<string> geneVec;
	for( size_t i=0; i<dataContext.size(); i++ ){
		string line = dataContext[i];
		vector<string>	vec = tokenByTab(line);
		string	geneName = vec[0];
		for( size_t j=0; j<functionGeneList.size(); j++ ){
			if( geneName == functionGeneList[j] ){
				geneVec.push_back( geneName );
				break;
			}
		}
	}
	return geneVec;
}

void
Annotation::readGeneListFile( const string& fileName ){
	string line;
	geneListVec.clear();
	ifstream	iif;
	iif.open( fileName.c_str() );
	if( !iif ){
		cerr<<"Can not open "<<fileName<<endl;
		return;
	}else{
		string line;
		vector<string>		strVec;
		while( getline( iif, line ) ){
			strVec = tokenByTab( line );
			geneListVec.push_back( strVec );
		}
	}
}

void
Annotation::run(){
	cout<<"dataContext:"<<dataContext.size()<<endl;
	//-----------------------
	time_t	t0 = clock();
	resultVec.clear();
	for( size_t i=0; i<dataContext.size(); i++ ){
		string line = dataContext[i];
		vector<string> vec = tokenByTab(line);
		string	geneName = vec[0];
		geneNameVec.push_back( vec[0] );

		vector<double>	valVec1, valVec2;
		for( size_t i=0; i<sampleClassIndexVec[0].size(); i++ ){
			size_t index =  sampleClassIndexVec[0][i];
			valVec1.push_back( atof( vec[index].c_str() ) );
		}

		for( size_t i=0; i<sampleClassIndexVec[1].size(); i++ ){
			size_t index =  sampleClassIndexVec[1][i];
			valVec2.push_back( atof( vec[index].c_str() ) );
		}
		StudentTest ttest( valVec1, valVec2 );
		double tVal = ttest.getStat();
		double pVal = ttest.getPvalue();
		testResult	rest(geneName, tVal, pVal);

		pair<string, testResult>	m=make_pair(geneName, rest);
		geneCorrelationVec.push_back(m);
	}

	sort( geneCorrelationVec.begin(), geneCorrelationVec.end(), comparatorSecondResult<string> );

	//geneList enrichment
	for( size_t i=0; i<geneListVec.size(); i++ ){
		vector<string> listVec;
		listVec.insert( listVec.end(), geneListVec[i].begin()+2, geneListVec[i].end() );
		float score = enrichmentScore( listVec );
		vector<testResult>	testVec = getTestVec( listVec );

		pair<string, float>	scoreP = make_pair(geneListVec[i][0], score);
		enrichmentScoreVec.push_back( scoreP );

		string	pathway = geneListVec[i][0];
		vector<string>	geneListVec = listVec;
		int				pathwayGeneNum = geneListVec.size();
		float			es = score;
		float			p_value = 0;
		float			q_value = 0;

		AnnotationResult ar( pathway, geneListVec, pathwayGeneNum, annotatedGeneNum,
				es, p_value, q_value, testVec );
		resultVec.push_back( ar );
	}
	sort( resultVec.begin(), resultVec.end(), comparatorAnnotationResultES );
	sort( enrichmentScoreVec.begin(), enrichmentScoreVec.end(), comparatorPairSecond<string, float>);
}
