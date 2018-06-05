#include"layout.h"
#include<algorithm>
#include<time.h>

using namespace std;

Layout::Layout(vector<Vertex>& v, vector<Link>& l){
    verVec.clear();
    for( size_t i=0; i<v.size(); i++ ){
        verVec.push_back(v[i]);
    }
    linkVec.clear();
    for( size_t i=0; i<l.size(); i++ ){
        linkVec.push_back(l[i]);
    }

    frameWidth=10;
    frameLength=10;
    area=frameWidth*frameLength;
    K=sqrt(area/(float)verVec.size());
    max_iteration=1000;

    //map link to nodes
    for( size_t i=0; i<linkVec.size(); i++ ){
        Link e=linkVec[i];
        linkVec[i].set_source( find_vertex(e.get_source_id()) );
        linkVec[i].set_target( find_vertex(e.get_target_id()) );
    }
}

// Method for finding a node by id.
// @param string id
// @param Node** node
// @return bool
Vertex* Layout::find_vertex(string id){
    for(int i=0; i<verVec.size(); i++){
//        cout<<verVec.at(i).get_id()<<endl;
        if(verVec.at(i).get_id() == id){
            return &(verVec[i]);
        }
    }
    cerr<<"can not find: "<<id<<endl;
}


float
Layout::fa( const float& x, const float& k ){
    return x*x/k;
}

float
Layout::fr( const float& x, const float& k ){
    return k*k/x;
}

// Method for performing the force directed layout.
// the algorithm by Fruchterman and Reingold
// @return bool
bool Layout::layout(){
	//if there are fixed edge length, then call another layout function: layout_fixedEdgeLength()
    int     it=0;
    bool    flag=true;
    time_t 	t1=clock();
    float 	mean = 0;
    while( it< max_iteration && flag ){
    	it++;
    	if(it%10==0){
//    		cout<<"\r"<<it<<"\t"<<max_iteration;
    	}
    	flag=true;

    	mean = 0;
    	for( size_t i=0; i<linkVec.size(); i++ ){
    		Coord   delta = linkVec[i].get_source()->get_pos()-linkVec[i].get_target()->get_pos();
    		mean += delta.get_dis();
    	}
    	mean /= (float)(linkVec.size());

    	//each vertex has two vectors: .pos and .disp
    	// calculate dispersion forces
    	for( size_t i=0; i<verVec.size(); i++ ){
    		verVec[i].set_disp(0,0,0);
    		for( size_t j=0; j<verVec.size(); j++ ){
    			if( i != j ){
    				Coord   delta=(verVec[i].get_pos()-verVec[j].get_pos());
    				float   delta_abs=delta.get_dis();
    				Coord   disp;
    				if( delta_abs !=0 ){
    					if( delta_abs < mean ){
    						delta_abs /=3.0;
    						disp=verVec[i].get_disp()+(delta/delta_abs)*fr(delta_abs, K);
    					}else{
    						disp=verVec[i].get_disp()+(delta/delta_abs)*fr(delta_abs, K);
    					}
    				}else{
    					disp=verVec[i].get_disp();
    				}
    				verVec[i].set_disp(disp);
    			}
    		}
    	}

    	// calculate attractive forces
    	for( size_t i=0; i<linkVec.size(); i++ ){
    		//each edges is an ordered pair of vertices .vand.u
    		Coord   delta = linkVec[i].get_source()->get_pos()-linkVec[i].get_target()->get_pos();
    		float   delta_abs=delta.get_dis();
    		Coord	v,y;
    		//
    		//    		if( delta_abs > mean ){
    		if( delta_abs > mean ){
    			v=linkVec[i].get_source()->get_disp()-(delta/delta_abs)*fa(delta_abs,K);
    			y=linkVec[i].get_target()->get_disp()+(delta/delta_abs)*fa(delta_abs,K);
    		}else{
    			v=linkVec[i].get_source()->get_disp();
    			y=linkVec[i].get_target()->get_disp();
    		}

    		linkVec[i].set_source_disp(v);
    		linkVec[i].set_target_disp(y);
    	}
    	//limit max displacement to temperature t and prevent from displacement outside frame
    	for(size_t i=0; i<verVec.size(); i++){
    		Coord   disp=verVec[i].get_disp();
    		Coord   c=verVec[i].get_pos()+(disp/disp.get_dis());//*min(verVec[i].get_disp(),temperature);
    		verVec[i].set_pos(c);
    		if( disp.get_dis()/float(linkVec.size()) > 1.0 ){
    			flag=true;
    		}
    	}
    }

    //relocate edge length
    bool flagEdge = true;
    for(size_t i=0; i<linkVec.size(); i++){
    	if( isnan( linkVec[i].get_value())){
    		flagEdge=false;
    	}
    }
    if( 1 /* flagEdge */ ){
    	return layout_fixedEdgeLength();
    }

    normalScale();
    cout<<"layout time:"<<float(clock()-t1)/CLOCKS_PER_SEC<<endl;
    return true;
}

/**
 *
 */
bool Layout::layout2(){
	int		it = 0;
	bool	flag = true;
    time_t 	t1=clock();
    float 	mean = 0;

    for( size_t i=0; i<verVec.size(); i++ ){
    	float angle = ( float(i)/float(verVec.size()) ) *2* 3.14159;
    	float x=cos(angle);
    	float y=sin(angle);
    	Coord c(x,y,0);
    	verVec[i].set_pos(c);
    }


    normalScale();
    cout<<"layout time:"<<float(clock()-t1)/CLOCKS_PER_SEC<<endl;
    return true;
}

vector<Vertex>
Layout::get_connectedVertex( Vertex v ){
	vector<Vertex>	vec;
	for( size_t i=0; i<linkVec.size(); i++ ){
		if( linkVec[i].get_source()->get_id() == v.get_id() ){
			vec.push_back( *linkVec[i].get_target() );
		}
		if( linkVec[i].get_target()->get_id() == v.get_id() ){
			vec.push_back( *linkVec[i].get_source() );
		}
	}

	return vec;
}

int
Layout::join_num(  vector<Vertex> v1, vector<Vertex> v2 ){
	int count=0;
	for( size_t i=0; i<v1.size(); i++ ){
		for( size_t j=0; j<v2.size(); j++ ){
			if( v1[i].get_id() == v2[j].get_id() ){
				count++;
			}
		}
	}
	return count;
}

bool
Layout::layout_fixedEdgeLength(){
	float	maxDist = -10e10;
	float	maxVal = -10e10;
	for( size_t i=0; i<linkVec.size(); i++ ){
		Coord   delta = linkVec[i].get_source()->get_pos()-linkVec[i].get_target()->get_pos();
		float   delta_abs=delta.get_dis2D();
		maxDist=maxDist>delta_abs ? maxDist:delta_abs;
		maxVal = maxVal > linkVec[i].get_value() ? maxVal:linkVec[i].get_value();
	}

	for( size_t i=0; i<linkVec.size(); i++ ){
		Coord   delta = linkVec[i].get_target()->get_pos()-linkVec[i].get_source()->get_pos();
		float   delta_abs=delta.get_dis2D();
		Coord	trans= delta*((linkVec[i].get_value()/maxVal)*maxDist-delta_abs)/delta_abs;
		Coord	targetCoord=linkVec[i].get_target()->get_pos();
		Coord	sourceCoord=linkVec[i].get_source()->get_pos();
		if( delta_abs != 0 ){
			linkVec[i].set_targetX( targetCoord.x+trans.x );
			linkVec[i].set_targetY( targetCoord.y+trans.y );
		}else{
			linkVec[i].set_targetX( sourceCoord.x );
			linkVec[i].set_targetY( sourceCoord.y );
		}
	}
    return true;
}

void
Layout::print_all_vertex(){
    for( size_t i=0; i<verVec.size(); i++ ){
        cout.width(5);
        cout<<left<<i+1;
        Coord c=verVec[i].get_pos();
        cout.width(10);
        cout<<left<<c.x;
        cout.width(10);
        cout<<left<<c.y;
        cout.width(10);
        cout<<left<<c.z<<endl;
    }
}

void
Layout::normalScale(){
    Coord   sum(0,0,0);
    float   maxDis=-10e10;
    for(size_t i=0; i<verVec.size()-1; i++){
        sum=sum+verVec[i].get_pos();
        for( size_t j=i+1; j<verVec.size(); j++ ){
            float dis=verVec[i].get_pos().distTo(verVec[j].get_pos());
            maxDis = maxDis > dis ? maxDis : dis;
        }
    }
    if( maxDis != 0 ){
        Coord   center = sum/(float)verVec.size();
        for( size_t i=0; i<verVec.size(); i++ ){
            Coord newC = (verVec[i].get_pos()-center)/(2*maxDis);
            verVec[i].set_pos(newC);
        }
    }
}

//------------------------------------------------------------------------------------

void
Layout::updateGraphDistMatrix(){
	int searchDepth = 10e10;
	vector<vector<int> > dist( verVec.size(), vector<int>(verVec.size(),0) );
	graphDistMatrix=dist;
	size_t dim=verVec.size();
	for( size_t i=0; i<dim; i++ ){
		string id=verVec[i].get_id();
		vector<Vertex> bfsV=bfs( verVec, linkVec, id, searchDepth );
		for( size_t j=0; j<dim; j++ ){
			graphDistMatrix[i][j]=bfsV[j].get_depth();
		}
//		cout<<i<<"\t"<<dim<<endl;
	}

	if(0){
		for( size_t i=0; i<dim; i++ ){
			for( size_t j=0; j<dim; j++ ){
				cout<<graphDistMatrix[i][j]<<"\t";
			}
			cout<<endl;
		}
	}
//	return true;
}

/**
 * Pawel Gajer, A Fast Multi-Dimensional Algorithm for Drawing Large Graphs
 */
void
Layout::MISfilteration(){

	size_t dim=verVec.size();
	vector<bool> 	visitedVec; //0 indicated not visited
	vector<size_t> 	currentVec;
	vector<size_t> 	newCurrentVec;
	int iteration=0;

	visitedVec.clear();
	currentVec.clear();
	newCurrentVec.clear();

	for( size_t i=0; i<dim; i++ ){
		visitedVec.push_back(false);
		currentVec.push_back(i);
	}
	int unvisited=dim;
	filterationIndexVec.clear();
	filterationIndexVec.push_back( currentVec );
	currentVec.clear();
	int distCut=0;
	while(unvisited>2){
		distCut++;
		size_t current=find(visitedVec.begin(),visitedVec.end(),false)-visitedVec.begin();
		while(  find( visitedVec.begin(), visitedVec.end(), false ) != visitedVec.end()  ){
			 //get the first unvisited
			size_t current=find(visitedVec.begin(), visitedVec.end(), false)-visitedVec.begin();
			currentVec.push_back(current);
			visitedVec[current]=true;
			for( size_t i=0; i<dim; i++ ){
				if( visitedVec[i]==false && graphDistMatrix[current][i]<=distCut ){
					visitedVec[i]=true;
				}
			}
		}
		cout<<currentVec.size()<<endl;
		if(!currentVec.empty() && currentVec.size() != unvisited){
			filterationIndexVec.push_back(currentVec);
		}else{
			break;
		}

		for(size_t i=0; i<currentVec.size(); i++ ){
			visitedVec[ currentVec[i] ]=false;
		}
		unvisited=currentVec.size();
		currentVec.clear();
	}

	if(1){
		cout<<endl<<"filteration:"<<endl;
		for( size_t i=0; i<filterationIndexVec.size(); i++ ){
			for( size_t j=0; j<filterationIndexVec[i].size(); j++ ){
				cout<<filterationIndexVec[i][j]<<"\t";
			}
			cout<<endl;
		}
	}
}

/**
 * layout subset of verVec, based on the graph distance
 */
bool
Layout::layout_SubSet2( int maxIter ){


	int 	it=0;
	bool	flag=true;
	size_t 	indexVecSize=verVec.size();

    while( it<maxIter/*max_iteration*/ && flag ){
		it++;
		if( it%10==0 ){
//			cout<<"\xd"<<it<<"\t"<<max_iteration<<" $";
		}

		float	averageEdgeLength=0;
		size_t 	linkVecSize=linkVec.size();
		for(size_t i=0; i<linkVecSize; i++){
			averageEdgeLength+=linkVec[i].get_dist();
		}
		averageEdgeLength/=(float)linkVecSize;

		if(1){
			//edge length variance
			float sum=0;
			for(size_t i=0; i<linkVecSize; i++){
				sum+=(linkVec[i].get_dist()-averageEdgeLength)*(linkVec[i].get_dist()-averageEdgeLength);
			}
			cout<<endl<<"averageEdgeLength:"<<averageEdgeLength;
			cout<<"\t variance:"<<sum/(float)linkVecSize<<endl;
		}

		for( size_t i=0; i<linkVecSize; i++ ){
			float 	dist=linkVec[i].get_dist();
			Coord	delta=linkVec[i].get_target()->get_pos()-linkVec[i].get_source()->get_pos();
			Coord v,y;

			if(dist>averageEdgeLength){
				Coord	updateCoord=delta*((dist-averageEdgeLength)/(4*dist));
				Coord	c1=linkVec[i].get_source()->get_pos()+updateCoord;
				Coord	c2=linkVec[i].get_target()->get_pos()-updateCoord;
				linkVec[i].get_source()->set_pos(c1);
				linkVec[i].get_target()->set_pos(c2);
			}else if(dist!=0){
//				Coord	updateCoord=delta*((averageEdgeLength-dist)/(4*dist));
//				Coord	c1=linkVec[i].get_source()->get_pos()-updateCoord;
//				Coord	c2=linkVec[i].get_target()->get_pos()+updateCoord;
//				linkVec[i].get_source()->set_pos(c1);
//				linkVec[i].get_target()->set_pos(c2);
			}
		}
	}

    normalScale();
	return true;
}

/**
 * layout subset of verVec, based on the graph distance
 */
bool
Layout::layout_SubSet( vector<size_t> indexVec, int maxIter ){

	if( indexVec.empty() ){
		cerr<<"error: no vertex index!!!"<<endl;
		return false;
	}
	if( indexVec.size() <=2 ){
		return true;
	}

	int 	it=0;
	bool	flag=true;
	size_t 	indexVecSize=indexVec.size();

	// search for the minimum graph distance of vertexes in indexVec
	int minDist=10e10;
	for(size_t i=0; i<indexVecSize-1; i++){
		for(size_t j=i+1; j<indexVecSize; j++){
			int dist=graphDistMatrix[ indexVec[i] ][ indexVec[j] ];
			if( dist>0 ){
				minDist=minDist<dist ? minDist:dist;
			}
		}
	}

    while( it<maxIter/*max_iteration*/ && flag ){
		it++;
		if( it%10==0 ){
//			cout<<"\xd"<<it<<"\t"<<max_iteration<<" $";
		}

		flag = true;
    	// each vertex has two vectors: .pos and .disp
		// calculate dispersion forces
		for(size_t i=0; i<indexVecSize; i++){
			verVec[ indexVec[i] ].set_disp(0,0,0);
			for( size_t j=0; j<indexVecSize; j++ ){
				if( i != j && graphDistMatrix[ indexVec[i] ][ indexVec[j] ] <= minDist+10 ){
					Coord	delta=verVec[ indexVec[i] ].get_pos()-verVec[ indexVec[j] ].get_pos();
					float	delta_abs=delta.get_dis();
					int		graphDis = graphDistMatrix[ indexVec[i] ][ indexVec[j] ];
//					delta_abs = delta_abs/(float)graphDis;

					Coord	disp;
					if( delta_abs!=0 ){
						disp=verVec[ indexVec[i] ].get_disp() + (delta/delta_abs)*fr( delta_abs, K );
					}else{
						disp=verVec[ indexVec[i] ].get_disp();
					}
					verVec[ indexVec[i] ].set_disp(disp);
				}
			}
		}


		// calculate attractive forces: first search for those vertex that has the minimum graph distance
		// to current vertex (indexVec[i]) and save them in vertexIndexVec[minIndex], then calculate the
		// attractive forces only with those closest vertexes
		for( size_t i=0; i<indexVecSize; i++ ){

			// distance between current vertex (indexVec[i]) to other vertex, the graphDistVec only saves distances.
	        vector<int>                 graphDistVec;

	        //indexes of vertexes that have the same distance as in graphDistVec.
	        //for example: if the first (or ith) element of graphDistVec is 4, then the first (or ith)
	        //vector of vertexIndexVec save those vertexes that has graph dist 4 with current vertex indexVec[i]
			vector<vector<size_t> > 	vertexIndexVec;

			//save vertexes that have same distance with current vertex (indexVec[i]), and save them in vertexIndexVec
			//the corresponding distances are saved in graphDistVec
			for( size_t j=0; j<indexVecSize; j++ ){
				if( i!=j ){
					int dist=graphDistMatrix[indexVec[i]][indexVec[j]];
					if( find(graphDistVec.begin(), graphDistVec.end(), dist)==graphDistVec.end() ){
						graphDistVec.push_back(dist);
						vector<size_t> indVec;
						indVec.push_back( indexVec[j] );
						vertexIndexVec.push_back( indVec );
					}else{
						size_t v=find(graphDistVec.begin(), graphDistVec.end(), dist)-graphDistVec.begin();
						vertexIndexVec[v].push_back( indexVec[j] );
					}
				}
			}

			//find those vertexes that have minimum distance to current vertex (indexVec[i]).
			size_t  minIndex=min_element(graphDistVec.begin(),graphDistVec.end())-graphDistVec.begin();

			//update the attraction force of current vertex (indexVec[i]) and its closest neighbors save in
			//vertexIndexVec[minIndex]
			for(size_t j=0; j<vertexIndexVec[minIndex].size(); j++){
				Coord 	delta = verVec[ indexVec[i] ].get_pos() - verVec[ vertexIndexVec[minIndex][j] ].get_pos();
				float	delta_abs=delta.get_dis();
				int		graphDis = graphDistMatrix[ indexVec[i] ][ vertexIndexVec[minIndex][j] ];
				delta_abs = delta_abs/(float)graphDis;

				Coord v,y;
				if( delta_abs != 0 ){
					v=verVec[ indexVec[i] ].get_disp() - (delta/delta_abs)*fa(delta_abs,K);
					y=verVec[ vertexIndexVec[minIndex][j] ].get_disp() + (delta/delta_abs)*fa(delta_abs,K);
				}else{
					v=verVec[ indexVec[i] ].get_disp();
					y=verVec[ vertexIndexVec[minIndex][j] ].get_disp();
				}
				verVec[ indexVec[i] ].set_disp( v );
				verVec[ vertexIndexVec[minIndex][j] ].set_disp( y );
			}
		}

		//limit max displacement to temperature t and prevent from displacement outside frame
		for( size_t i=0; i<indexVecSize; i++ ){
			Coord	disp=verVec[ indexVec[i] ].get_disp();
			Coord	c=verVec[ indexVec[i] ].get_pos() + (disp/disp.get_dis());//*min(verVec[i].get_disp(),temperature);
			verVec[ indexVec[i] ].set_pos( c );
			if( it < 100 ){
				flag = true;
			}
		}
	}

	if( 0 ){
		cout<<endl;
		for( size_t i=0; i<indexVec.size(); i++ ){
			cout<<i<<"\t";
			verVec[ indexVec[i] ].print();
		}
	}

    normalScale();
	return true;
}

/**
 * note that the newIndexVec contains more vertex than the fixedIndexVec
 */
bool
Layout::layout_SubSet(vector<size_t> newIndexVec, vector<size_t> fixedIndexVec,
                      int maxIter){

	for( size_t i=0; i<newIndexVec.size(); i++ ){
        vector<int>                 graphDistVec;   // distance between new vertex to fixed index
		vector<vector<size_t> > 	vertexIndexVec;  //indexes of new vertex that have distance in graphDistVec to fixed vertex.

		if( find( fixedIndexVec.begin(), fixedIndexVec.end(), newIndexVec[i] ) ==
				fixedIndexVec.end() ){
			//for a new vertex, if there are two fixed vertex that have minimum graph distance, then
			//the coord of the new vertex is triangle with the two fixed vertex; if there are more than two
			//fixed vertex that have minimum graph distance, the the coord is the average of those fixed vertex.
			for( size_t j=0; j<fixedIndexVec.size(); j++ ){
				int	dist=graphDistMatrix[newIndexVec[i]][fixedIndexVec[j]];
				if( find(graphDistVec.begin(), graphDistVec.end(), dist)==graphDistVec.end() ){
					graphDistVec.push_back(dist);
					vector<size_t> indVec;
					indVec.push_back( fixedIndexVec[j] );
					vertexIndexVec.push_back( indVec );
				}else{
					size_t v=find(graphDistVec.begin(), graphDistVec.end(), dist)-graphDistVec.begin();
					vertexIndexVec[v].push_back( fixedIndexVec[j] );
				}
			}

            if(0){
                cout<<newIndexVec[i]<<":";
                for(size_t m=0; m<graphDistVec.size(); m++){
                    cout<<graphDistVec[m]<<"->";
                    for(size_t n=0; n<vertexIndexVec[m].size(); n++){
                        cout<<vertexIndexVec[m][n]<<" ";
                    }
                    cout<<": ";
                }
                cout<<"; "<<endl;
            }

            size_t minIndex=min_element(graphDistVec.begin(),graphDistVec.end())-graphDistVec.begin();

            //initialize the coord of the new vertex (newIndexVec[i])
            if( vertexIndexVec[minIndex].size()==1 ){
//                cout<<"!!!"<<endl;
                Coord	v1=verVec[ vertexIndexVec[minIndex][0] ].get_pos();
                size_t  tempIndex=rand()%verVec.size();
                Coord   v2=verVec[tempIndex].get_pos();
				Coord	center=(v1+v2)/2.0;
				float 	x=center.x+ (rand()%10000000/1000000.0)*(v1.x-v2.x);
				float 	y=center.y+ (rand()%10000000/1000000.0)*(v1.y-v2.y);
				float 	z=center.z+ (rand()%10000000/1000000.0)*(v1.z-v2.z);
				Coord	n(x,y,z);
				verVec[ newIndexVec[i] ].set_pos(n);
            }else if( vertexIndexVec[minIndex].size() >=2 ){
            	Coord 	center(0,0,0);
            	for( size_t j=0; j<vertexIndexVec[minIndex].size(); j++ ){
            		center = center+verVec[ vertexIndexVec[minIndex][j] ].get_pos();
            	}
            	center = center/float(vertexIndexVec[minIndex].size());
            	Coord	v1=verVec[ vertexIndexVec[minIndex][0] ].get_pos();
            	Coord	v2=verVec[ vertexIndexVec[minIndex][1] ].get_pos();
            	float 	x=center.x+ (rand()%10000000/1000000.0)*(v1.x-v2.x);
            	float 	y=center.y+ (rand()%10000000/1000000.0)*(v1.y-v2.y);
            	float 	z=center.z+ (rand()%10000000/1000000.0)*(v1.z-v2.z);

            	Coord	n(x,y,z);
            	verVec[ newIndexVec[i] ].set_pos(n);
			}
		}
	}

	vector<size_t> combineIndexVec;
	combineIndexVec.insert(combineIndexVec.end(), newIndexVec.begin(), newIndexVec.end());

    layout_SubSet( combineIndexVec, maxIter );
}

void
Layout::Pawel_Michael_layout(){
    layout_SubSet(filterationIndexVec.back(), 5 );

    for( size_t i=1; i<filterationIndexVec.size(); i++ ){
		size_t 	index=filterationIndexVec.size() - i -1;
        cout<<"$:"<<i<<endl;
        int iter=(int)filterationIndexVec[ index ].size();
        layout_SubSet( filterationIndexVec[ index ], filterationIndexVec[index+1], 2*iter );
	}

    layout_SubSet2( 10 );

	normalScale();
	cout<<"Pawel_Michael_layout finished!!!"<<endl;
    void    print_all_vertex();
}




