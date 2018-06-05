#ifndef PATHWAY_H
#define PATHWAY_H

#include<vector>
#include<iostream>
#include<string>
#include<stdlib.h>
#include"filedownloader.h"
#include"KGML.h"
#include"annotation.h"

using namespace std;

enum NodeType{protein=0, complex, abstract, family, miRNA, rna, chemical, unknown };

class Node{
public:
    Node(){ type=unknown; name=""; index=0; groupID=0; value=0; pvalue=-1; x=y=z=0; }
    Node( const NodeType& t, const string& n, const size_t& i)
        :type(t),name(n),index(i){ value = 0; pvalue=-1; x=y=z=0; }
    Node( const NodeType& t, const string& n, const size_t& i, const size_t& g)
        :type(t),name(n),index(i),groupID(g){ value = 0; pvalue=-1; x=y=z=0; }
    Node(const NodeType &t, const string &n, const size_t& i,
         const float& a, const float& b, const float& c ){
        type=t; name=n; x=a; y=b; z=c; index=i; value=0; pvalue=-1;
    }
    Node(const NodeType &t, const string &n, const size_t& i,const size_t& g,
         const float& a, const float& b, const float& c ){
        type=t; name=n; x=a; y=b; z=c; index=i; groupID=g;value=0;pvalue=-1;
    }
    Node( const Node* n ){
    	type=n->type; name=n->name; index=n->index; groupID=n->groupID;
    	value=n->value;pvalue=n->pvalue;x=n->x;y=n->y;z=n->z;color=n->color;
    }
    Node( const Node& n ){
    	type=n.type; name=n.name; index=n.index; groupID=n.groupID;
    	value=n.value;pvalue=n.pvalue;x=n.x;y=n.y;z=n.z;color=n.color;
    }

    NodeType    get_type(){ return type; }
    string      get_name(){ return name; }
    size_t      get_index(){ return index; }
    size_t		get_groupID(){ return groupID; }
    string		get_color(){ return color; }
    void        set_x(const float v){ x=v; }
    void        set_y(const float v){ y=v; }
    void        set_z(const float v){ z=v; }
    void		set_color( const string c ){ color=c; }
    void        set_value(const float v){ value = v; }
    void        set_pvalue(const float v){ pvalue = v; }

    const float get_x(){ return x; }
    const float get_y(){ return y; }
    const float get_z(){ return z; }
    const float get_value(){ return value; }
    const float get_pvalue(){ return pvalue; }
    void        print(){
    	string	t("");
    	if(type==protein){ t=string("protein");}
    	cout<<t<<"\t"<<name<<"\t"<<x<<"\t"<<y<<"\t"<<z<<endl; }
    Node&       operator=(const Node& n){ type=n.type; name=n.name; index=n.index; value=n.value; pvalue=n.pvalue; groupID=n.groupID; };
    Node*       operator=(const Node* n){ type=n->type; name=n->name; index=n->index; value=n->value; pvalue=n->pvalue; groupID=n->groupID; }
private:
    size_t      index;
    string      name;
    string		color;
    NodeType    type;
    size_t		groupID;
    float       value;
    float		pvalue;
    float       x;
    float       y;
    float       z;
};

Node*
findNodeByName( vector<Node>& nodeVec, const string& name);

Node*
findNodeByIndex( vector<Node>& nodeVec, string& ind);

enum EdgeType{promote, inhibit, trans_promote, trans_inhibit,
              process_promote, process_inhibit, component, member,NA };

class Edge{
public:
    Edge(){ parent=NULL; child=NULL; type=promote; }
    Edge( const Edge&);
    Edge( Node* p, Node* c, const EdgeType& t):
        parent(p),child(c),type(t){}
    Edge( Node* p, Node* c, const EdgeType& t, float v):
        parent(p),child(c),type(t),value(v){}
    void 		print();
    void        set_parent(Node* n){ parent=n; }
    void        set_child( Node* n ){ child=n; }
    void        set_type(EdgeType t){ type=t; }
    void		set_value( float v ){ value = v; }
    float		get_length2D();
    Node*       get_parent(){ return parent; }
    Node*       get_child(){ return child; }
    float		get_value(){ return value; }
    EdgeType    get_type(){ return type; }
    Edge&       operator=(const Edge& e){ parent=e.parent; child=e.child; type=e.type; value=e.value; }
    Edge&       operator=(const Edge* e){ parent=e->parent; child=e->child; type=e->type; value=e->value; }
private:
    Node*       parent;
    Node*       child;
    float		value;
    EdgeType    type;
};

class Pathway{
//	 Q_OBJECT
public:
    Pathway(){}
    Pathway(const Pathway&);
    Pathway& operator =(Pathway& p);
    int      						readPathwayFile_pv(const char *fileName);
    int     						readPathwayFile(const char* fileName);
    int								readReactionFile(const char* fileName);
    int     						readBioPlexInteractionList(const char* filenName);
    int								readKeggPathwayOnline(const char* url);
    vector<pair<string, float> > 	readGeneValue(const char* fileName);
    int								get_keggPathway( const KGML_pathway path, bool dimen3=false   );
    int								get_keggPathway( const KGML_pathway path, const vector<testResult> testVec, bool dimen3=false );
    vector<Node>    				get_nodeVec(){ return nodeVec; }
    const vector<Edge>    			get_edgeVec(){ return edgeVec; }
    float							get_range(){ return range; }
    float							get_minX(){ return minX; }
    float							get_minY(){ return minY; }
    float							get_maxX(){ return maxX; }
    float							get_maxY(){ return maxY; }
    void							set_node_coordX( size_t i,float x ){ nodeVec[i].set_x(x); }
    void							set_node_coordY( size_t i,float y ){ nodeVec[i].set_y(y); }
    void    						printToFile(string fileName);
    void							print();
    void							clear(){ edgeVec.clear(); nodeVec.clear(); }

private:
    bool    generateNodePos();
    bool	generateNodePos_withEdgeLengthFixed();
    bool    geneNameMatch( const string& n1, const string& n2 );
    bool	remove_singletonNodes();
    bool 	is_number(const std::string& s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it)) ++it;
        return !s.empty() && it == s.end();
    }
private:
    vector<Node>    nodeVec;
    vector<Edge>    edgeVec;
    float			range;		//max of maxX-minX or maxY-minY
    float			minX,minY,maxX,maxY;
};

#endif // PATHWAY_H
