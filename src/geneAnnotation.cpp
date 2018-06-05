/*
 * geneAnnotation.cpp
 *
 *  Created on: Aug 23, 2016
 *      Author: stan
 */

#include"geneAnnotation.h"

using namespace std;

struct pairSecondCmp{
	bool operator()( const pair<string, float>& lhs, const pair<string, float>& rhs )const{
		return lhs.second > rhs.second ;
	}
};

struct pairFirstCmp{
	bool operator()( const pair<float, string>& lhs, const pair<float, string>& rhs )const{
		return std::abs(lhs.first) > std::abs(rhs.first) ;
	}
};

struct vectorFirstCmp{
	bool operator()( const vector<string>& lhs, const vector<string>& rhs )const{
		return atof(lhs.front().c_str()) > atof(rhs.front().c_str()) ;
	}
};

struct vectorSecondCmp{
	bool operator()( const vector<string>& lhs, const vector<string>& rhs )const{
		return atof(lhs[1].c_str()) > atof(rhs[1].c_str()) ;
	}
};


/*
 * inputMatrix, first column is gene name, first row is sample name
 */
vector< pair<string, float> >
tTestSort( const vector<string> geneNameVec, const vector<vector<float> > inputMatrix,
		const vector<size_t> sampleIndex1, const vector<size_t> sampleIndex2 ){

	if( inputMatrix.empty() ){
		cerr<<"error! input data empty!!!"<<endl;
	}

	if( sampleIndex1.empty() ){
		cerr<<"error! please notify class 1 index."<<endl;
	}

	if( sampleIndex2.empty() ){
		cerr<<"error! please notify class 2 index."<<endl;
	}

	size_t rnum = inputMatrix.size();
	size_t cnum = inputMatrix[0].size();

	vector< pair<string, float> > 	valVec;
	valVec.clear();

	for( size_t i=0; i<rnum; i++ ){
		float	mean1 = 0;
		float	variance1 = 0;
		size_t 	sampleSize1 = sampleIndex1.size();
		for( size_t j=0; j<sampleSize1; j++ ){
			mean1 += inputMatrix[i][ sampleIndex1[j]];
		}
		mean1 = mean1 / float( sampleSize1 );
		for( size_t j=0; j<sampleSize1; j++ ){
			variance1 = variance1 + ( inputMatrix[i][sampleIndex1[j]] - mean1 )*
					( inputMatrix[i][sampleIndex1[j]] - mean1 );
		}
		variance1 = variance1/(float)(sampleSize1-1);

		//variance of class 2
		float	mean2 = 0;
		float 	variance2 = 0;
		size_t 	sampleSize2 = sampleIndex2.size();
		for( size_t j=0; j<sampleSize2; j++ ){
			mean2 += inputMatrix[i][ sampleIndex2[j]];
		}
		mean2 = mean2 / float( sampleSize2 -1 );
		for( size_t j=0; j<sampleSize2; j++ ){
			variance2 = variance2 + ( inputMatrix[i][sampleIndex2[j]] - mean2 )*
					( inputMatrix[i][sampleIndex2[j]] - mean2 );
		}
		variance2 = variance2/(float)(sampleSize2);

		// covariance
		float	covariance = 0;
		covariance = sqrt( ( (sampleSize1-1)*variance1 + (sampleSize2-1)*variance2 )/
				( sampleSize1+sampleSize2 -2 ) );

		float	t = (mean1-mean2)/( covariance*sqrt( 1/float(sampleSize1) + 1/float(sampleSize2) ) );


		pair<string, float>	m;
		m = make_pair(geneNameVec[i], t);
		valVec.push_back( m );
	}

	sort( valVec.begin(), valVec.end(), pairSecondCmp() );

	return valVec;
}

float
geneAnnotation( vector<pair<string, float> > geneRank, vector<string> functionalGeneSet ){
	float	NR = 0;

	size_t	grSize = geneRank.size();
	size_t 	fgSize = functionalGeneSet.size();

	for( size_t i=0; i<grSize; i++ ){
		for( size_t j=0; j<fgSize; j++ ){
			if( functionalGeneSet[j] == geneRank[i].first ){
				NR += geneRank[i].second;
				break;
			}
		}
	}

	float	ES = 0;
	float	maxES = 0;
	for( size_t i=0; i<grSize; i++ ){
		bool exist = false;
		for( size_t j=0; j<fgSize; j++ ){
			if( geneRank[i].first ==  functionalGeneSet[j]  ){
				exist =true;
				break;
			}
		}

		if( exist ){
			//			ES += geneRank[i].second / NR;
			//			ES += 1 / NR;
			ES += 1.0/(float)fgSize;

			if( std::abs(ES) > std::abs(maxES) ){
				maxES = ES;
			}
		}else{
			ES -= 1/(float)(grSize - fgSize);
			if( std::abs(ES) > std::abs(maxES) ){
				maxES = ES;
			}
		}
	}
	return maxES;
}

bool
geneAnnotation( vector<pair<string, float> > geneRank, vector<string> functionalGeneSet, const float refES ){
	float	NR = 0;

	size_t	grSize = geneRank.size();
	size_t 	fgSize = functionalGeneSet.size();

	for( size_t i=0; i<grSize; i++ ){
		for( size_t j=0; j<fgSize; j++ ){
			if( functionalGeneSet[j] == geneRank[i].first ){
				NR += geneRank[i].second;
				break;
			}
		}
	}

	float	ES = 0;
	float	maxES = 0;
	vector<string>	tempGeneSet;
	for( size_t i=0; i<fgSize; i++ ){
		tempGeneSet.push_back( functionalGeneSet[i] );
	}
	for( size_t i=0; i<grSize; i++ ){
		bool exist = false;
		size_t tempGeneSetSize = tempGeneSet.size();
		for( size_t j=0; j<tempGeneSetSize; j++ ){
			if( geneRank[i].first ==  tempGeneSet[j]  ){
				exist =true;
//				tempGeneSet.erase( tempGeneSet.begin() + j );
				break;
			}
		}

		if( exist ){
//			ES += geneRank[i].second / NR;
//			ES += 1 / NR;
			ES += 1.0/(float)fgSize;

			if( std::abs(ES) > std::abs(maxES) ){
				maxES = ES;
			}
		}else{
			ES -= 1/(float)(grSize - fgSize);
			if( std::abs(ES) > std::abs(maxES) ){
				maxES = ES;
			}
		}
		if( std::abs(refES) < std::abs(maxES) ){
			return true;
		}
	}
	return false;
}

/*
 * return true if permuted annotation value is greater than refES
 */
bool
permutation( const vector<string> geneNameVec,
		const vector<vector<float> > inputMatrix,
		const vector<size_t> sampleIndex1,
		const vector<size_t> sampleIndex2,
		const vector<string> functionalGeneSet,
		const float refES ){

	size_t sampleSize1 = sampleIndex1.size();
	size_t sampleSize2 = sampleIndex2.size();

	vector<size_t>	allIndex;
	allIndex.insert(allIndex.end(), sampleIndex1.begin(), sampleIndex1.end());
	allIndex.insert(allIndex.end(), sampleIndex2.begin(), sampleIndex2.end());

	random_shuffle( allIndex.begin(), allIndex.end() );

	vector<size_t>	newSampleIndex1;
	vector<size_t>	newSampleIndex2;
	for( size_t i=0;i<sampleSize1; i++ ){
		newSampleIndex1.push_back( allIndex[i] );
	}
	for( size_t i=0; i<sampleSize2;i++ ){
		newSampleIndex2.push_back( allIndex[i+sampleSize1] );
	}

	vector< pair<string, float> > refResult;
	refResult = tTestSort( geneNameVec, inputMatrix, newSampleIndex1, newSampleIndex2 );

	return geneAnnotation(refResult, functionalGeneSet, refES);
}

void
test_tTestSort(){
	cout<<"test ttest"<<endl;
	ifstream	iif;
	iif.open( "test/matrixData" );
	string	line;
	vector<string>	geneNameVec;
	vector<vector<float> > dataMatrix;
	getline( iif, line ) ;
	while( getline( iif, line ) ){
		vector<string>	strVec = tokenBySpace( line );
		geneNameVec.push_back( strVec.front() );
		vector<float>	lineVal;
		for( size_t i=1; i<strVec.size(); i++ ){
			lineVal.push_back( atof(strVec[i].c_str() ) );
		}
		dataMatrix.push_back(lineVal);
	}

	vector<size_t> vecH;
	vector<size_t> vecL;
	for( size_t i=0; i<64; i++ ){
		vecH.push_back( i );
	}
	for( size_t i=64; i<176; i++ ){
		vecL.push_back( i );
	}

	vector< pair<string, float> > result;
	result = tTestSort( geneNameVec, dataMatrix, vecH, vecL );

	//
	DIR	*pDIR;
	struct dirent *entry;
	vector< vector<string> >	esVec;

	size_t	count = 0;
	if( pDIR = opendir( "dataBase/GSEA_KEGG" ) ){
		while( entry = readdir(pDIR) ){
			if( entry->d_name != string(".") && entry->d_name != string("..") ){
				ifstream	iif3;
				string		fileName = string("dataBase/GSEA_KEGG/")+entry->d_name;

				iif3.open(fileName.c_str());
				string	pathwayName;
				getline(iif3, pathwayName);
				getline(iif3, line);

				vector<string>	geneSet;
				while( getline( iif3, line ) ){
					geneSet.push_back( line );
				}

				float ES = geneAnnotation( result, geneSet );

				size_t	p_count = 0;
				int 	maxPerm = 100;
				time_t	t1=clock();

				for( size_t i=0; i<maxPerm; i++ ){
					if( permutation( geneNameVec, dataMatrix, vecH, vecL, geneSet, ES) ){
						p_count++;
					}
				}
			    cout<<"time consumed:"<<float(clock()-t1)/CLOCKS_PER_SEC<<"\t";
				cout.width(10);
				cout<<ES;
				cout.width(10);
				cout<<(float)p_count/float(maxPerm)<<"\t\t";
				cout<<pathwayName<<endl;

				vector<string>	valVec;
				stringstream	ss1, ss2;
				ss1<<std::abs(ES);
				ss2<<((float)p_count/float(maxPerm));
				valVec.push_back( ss1.str() );
				valVec.push_back( ss2.str() );
				valVec.push_back( pathwayName );
				esVec.push_back(valVec);

				iif3.close();
			}
		}
		closedir(pDIR);
	}
	iif.close();
	sort( esVec.begin(), esVec.end(), vectorSecondCmp() );
	cout<<"-----------------"<<endl;
	for( size_t i=0; i<esVec.size(); i++ ){
		for( size_t j=0; j<esVec[i].size(); j++ ){
			cout<<esVec[i][j]<<"\t";
		}
		cout<<endl;
	}
}







