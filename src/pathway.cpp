#include<iostream>
#include<stdlib.h>
#include<string>
#include<fstream>
#include"pathway.h"
#include"tokenSeparator.h"
#include"forceDirectedLayout/layout.h"
#include"filedownloader.h"

#include<QXmlStreamReader>

using namespace std;

Node*
findNodeByName( vector<Node>& nodeVec, const string& name){
    for( vector<Node>::iterator it=nodeVec.begin(); it!=nodeVec.end(); it++ ){
        if( it->get_name() == name ){
            return &(*it);
        }
    }

    cerr<<"warnning!!! can not find node:"<<name<<endl;
    return NULL;
}

Node*
findNodeByIndex( vector<Node>& nodeVec, string& ind){
    for( vector<Node>::iterator it=nodeVec.begin(); it!=nodeVec.end(); it++ ){
        if( it->get_index() == atoi(ind.c_str()) ){
            return &(*it);
        }
    }

    cerr<<"warnning!!! can not find node:"<<ind<<endl;
    return NULL;
}

size_t
findNodeIndex( vector<Node>& nodeVec, const string& name ){
    for( vector<Node>::iterator it=nodeVec.begin(); it!=nodeVec.end(); it++ ){
        if( it->get_name() == name ){
            return it-nodeVec.begin();
        }
    }
}

//--------------------------------------------------------------

Edge::Edge( const Edge &e){
    parent=e.parent;
    child=e.child;
    type=e.type;
    value=e.value;
}

void
Edge::print(){

    cout<<parent->get_name()<<"\t"<<child->get_name()<<"\t"<<type<<"\t"<<value<<endl;
}

float
Edge::get_length2D(){
	float	x1=parent->get_x();
	float	y1=parent->get_y();
	float	x2=child->get_x();
	float	y2=child->get_y();
	return sqrt( (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2) );
}

//--------------------------------------------------------------

Pathway::Pathway(const Pathway &p){
	nodeVec = p.nodeVec;
	edgeVec = p.edgeVec;
	range = p.range;
	minX = p.minX;
	minY = p.minY;
	maxX = p.maxX;
	maxY = p.maxY;
}

Pathway& Pathway::operator =(  Pathway& p){
    vector<Node>    nv=p.get_nodeVec();
    for(size_t i=0; i<p.get_nodeVec().size(); i++){
        nodeVec.push_back( p.get_nodeVec()[i] );
    }
    vector<Edge> ev=p.get_edgeVec();
    for( size_t i=0; i<p.get_edgeVec().size(); i++ ){
        size_t i1=ev[i].get_parent()->get_index();
        size_t i2=ev[i].get_child()->get_index();
        size_t ip, ic;
        ip = ic =0;
        for( size_t j=0; j<nodeVec.size(); j++ ){
        	if( i1 == nodeVec[j].get_index() ){
        		ip = j;
        	}
        	if( i2 == nodeVec[j].get_index() ){
        		ic = j;
        	}
        }

        Edge e(ev[i]);
        e.set_parent(&nodeVec[ip]);
        e.set_child(&nodeVec[ic]);
        edgeVec.push_back(e);
    }
    range = p.range;
    minX = p.minX;
    minY = p.minY;
    maxX = p.maxX;
    maxY = p.maxY;
}

int
Pathway::readPathwayFile_pv(const char *fileName){
    ifstream input;
    input.open(fileName);
    if(!input){
        cerr<<"error!!! can not open pathway file:"<<fileName<<endl;
    }

    string line;
    size_t index=0;
    nodeVec.clear();
    edgeVec.clear();
    getline(input,line);
    vector< pair<string, string> > pairVec;
    size_t  count=0;
    while (getline(input,line)) {
        count++;
        if(count%100==0){ cout<<count<<endl; }
        vector<string> strVec=tokenByTab(line);
        string      symbolA=strVec[1];
        int         typeA=atoi(strVec[2].c_str());
        NodeType    tA=protein;
        float       xA=atof(strVec[3].c_str());
        float       yA=atof(strVec[4].c_str());
        float       zA=atof(strVec[5].c_str());
        string      symbolB=strVec[6];
        int         typeB=atoi(strVec[7].c_str());
        NodeType    tB=protein;
        float       xB=atof(strVec[8].c_str());
        float       yB=atof(strVec[9].c_str());
        float       zB=atof(strVec[10].c_str());
        string      edgeType=strVec[11];

        if(findNodeByName(nodeVec, symbolA)==NULL ){
            Node nA(tA, symbolA, index++, xA, yA, zA);
            nodeVec.push_back(nA);
        }
        if(findNodeByName(nodeVec, symbolB)==NULL){
            Node nB(tB, symbolB, index++,xB, yB, zB);
            nodeVec.push_back(nB);
        }
        pair<string, string> p=make_pair(symbolA, symbolB);
        pairVec.push_back(p);
    }
    for( size_t i=0; i<pairVec.size(); i++ ){
        Node *pA=findNodeByName(nodeVec, pairVec[i].first);
        Node *pB=findNodeByName(nodeVec, pairVec[i].second);
        Edge e(pA,pB,promote);
        edgeVec.push_back(e);
    }
    return 1;
}

int
Pathway::readBioPlexInteractionList(const char* fileName){
    ifstream input;
    input.open(fileName);
    if(!input){
        cerr<<"error!!! can not open pathway file:"<<fileName<<endl;
    }

    string line;
    size_t index=0;
    nodeVec.clear();
    edgeVec.clear();
    getline(input,line);
    vector< pair<string, string> > pairVec;
    while (getline(input,line)) {
        vector<string> strVec=tokenByTab(line);
        string SymbolA=strVec[4];
        string SymbolB=strVec[5];
        Node*   pA = findNodeByName(nodeVec, SymbolA );
        if(pA == NULL){
            Node n(protein, SymbolA, index++);
            nodeVec.push_back(n);
        }
        Node*   pB = findNodeByName(nodeVec, SymbolB);
        if(pB == NULL){
            Node n(protein, SymbolB, index++);
            nodeVec.push_back(n);
        }
        pair<string,string> p=make_pair(SymbolA, SymbolB);
        pairVec.push_back(p);
    }
    for( size_t i=0; i<pairVec.size(); i++ ){
        Node *pA=findNodeByName(nodeVec, pairVec[i].first);
        Node *pB=findNodeByName(nodeVec, pairVec[i].second);
        Edge e(pA,pB,promote);
        edgeVec.push_back(e);
    }
    generateNodePos();
    return 1;
}

int
Pathway::readKeggPathwayOnline(const char* url){
//	QUrl imageUrl("http://www.kegg.jp/kegg-bin/download?entry=hsa00010&format=kgml");

	cout<<"$$$$$$$$$$44"<<endl;

	return 0;
}

int
Pathway::readReactionFile(const char* fileName){
	ifstream input;
	input.open(fileName);
	if(!input){
        cerr<<"error!!! can not open pathway file:"<<fileName<<endl;
	}

	string line;
	size_t index=0;
	nodeVec.clear();
	edgeVec.clear();
    vector< pair<string, string> > pairVec;

    while(getline(input,line)){
    	vector<string>	strVec=tokenByTab(line);
    	if( strVec.size()==3 ){
    		int type=atoi(strVec[1].c_str());
    		Node n( NodeType(type), strVec[0], index++);
    		nodeVec.push_back(n);
    	}else if( strVec.size()==2 ){
    		string parent=strVec[0];
    		string child=strVec[1];

    		Node* p_parent=findNodeByName(nodeVec, parent);
    		Node* p_child=findNodeByName(nodeVec, child);
    		if( p_parent!=NULL && p_child!=NULL ){
    			Edge e(p_parent, p_child, promote);
    			edgeVec.push_back(e);
    		}
    	}
    }
    generateNodePos();
}

int
Pathway::readPathwayFile(const char* fileName){
    ifstream input;
    input.open(fileName);
    if(!input){
        cerr<<"error!!! can not open pathway file:"<<fileName<<endl;
    }

    string line;
    size_t index=0;
    nodeVec.clear();
    edgeVec.clear();
    bool	hasPos=false;
    while(getline(input, line)){
        vector<string> strVec=tokenByTab(line);
        if(strVec.size()==2 || strVec.size()==6 || strVec.size()==3){
            string type=strVec.front();
            float x=0;
            float y=0;
            float z=0;
            string	color("black");
            if( strVec.size()==6 ){
            	color = strVec[5];
            	x=atof(strVec[2].c_str());
            	y=atof(strVec[3].c_str());
            	z=atof(strVec[4].c_str());
            	hasPos=true;
            }
            if( strVec.size()==3 ){
            	color = strVec[2];
            }

            if(type==string("protein")){
                Node n( protein, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                n.set_color(color);
                nodeVec.push_back(n);

            }else   if(type==string("complex")){
                Node n( complex, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else   if(type==string("abstract")){
                Node n( abstract, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else   if(type==string("family")){
                Node n( family, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else   if(type==string("miRNA")){
                Node n( miRNA, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else   if(type==string("rna")){
                Node n( rna, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else   if(type==string("chemical")){
                Node n( chemical, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);

            }else{
                cerr<<"warnning!!! unidentified:"<<type<<endl;
                Node n( unknown, strVec[1], index++ );
                n.set_x(x);n.set_y(y);n.set_z(z);
                nodeVec.push_back(n);
            }
        }else if( strVec.size() > 3 ){
            string  type = strVec.back();
            string  parent = strVec[0];
            Node*   p_parent;
            p_parent = findNodeByName(nodeVec, parent );

            string  child = strVec[1];
            Node*   p_child = findNodeByName(nodeVec, child );
            float	val;
            if( strVec.size()>3 ){
            	val= atof( strVec[3].c_str() );
            }
            if(type=="-a>"){
                Edge e(p_parent, p_child, promote);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="-a|"){
                Edge e(p_parent, p_child, inhibit);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="-t>"){
                Edge e(p_parent, p_child, trans_promote);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="-t|"){
                Edge e(p_parent, p_child, trans_inhibit);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="-ap>"){
                Edge e(p_parent, p_child, process_promote);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="-ap|"){
                Edge e(p_parent, p_child, process_inhibit);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="component>"){
                Edge e(p_parent, p_child, component);
                e.set_value(val);
                edgeVec.push_back(e);
            }else             if(type=="member>"){
            	Edge e(p_parent, p_child, member);
            	e.set_value(val);
            	edgeVec.push_back(e);
            }else{
            	Edge e(p_parent, p_child, NA);
            	e.set_value(val);
            	edgeVec.push_back(e);
            }
        }
    }
    if(!hasPos){
    	generateNodePos_withEdgeLengthFixed();
//    	generateNodePos();
    }else{
        maxX = -10e10;
        maxY = -10e10;
        minX = 10e10;
        minY = 10e10;
	    //set node coordinates
	    for( size_t i=0; i<nodeVec.size(); i++ ){
	        float x = nodeVec[i].get_x();
	        float y = nodeVec[i].get_y();

			maxX = maxX > x ? maxX : x;
			maxY = maxY > y ? maxY : y;
			minX = minX < x ? minX : x;
			minY = minY < y ? minY : y;
	    }
	}
}

int
Pathway::get_keggPathway( const KGML_pathway path, bool d3  ){
	if( path.isEmpty() ){
		return 0;
	}
	vector<KGML_entry>		entryVec = path.entryVec;
	vector<KGML_reaction>	reactionVec = path.reactionVec;
	vector<KGML_relation>	relationVec = path.relationVec;

    nodeVec.clear();
    edgeVec.clear();
    maxX = -10e10;
    maxY = -10e10;
    minX = 10e10;
    minY = 10e10;
    cout<<"entryVec"<<entryVec.size()<<endl;
	for( size_t i=0; i<entryVec.size(); i++ ){

		KGML_entry	en = entryVec[i];

		//true when node is exist in the reaction
		bool flag = false;
		for( size_t j=0; j<reactionVec.size(); j++ ){
			if( reactionVec[j].id == en.id ){
				flag = true;
				break;
			}
			KGML_reaction	re = reactionVec[j];
			vector<KGML_substrate>		subVec = re.substrateVec;
			vector<KGML_product>		prodVec = re.productVec;
			for( size_t k=0; k<subVec.size(); k++ ){
				if( subVec[k].id == en.id ){
					flag = true;
					break;
				}
			}
			for( size_t k=0; k<prodVec.size(); k++ ){
				if( prodVec[k].id == en.id ){
					flag = true;
					break;
				}
			}
		}
		//true when node is exist in the reaction
		for( size_t j=0; j<relationVec.size(); j++ ){
			if( relationVec[j].entry1 == en.id ||
					relationVec[j].entry2 == en.id  ){
				flag = true;
				break;
			}
			KGML_relation	re = relationVec[j];
			vector<KGML_subtype>		subVec = re.subtypeVec;
			for( size_t k=0; k<subVec.size(); k++ ){
				if( subVec[k].value == en.id ){
					flag = true;
					break;
				}
			}
		}

		if( flag && !en.graphicsVec[0].x.empty() && !en.graphicsVec[0].y.empty() ){
			string name = en.graphicsVec[0].name;
			float	x = atof(en.graphicsVec[0].x.c_str());
			float	y = atof(en.graphicsVec[0].y.c_str());

			maxX = maxX > x ? maxX : x;
			maxY = maxY > y ? maxY : y;
			minX = minX < x ? minX : x;
			minY = minY < y ? minY : y;
			if( en.type == string("compound") ){
				Node 	n( chemical, name, atoi(en.id.c_str()), x, y, 0);
				nodeVec.push_back( n );
			}else if( en.type == string("map") ){
				Node 	n( family, name, atoi(en.id.c_str()), x, y, 0);
				nodeVec.push_back( n );
			}else if( en.type == string("gene") ){
				Node 	n( protein, name, atoi(en.id.c_str()), x, y, 0);
				nodeVec.push_back( n );
			}else if( en.type == string("ortholog") ){
				Node 	n( protein, name, atoi(en.id.c_str()), x, y, 0);
				nodeVec.push_back( n );
			}else{
				Node 	n( unknown, name, atoi(en.id.c_str()), x, y, 0);
				nodeVec.push_back( n );
			}
		}
	}

	for( size_t i=0; i<reactionVec.size(); i++ ){
		KGML_reaction	re = reactionVec[i];
		vector<KGML_substrate>		subVec = re.substrateVec;
		vector<KGML_product>		prodVec = re.productVec;

		if(  subVec.size() > 0 || prodVec.size() > 0  ){
			KGML_entry en;
			for( size_t j=0; j<entryVec.size(); j++ ){
				if( entryVec[j].id == re.id ){
					en = entryVec[j];
				}
			}
			Node 	*p_n =  findNodeByIndex( nodeVec, re.id );
			if( p_n !=NULL ){
				for( size_t j=0; j<subVec.size(); j++ ){
					Node*	p_parent = findNodeByIndex( nodeVec, subVec[j].id );
					if( p_parent !=NULL && re.type == "irreversible"  ){
						Edge	e( p_parent, p_n, promote );
						edgeVec.push_back(e);
					}else if( p_parent !=NULL && re.type == "reversible"  ){
						Edge	e1( p_parent, p_n, promote );
						Edge	e2( p_n, p_parent, promote );
						edgeVec.push_back(e1);
						edgeVec.push_back(e2);
					}
				}
				for( size_t j=0; j<prodVec.size(); j++ ){
					Node*	p_child = findNodeByIndex( nodeVec, prodVec[j].id );
					if( p_child !=NULL &&  re.type == "irreversible" ){
						Edge	e( p_n, p_child, promote );
						edgeVec.push_back(e);
					}else if( p_child !=NULL && re.type == "reversible"  ){
						Edge	e1( p_child, p_n, promote );
						Edge	e2( p_n, p_child, promote );
						edgeVec.push_back(e1);
						edgeVec.push_back(e2);
					}
				}
			}
		}
	}

	if(reactionVec.empty()){
		for( size_t i=0; i<relationVec.size(); i++ ){
			KGML_relation	relation = relationVec[i];
			if( relation.type == string("maplink") ){
				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;

				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id && entryVec[j].type == string("map") ){
						Node*	p_parent = findNodeByIndex( nodeVec, s_e1 );
						vector<KGML_subtype>	subVec = relation.subtypeVec;
						Node* 	p_child = findNodeByIndex( nodeVec, s_e2 );
						if( p_parent !=NULL && p_child !=NULL ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
							break;
						}
					}else if( s_e2 == entryVec[j].id && entryVec[j].type == string("map") ){
						Node*	p_parent = findNodeByIndex( nodeVec, s_e2 );
						vector<KGML_subtype>	subVec = relation.subtypeVec;
						Node* 	p_child = findNodeByIndex( nodeVec, s_e1 );
						if( p_parent !=NULL && p_child !=NULL ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
							break;
						}
					}
				}
			}else if( relation.type == string("PPrel") ){

				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;
				Node*	p_parent;
				Node*	p_child;

				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id ){
						p_parent = findNodeByIndex( nodeVec, s_e1 );
					}else if( s_e2 == entryVec[j].id ){
						p_child = findNodeByIndex( nodeVec, s_e2 );
					}
				}
				if( p_parent !=NULL && p_child !=NULL ){
					if( !relation.subtypeVec.empty() ){
						if( relation.subtypeVec[0].name=="activation" ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
						}
						if( relation.subtypeVec[0].name=="inhibition" ){
							Edge	e( p_parent, p_child, inhibit );
							edgeVec.push_back( e );
						}
					}else{
						Edge	e( p_parent, p_child, NA );
						edgeVec.push_back( e );
					}
				}
			}else{
				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;
				Node*	p_parent;
				Node*	p_child;
				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id ){
						p_parent = findNodeByIndex( nodeVec, s_e1 );
					}else if( s_e2 == entryVec[j].id ){
						p_child = findNodeByIndex( nodeVec, s_e2 );
					}
				}
			    if( p_parent !=NULL && p_child != NULL ){
			    	Edge	e( p_parent, p_child, promote );
			    	edgeVec.push_back( e );
			    }
			}
		}
	}
	range = (maxX-minX)>(maxY-minY)?(maxX-minX):(maxY-minY);

	return 0;
}

int
Pathway::get_keggPathway( const KGML_pathway path, const vector<testResult> testVec, bool d3 ){
	if( path.isEmpty() ){
		return 0;
	}
	vector<KGML_entry>		entryVec = path.entryVec;
	vector<KGML_reaction>	reactionVec = path.reactionVec;
	vector<KGML_relation>	relationVec = path.relationVec;

    nodeVec.clear();
    edgeVec.clear();
    maxX = -10e10;
    maxY = -10e10;
    minX = 10e10;
    minY = 10e10;
	for( size_t i=0; i<entryVec.size(); i++ ){
		KGML_entry	en = entryVec[i];

		//true when node is exist in the reaction
		bool flag = false;
		for( size_t j=0; j<reactionVec.size(); j++ ){
			if( reactionVec[j].id == en.id ){
				flag = true;
				break;
			}

			KGML_reaction	re = reactionVec[j];
			vector<KGML_substrate>		subVec = re.substrateVec;
			vector<KGML_product>		prodVec = re.productVec;
			for( size_t k=0; k<subVec.size(); k++ ){
				if( subVec[k].id == en.id ){
					flag = true;
					break;
				}
			}
			for( size_t k=0; k<prodVec.size(); k++ ){
				if( prodVec[k].id == en.id ){
					flag = true;
					break;
				}
			}
		}
		//true when node is exist in the reaction
		for( size_t j=0; j<relationVec.size(); j++ ){
			if( relationVec[j].entry1 == en.id ||
					relationVec[j].entry2 == en.id  ){
				flag = true;
				break;
			}
			KGML_relation	re = relationVec[j];
			vector<KGML_subtype>		subVec = re.subtypeVec;
			for( size_t k=0; k<subVec.size(); k++ ){
				if( subVec[k].value == en.id ){
					flag = true;
					break;
				}
			}
		}

		if( flag  && !en.graphicsVec[0].x.empty() && !en.graphicsVec[0].y.empty()  ){
			string name = en.graphicsVec[0].name;
			float	x = atof(en.graphicsVec[0].x.c_str());
			float	y = atof(en.graphicsVec[0].y.c_str());
			maxX = maxX > x ? maxX : x;
			maxY = maxY > y ? maxY : y;
			minX = minX < x ? minX : x;
			minY = minY < y ? minY : y;

			vector<string>		geneVec = tokenByComma(name);
			float 				testValue = 0;
			float 				testpValue = -1;
			bool				tempFlag = false;
			for( size_t n=0; n<testVec.size(); n++ ){
				for( size_t m=0; m<geneVec.size(); m++ ){
					if( geneVec[m]==testVec[n].geneName ){
						testValue = testVec[n].value;
						testpValue = testVec[n].pValue;
						tempFlag = true;
						break;
					}
				}
			}
			if( !tempFlag ){
				cout<<"!!! can not find:";
				for( size_t nameIndex = 0; nameIndex<geneVec.size();nameIndex++ ){
					cout<<geneVec[nameIndex]<<"\t";
				}
				cout<<endl;
			}
			if( en.type == string("compound") ){
				Node 	n( chemical, name, atoi(en.id.c_str()), x, y, 0);
				n.set_value(testValue);
				n.set_pvalue(testpValue);
				nodeVec.push_back( n );
			}else if( en.type == string("map") ){
				Node 	n( family, name, atoi(en.id.c_str()), x, y, 0);
				n.set_value(testValue);
				n.set_pvalue(testpValue);
				nodeVec.push_back( n );
			}else if( en.type == string("gene") ){
				Node 	n( protein, name, atoi(en.id.c_str()), x, y, 0);
				n.set_value(testValue);
				n.set_pvalue(testpValue);
				nodeVec.push_back( n );
			}else if( en.type == string("ortholog") ){
				Node 	n( protein, name, atoi(en.id.c_str()), x, y, 0);
				n.set_value(testValue);
				n.set_pvalue(testpValue);
				nodeVec.push_back( n );
			}else{
				Node 	n( unknown, name, atoi(en.id.c_str()), x, y, 0);
				n.set_value(testValue);
				n.set_pvalue(testpValue);
				nodeVec.push_back( n );
			}
		}
	}


	for( size_t i=0; i<reactionVec.size(); i++ ){
		KGML_reaction	re = reactionVec[i];
		vector<KGML_substrate>		subVec = re.substrateVec;
		vector<KGML_product>		prodVec = re.productVec;

		if(  subVec.size() > 0 || prodVec.size() > 0  ){
			KGML_entry en;
			for( size_t j=0; j<entryVec.size(); j++ ){
				if( entryVec[j].id == re.id ){
					en = entryVec[j];
				}
			}
			Node 	*p_n =  findNodeByIndex( nodeVec, re.id );
			if( p_n !=NULL ){
				for( size_t j=0; j<subVec.size(); j++ ){
					Node*	p_parent = findNodeByIndex( nodeVec, subVec[j].id );
					if( p_parent !=NULL && re.type == "irreversible"  ){
						Edge	e( p_parent, p_n, promote );
						edgeVec.push_back(e);
					}else if( p_parent !=NULL && re.type == "reversible"  ){
						Edge	e1( p_parent, p_n, promote );
						Edge	e2( p_n, p_parent, promote );
						edgeVec.push_back(e1);
						edgeVec.push_back(e2);
					}
				}
				for( size_t j=0; j<prodVec.size(); j++ ){
					Node*	p_child = findNodeByIndex( nodeVec, prodVec[j].id );
					if( p_child !=NULL &&  re.type == "irreversible" ){
						Edge	e( p_n, p_child, promote );
						edgeVec.push_back(e);
					}else if( p_child !=NULL && re.type == "reversible"  ){
						Edge	e1( p_child, p_n, promote );
						Edge	e2( p_n, p_child, promote );
						edgeVec.push_back(e1);
						edgeVec.push_back(e2);
					}
				}
			}
		}
	}

	if(reactionVec.empty()){
		for( size_t i=0; i<relationVec.size(); i++ ){
			KGML_relation	relation = relationVec[i];
			if( relation.type == string("maplink") ){
				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;

				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id && entryVec[j].type == string("map") ){
						Node*	p_parent = findNodeByIndex( nodeVec, s_e1 );
						vector<KGML_subtype>	subVec = relation.subtypeVec;
						Node* 	p_child = findNodeByIndex( nodeVec, s_e2 );
						if( p_parent !=NULL && p_child !=NULL ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
							break;
						}
					}else if( s_e2 == entryVec[j].id && entryVec[j].type == string("map") ){
						Node*	p_parent = findNodeByIndex( nodeVec, s_e2 );
						vector<KGML_subtype>	subVec = relation.subtypeVec;
						Node* 	p_child = findNodeByIndex( nodeVec, s_e1 );
						if( p_parent !=NULL && p_child !=NULL ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
							break;
						}
					}
				}
			}else if( relation.type == string("PPrel") ){
				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;
				Node*	p_parent;
				Node*	p_child;

				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id ){
						p_parent = findNodeByIndex( nodeVec, s_e1 );
					}else if( s_e2 == entryVec[j].id ){
						p_child = findNodeByIndex( nodeVec, s_e2 );
					}
				}
				if( p_parent !=NULL && p_child !=NULL ){
					if( !relation.subtypeVec.empty() ){
						if( relation.subtypeVec[0].name=="activation" ){
							Edge	e( p_parent, p_child, promote );
							edgeVec.push_back( e );
						}
						if( relation.subtypeVec[0].name=="inhibition" ){
							Edge	e( p_parent, p_child, inhibit );
							edgeVec.push_back( e );
						}
					}else{
						Edge	e( p_parent, p_child, NA );
						edgeVec.push_back( e );
					}
				}
			}else{
				string	s_e1 = relation.entry1;
				string	s_e2 = relation.entry2;
				Node*	p_parent;
				Node*	p_child;
				//get map entry1 or entry2 and take it as map node
				for( size_t j=0; j<entryVec.size(); j++ ){
					if( s_e1 == entryVec[j].id ){
						p_parent = findNodeByIndex( nodeVec, s_e1 );
					}else if( s_e2 == entryVec[j].id ){
						p_child = findNodeByIndex( nodeVec, s_e2 );
					}
				}
			    if( p_parent !=NULL && p_child != NULL ){
			    	Edge	e( p_parent, p_child, promote );
			    	edgeVec.push_back( e );
			    }
			}
		}
	}
	range = (maxX-minX)>(maxY-minY)?(maxX-minX):(maxY-minY);

	if( !edgeVec.empty() && d3 ){
		generateNodePos();
	}
	return 0;
}

/**
 * fix parent postion, and random put child position with fixed length to its parent
 */
bool
Pathway::generateNodePos_withEdgeLengthFixed(){
	for( size_t i=0; i<edgeVec.size(); i++ ){
		edgeVec[i].get_parent()->set_x(0);
		edgeVec[i].get_parent()->set_y(0);
		edgeVec[i].get_parent()->set_z(0);

		float length=edgeVec[i].get_value();

		float x=((float)rand()/RAND_MAX)*edgeVec[i].get_value();
		float y=sqrt(length*length-x*x);
		if(i%2==0){
			x=-((float)rand()/RAND_MAX)*edgeVec[i].get_value();
			y=-sqrt(length*length-x*x);
		}
		edgeVec[i].get_child()->set_x(x);
		edgeVec[i].get_child()->set_y(y);
		edgeVec[i].get_child()->set_z(0);
		cout<<edgeVec[i].get_child()->get_name()<<"\t"<<x<<"\t"<<y<<endl;
	}

    maxX=maxY=-10e10;
    minX=minY=10e10;

    //set node coordinates
    for( size_t i=0; i<nodeVec.size(); i++ ){
    	float x=nodeVec[i].get_x();
    	float y=nodeVec[i].get_y();
    	float z=nodeVec[i].get_z();

		maxX = maxX > x ? maxX : x;
		maxY = maxY > y ? maxY : y;
		minX = minX < x ? minX : x;
		minY = minY < y ? minY : y;
    }
    cout<<"maxX:"<<maxX<<" minX:"<<minX<<endl;
    cout<<"maxY:"<<maxY<<" minY:"<<minY<<endl;
	return true;
}

bool
Pathway::generateNodePos(){

    vector<Vertex> fdlVertexVec;
    for( size_t i=0; i<nodeVec.size(); i++ ){
        stringstream ss;
        ss<<i;
        Vertex v(ss.str());
        v.set_name( nodeVec[i].get_name() );
        fdlVertexVec.push_back(v);
    }

    vector<Link> fdlLinkVec;
//    vector<Link> fdlLinkVec(edgeVec.size());
    for( size_t i=0; i<edgeVec.size(); i++ ){
        Edge edge = edgeVec[i];
        size_t indexP=0;
        size_t indexC=0;

        size_t p=edge.get_parent()->get_index();
        size_t c=edge.get_child()->get_index();

        for( size_t j=0; j<nodeVec.size(); j++ ){
        	if( p == nodeVec[j].get_index() ){
        		indexP = j;
        	}
        	if( c == nodeVec[j].get_index() ){
        		indexC = j;
        	}
        }

        Link	link(fdlVertexVec[indexP], fdlVertexVec[indexC],edge.get_value() );
        fdlLinkVec.push_back( link );

    }

    cout<<fdlLinkVec.size()<<endl;
    Layout ly( fdlVertexVec, fdlLinkVec);

    if(!ly.layout()){
        cerr<<"can not layout graph"<<endl;
    }

    maxX=maxY=-10e10;
    minX=minY=10e10;

    //set node coordinates
    for( size_t i=0; i<nodeVec.size(); i++ ){
        Coord c=ly.get_vertex_vector()[i].get_pos();
        nodeVec[i].set_x(c.x);
        nodeVec[i].set_y(c.y);
        nodeVec[i].set_z(c.z);

		maxX = maxX > c.x ? maxX : c.x;
		maxY = maxY > c.y ? maxY : c.y;
		minX = minX < c.x ? minX : c.x;
		minY = minY < c.y ? minY : c.y;
    }
}

bool
Pathway::geneNameMatch(const string &s1, const string &s2){
    string str1=s1.substr(0, s1.find("|"));
    string str2=s2.substr(0, s2.find("|"));

    if( str1 == str2 ){
        return true;
    }else{
        return false;
    }
}

vector<pair<string, float> >
Pathway::readGeneValue(const char* fileName){

    ifstream    input;
    input.open(fileName);
    if(!input){
        cerr<<"error!!! can not open pathway file:"<<fileName<<endl;
    }
    string line;
    size_t index=0;
    vector<pair<string, float> > pairVec;
    size_t count = 0;
    while( getline(input, line) ){
        vector<string>  strVec = tokenBySpace(line);
        if(strVec.size() < 2){
            cerr<<"error: "<<line<<endl;
        }else{
            string  geneName = strVec[0];
            float   geneValue = atof(strVec[1].c_str());
            pair<string, float> p=make_pair(geneName, geneValue);
            pairVec.push_back(p);

            //set gene node value
            for( size_t i=0; i<nodeVec.size(); i++ ){
                string name = nodeVec[i].get_name();
                if( geneNameMatch(name, geneName) ){
                    nodeVec[i].set_value(geneValue);

                    count++;
                    break;
                }
            }
        }
    }

    return pairVec;
}

bool
Pathway::remove_singletonNodes(){
	vector<Node> newNodeVec;

	for( size_t i=0; i<edgeVec.size(); i++ ){

		Node parent(edgeVec[i].get_parent());
		Node child(edgeVec[i].get_child());
		bool flag1,flag2;
		flag1=flag2=true;
		for( size_t i=0; i<newNodeVec.size(); i++ ){
			if( newNodeVec[i].get_name() == parent.get_name() ){
				flag1=false;
			}
			if(newNodeVec[i].get_name()==child.get_name() ){
				flag2=false;
			}
		}
		if( flag1 ){
			newNodeVec.push_back(parent);
		}
		if(flag2){
			newNodeVec.push_back(child);
		}
	}
	nodeVec.clear();
	for( size_t i=0; i<newNodeVec.size(); i++ ){
		nodeVec.push_back(newNodeVec[i]);
	}

	for( size_t j=0; j<edgeVec.size(); j++ ){

		string p=edgeVec[j].get_parent()->get_name();
		string c=edgeVec[j].get_child()->get_name();
		int index_p, index_c;
		Node* p_p=findNodeByName(nodeVec, p);
		Node* p_c=findNodeByName(nodeVec, c);
		edgeVec[j].set_parent(p_p);
		edgeVec[j].set_child(p_c);
	}

	for( size_t i=0; i<edgeVec.size(); i++ ){
		edgeVec[i].print();
	}
}

void
Pathway::printToFile(string fileName){
    ofstream off(fileName.c_str());
    for( size_t i=0; i<nodeVec.size(); i++ ){
    	string	t="-";
    	switch(nodeVec[i].get_type()){
    	case protein: t="protein";
    	}
    	off<<t<<"\t"<<nodeVec[i].get_name()<<"\t"<<
    			nodeVec[i].get_x()<<"\t"<<nodeVec[i].get_y()<<"\t"<<nodeVec[i].get_z()<<"\t"<<nodeVec[i].get_color()<<endl;
    }
    for( size_t i=0; i<edgeVec.size(); i++ ){

        off<<edgeVec[i].get_parent()->get_name();
        off<<"\t"<<edgeVec[i].get_child()->get_name();
        off<<"\t"<<"-";
        off<<"\t"<<edgeVec[i].get_value()<<endl;
    }
    off.close();
}

void
Pathway::print(){
	cout<<"index\tSymbolA\ttypeA\txA\tyA\tzA\tSymbolB\ttypeB\txB\tyB\tzB\tedgeType"<<endl;
    for( size_t i=0; i<edgeVec.size(); i++ ){
        cout<<i<<"\t"<<edgeVec[i].get_parent()->get_name();
        cout<<"\t"<<edgeVec[i].get_parent()->get_type();
        cout<<"\t"<<edgeVec[i].get_parent()->get_x();
        cout<<"\t"<<edgeVec[i].get_parent()->get_y();
        cout<<"\t"<<edgeVec[i].get_parent()->get_z();
        cout<<"\t"<<edgeVec[i].get_child()->get_name();
        cout<<"\t"<<edgeVec[i].get_child()->get_type();
        cout<<"\t"<<edgeVec[i].get_child()->get_x();
        cout<<"\t"<<edgeVec[i].get_child()->get_y();
        cout<<"\t"<<edgeVec[i].get_child()->get_z();
        cout<<"\t"<<edgeVec[i].get_type()<<endl;
    }
	cout<<"edgeVec: "<<edgeVec.size()<<endl;
}


