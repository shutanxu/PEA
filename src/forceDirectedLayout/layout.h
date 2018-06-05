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

#ifndef __LAYOUT_H
#define __LAYOUT_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

#include "graph.h"

using namespace std;

class Layout{
public:
    Layout(vector<Vertex>& v, vector<Link>& l );
    bool 			layout();
    bool 			layout2();
    bool			layout_SubSet( vector<size_t> indexVec, int maxIter );
    bool			layout_SubSet2( int maxIter );
    bool			layout_SubSet(vector<size_t> indexVec, vector<size_t> fixedIndexVec, int maxIter );

    vector<Vertex>  get_vertex_vector(){ return verVec; }
    vector<Link>    get_link_vector(){ return linkVec; }
    void			updateGraphDistMatrix();
    void 			MISfilteration();
    void			Pawel_Michael_layout();

private:
    void    normalScale();
    Vertex* find_vertex(string);
    float   fa(const float& x, const float& k);
    float   fr(const float& x, const float& k);
    void    print_all_vertex();
    bool	layout_fixedEdgeLength();
    vector<Vertex>			get_connectedVertex( Vertex v );
    int						join_num( vector<Vertex> v1, vector<Vertex> v2 );
private:
    vector<Vertex>      	verVec;
    vector<Link>        	linkVec;
    vector< vector<int> > 	graphDistMatrix;
	vector<vector<size_t> > filterationIndexVec;
    int                 	frameWidth;
    int                 	frameLength;
    float                	area;
    float               	K;
    int                 	max_iteration;
};

#endif


