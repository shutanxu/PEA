/*
 * Copyright 2014 Jason Graves (GodLikeMouse/Collaboradev)
 * http://www.collaboradev.com
 *
 * This file is part of ForceDirectedLayout.
 *
 * ForceDirectedLayout is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * ForceDirectedLayout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ForceDirectedLayout. If not, see http://www.gnu.org/licenses/.
 */

#include "graph.h"
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <time.h>

void
Coord::print(){
    cout.width(13);
    cout<<left<<x;
    cout.width(13);
    cout<<left<<y;
    cout.width(13);
    cout<<left<<z<<endl;
}

Coord
operator+(const Coord& c1, const Coord& c2){
    float   x=c1.x+c2.x;
    float   y=c1.y+c2.y;
    float   z=c1.z+c2.z;
    return  Coord(x,y,z);
}

Coord
operator-(const Coord& c1, const Coord& c2){
    float   x=c1.x-c2.x;
    float   y=c1.y-c2.y;
    float   z=c1.z-c2.z;
    return  Coord(x,y,z);
}

Coord
operator*(const Coord& c1, const float& f){
    float   x=c1.x*f;
    float   y=c1.y*f;
    float   z=c1.z*f;
    return  Coord(x,y,z);
}

Coord
operator/(const Coord& c1, const float& f){
    float   x=c1.x/f;
    float   y=c1.y/f;
    float   z=c1.z/f;
    return  Coord(x,y,z);
}

//-----------------------------------------------------

// Constructor
Vertex::Vertex(string index){
    id=index;
    float x1 = rand() % 10000000 / 10000000.0f;
    float y1 = rand() % 10000000 / 10000000.0f;
    float z1 = rand() % 10000000 / 10000000.0f;
    Coord p1(x1,y1,z1);

    float x2 = rand() % 10000000 / 10000000.0f;
    float y2 = rand() % 10000000 / 10000000.0f;
    float z2 = rand() % 10000000 / 10000000.0f;
    Coord p2(x2,y2,z2);

    pos = p1;
    disp= p2;

    visited=false;
    depth=0;
}

// copy constructor
Vertex::Vertex(const Vertex& v){
	id 		= v.id;
	name	= v.name;
	pos		= v.pos;
	disp	= v.disp;
	visited	= v.visited;
	depth	= v.depth;
}

void
Vertex::set_pos(float x, float y, float z){
    pos.set_x(x);
    pos.set_y(y);
    pos.set_z(z);
}

void
Vertex::set_disp(float x, float y, float z){
    disp.set_x(x);
    disp.set_y(y);
    disp.set_z(z);
}

void
Vertex::print(){
    cout<<id<<"\t";
    pos.print();
}

//-------------------------------------------------

Link::Link(){

}

Link::Link(const Link& l){
	id= l.id;
	source=NULL;
	target=NULL;
	source_id=l.source_id;
	target_id=l.target_id;
	value=l.value;
}

float
Link::get_dist(){
	Coord c1=source->get_pos();
	Coord c2=target->get_pos();
	float dis=c1.distTo(c2);
	return dis;
}

//---------------------------------------------------

Vertex
get_vertex( vector<Vertex>& verVec, const string& vertex  ){
	for( size_t i=0; i<verVec.size(); i++ ){
		if( verVec[i].get_id() == vertex ){
			return verVec[i];
		}
	}
}

void
update_link( vector<Vertex>& ver, vector<Link>& link ){

	for( size_t i=0; i<link.size(); i++ ){
		string si=link[i].get_source_id();
		string ti=link[i].get_target_id();
		int count=0;
		for( size_t j=0; j<ver.size(); j++ ){
			string id=ver[j].get_id();
			if( id == si ){
				link[i].set_source( &ver[j] );
				count++;
			}
			if( id == ti ){
				link[i].set_target( &ver[j] );
				count++;
			}
			if( count ==2 ){
				break;
			}
		}
		if( count !=2 ){
			cerr<<"can not find link:"<<i<<" in vertex vector"<<endl;
		}
	}
//	link[1].get_source()->print();
}

/**
 * maxDepth indicate the maximum depth from root to search
 */
vector< Vertex >
bfs( vector<Vertex>& ver, vector<Link>& link, string& root, const int& maxDepth ){

    vector<Vertex> 	verVec;
    size_t			verSize=ver.size();
    for(size_t i=0; i<verSize; i++){
        Vertex v(ver[i]);

        if( ver[i].get_id()!=root ){
        	//set the default depth of all vertex to root as maximum value, except for root itself as 0
        	v.set_depth(1e10);
        }
        verVec.push_back(v);
    }
    vector<Link>    linkVec;
    size_t			linkSize=link.size();
    for(size_t i=0; i<linkSize;i++){
    	Link l(link[i]);
    	linkVec.push_back( l );
    }

    //update vertex pointers in linkVec
    update_link( verVec, linkVec );

	queue<string> vertexQueue;
	vertexQueue.push(root);

	while( !vertexQueue.empty() ){
		string current=vertexQueue.front();
		vertexQueue.pop();
//		cout<<current<<endl;

		for( size_t i=0; i<linkSize; i++ ){
			if( linkVec[i].get_source_id() == current && !linkVec[i].get_target()->isVisited() ){

                linkVec[i].get_source()->set_visited();
                linkVec[i].get_target()->set_visited();

                int depth=linkVec[i].get_source()->get_depth()+1;
                linkVec[i].get_target()->set_depth( depth );

                if( depth < maxDepth ){
                	vertexQueue.push( linkVec[i].get_target_id() );
                }
            }else if( linkVec[i].get_target_id() == current && !linkVec[i].get_source()->isVisited() ){

            	linkVec[i].get_source()->set_visited();
                linkVec[i].get_target()->set_visited();

                int depth=linkVec[i].get_target()->get_depth()+1;
                linkVec[i].get_source()->set_depth(depth);

                if( depth < maxDepth ){
                	vertexQueue.push( linkVec[i].get_source_id() );
                }
            }
		}
	}
//    for( size_t i=0; i<verVec.size(); i++ ){
//    	ver[i].set_depth(verVec[i].get_depth() );
//    }

    if(0){
        for( size_t i=0; i<verVec.size(); i++ ){
            cout<<i<<"\t"<<verVec[i].get_id()<<"\t"<<verVec[i].get_depth()<<endl;
        }
    }

    return verVec;
}




