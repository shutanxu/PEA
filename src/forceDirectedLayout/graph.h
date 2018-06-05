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

#ifndef __VRETEX_H
#define __VERTEX_H

#include <string>
#include <stdlib.h>
#include <map>
#include<vector>
#include<math.h>

using namespace std;

struct Coord{
    Coord (const float& a, const float& b, const float& c):x(a),y(b),z(c){}
    Coord(){    x=rand() % 10000000 / 1000000.0f;
                y=rand() % 10000000 / 1000000.0f;
                z=rand() % 10000000 / 1000000.0f; }
    Coord( const Coord& c ){
        x=c.x; y=c.y; z=c.z;
    }
    Coord& operator =(const Coord& c){ x=c.x; y=c.y; z=c.z; }

    friend  Coord operator+(const Coord& c1, const Coord& c2);
    friend  Coord operator-(const Coord& c1, const Coord& c2);
    friend  Coord operator*(const Coord& c1, const float& f);
    friend  Coord operator/(const Coord& c1, const float& f);

    float
    distTo( const Coord& c2){
        return sqrt( (x-c2.x)*(x-c2.x) +
                     (y-c2.y)*(y-c2.y) +
                     (z-c2.z)*(z-c2.z) );
    }

    void    print();
    void    set_x(float v){ x=v; }
    void    set_y(float v){ y=v; }
    void    set_z(float v){ z=v; }
    float   get_dis(){ return sqrt(x*x+y*y+z*z); }
    float   get_dis2D(){ return sqrt(x*x+y*y); }
    float   x;
    float   y;
    float   z;
};

//------------------------------------------------------------------------

class Vertex{
public:
    Vertex(string index);
    Vertex( const Vertex& v );
    Coord   get_pos(){ return pos; }
    Coord   get_disp(){ return disp; }
    string  get_id(){ return id; }
    string	get_name(){ return name; }
    int     get_depth(){ return depth; }
    void    set_pos( float x, float y, float z);
    void    set_pos( Coord c){ pos=c; }
    void    set_disp( float x, float y, float z);
    void    set_disp( Coord c ){ disp=c; }
    void    set_depth(int d){ depth=d; }
    void	set_x( float v ){ pos.x=v; }
    void	set_y( float v ){ pos.y=v; }
    void	set_z( float v ){ pos.z=v; }
    void	set_name( string s ){ name = s; }
    void    add_depth( ){ depth=depth+1; }
    void    print();
    bool	isVisited(){ return visited; }
    void    set_visited(){ visited=true; }
private:
    string  id;
    string	name;
    Coord   pos;
    Coord   disp;
    bool	visited;
    int		degree;		//num of vertexes connected with this vertex
    int     depth;
};

class Link{
public:
    Link();
    Link( const Link &l );
    Link( Vertex vs, Vertex vt, float v ){ source=&vs; target=&vt; value=v; source_id=vs.get_id(); target_id=vt.get_id(); }
    Link    operator=(const Link& l){
        id=l.id; value=l.value; source=l.source; target=l.target; source_id=l.source_id; target_id=l.target_id; }
    void    set_source(Vertex* v){ source = v; source_id=v->get_id(); }
    void    set_source_disp(Coord c){source->set_disp(c.x,c.y,c.z);}
    void    set_target(Vertex* v){ target = v; target_id=v->get_id(); }
    void    set_target_disp(Coord c){target->set_disp(c.x,c.y,c.z);}
    void	set_value( float v ){ value = v; }
    void	set_sourceX(float v){ source->set_x(v); }
    void	set_sourceY(float v){ source->set_y(v); }
    void	set_sourceZ(float v){ source->set_z(v); }
    void	set_targetX(float v){ target->set_x(v); }
    void	set_targetY(float v){ target->set_y(v); }
    void	set_targetZ(float v){ target->set_z(v); }

    string  get_id(){ return id; }
    float	get_dist();
    float	get_value(){ return value; }
    Vertex* get_source(){ return source; }
    Vertex* get_target(){ return target; }
    string  get_source_id(){ return source_id; }
    string  get_target_id(){ return target_id; }
private:
    string  id;
    float	value;
    Vertex* source;
    Vertex* target;
    string  source_id;
    string  target_id;
};

void
update_link( vector<Vertex>& ver, vector<Link>& link );

/**
 * breadth(broad) first search
 * return vector of pairs, each pair contains vertex index and length to root
 */
vector< Vertex >
bfs( vector<Vertex>& ver, vector<Link>& linkVec, string& root, const int& maxDepth );


#endif
