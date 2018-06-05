/*
 * KGML.h
 *
 *  Created on: Jul 12, 2016
 *      Author: stan
 */

#ifndef KGML_H_
#define KGML_H_

#include<QMainWindow>
#include<QGridLayout>
#include<QtWidgets>
#include<QXmlStreamReader>
#include<QXmlSimpleReader>
#include<QXmlInputSource>
#include<QDomDocument>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<fstream>
#include"filedownloader.h"

using namespace std;

struct KGML_component{
	KGML_component(){}
	KGML_component(const string i):id(i){}
	void	print();
	string		id;
};

struct KGML_graphics{
	KGML_graphics(){}
	KGML_graphics(const string n, const string a, const string b, const string c,
			const string t, const string w, const string h, const string f):
				name(n),x(a),y(b),coords(c),type(t),width(w),height(h),fgcolor(f){}
	void	print();
	string		name;
	string		x;
	string		y;
	string		coords;
	string		type;
	string		width;
	string		height;
	string		fgcolor;
};

struct KGML_entry{
	KGML_entry(){}
	KGML_entry(const string i, const string n, const string t, const string l, const string r):
		id(i),name(n),type(t),link(l),reaction(r){ componentVec.clear(); graphicsVec.clear(); }

	void 		add_component( const KGML_component cp ){ componentVec.push_back(cp); }
	void 		add_graphics( const KGML_graphics gh ){ graphicsVec.push_back(gh); }
	void		print();
	string		id;
	string		name;
	string		type;
	string		link;
	string		reaction;

	vector<KGML_component>	componentVec;
	vector<KGML_graphics>	graphicsVec;
};

//--------------------------------------------

struct KGML_alt{
	KGML_alt(){}
	KGML_alt( const string n ):name(n){}
	void	print();
	string		name;
};

struct KGML_substrate{
	KGML_substrate(){}
	KGML_substrate(const string i, const string n):id(i),name(n){ altVec.clear(); }
	void		add_alt( const KGML_alt al ){ altVec.push_back(al); }
	void		print();
	string		id;
	string		name;

	vector<KGML_alt>	altVec;
};

struct KGML_product{
	KGML_product(){}
	KGML_product(const string i, const string n ):id(i),name(n){ altVec.clear(); }
	void		add_alt( const KGML_alt al ){ altVec.push_back(al); }
	void		print();
	string		id;
	string		name;

	vector<KGML_alt>	altVec;
};

struct KGML_reaction{
	KGML_reaction(){}
	KGML_reaction(const string i, const string n, const string t):
		id(i),name(n),type(t){ substrateVec.clear(); productVec.clear(); }
	void		add_substrate( const KGML_substrate st ){ substrateVec.push_back(st); }
	void		add_product( const KGML_product pd ){ productVec.push_back(pd); }
	void		print();
	string		id;
	string		name;
	string		type;

	vector<KGML_substrate> 	substrateVec;
	vector<KGML_product>	productVec;
};

//---------------------------------------------

struct KGML_subtype{
	KGML_subtype(){}
	KGML_subtype(const string n, const string v):name(n),value(v){}
	void		print();
	string		name;
	string		value;
};

struct KGML_relation{
	KGML_relation(){}
	KGML_relation(const string e1, const string e2, const string t):
		entry1(e1),entry2(e2),type(t){ subtypeVec.clear(); }
	void		add_subtype( const KGML_subtype st ){ subtypeVec.push_back(st); }
	void		print();
	string		entry1;
	string		entry2;
	string		type;

	vector<KGML_subtype>	subtypeVec;
};

//-----------------------------------------------

struct KGML_pathway{
	KGML_pathway(){ empty = true; }
	KGML_pathway( const string nm, const string o, const string nu,
			const string t, const string i ):
		name(nm), org(o), number(nu), title(t), image(i){ entryVec.clear();
		reactionVec.clear(); relationVec.clear(); empty = false; }
	void		add_entry( const KGML_entry et ){ entryVec.push_back(et); }
	void		add_reaction( const KGML_reaction rt ){ reactionVec.push_back(rt); }
	void		add_relation( const KGML_relation rl ){ relationVec.push_back(rl); }
	void		print();
	void		print_compoundVec();
	void		print_mapVec();
	bool		isEmpty()const{ return empty; }
	void		map_KOgeneID_to_Name(const char* fileName);

	string		name;
	string		org;
	string		number;
	string		title;
	string 		image;
	bool		empty;

	vector<KGML_entry>		entryVec;
	vector<KGML_reaction>	reactionVec;
	vector<KGML_relation>	relationVec;

};

vector<KGML_entry> 				get_KGML_entry(const QDomElement firstEntry);
KGML_pathway 					readKeggXMLfile( const string& fileName, bool getEntryName = false);
string							get_keggEntryName( const string& url );
vector<pair<string, string> >	get_kegg_K_name(const char* fileName);
vector<pair<string, string> >	get_kegg_C_name(const char* fileName);
vector<pair<string, string> >	get_kegg_hsa_name(const char* fileName);
vector<pair<string, string> >	get_kegg_map_name(const char* fileName);
string							lookUpItem( vector<pair<string, string> > vec, string item );
#endif /* KGML_H_ */


