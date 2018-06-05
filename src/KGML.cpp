/*
 * KGML.cpp
 *
 *  Created on: Jul 12, 2016
 *      Author: stan
 */

#include"KGML.h"
#include"tokenSeparator.h"

void
KGML_component::print(){
	cout<<"\t\t component: id="<<id<<endl;
}

void
KGML_graphics::print(){
	cout<<"\t\t graphics: name="<<name<<" x="<<x<<" y="<<y<<" coords="<<coords<<" type="<<type
			<<" width="<<width<<" height="<<height<<" fgcolor="<<fgcolor<<endl;
}

void
KGML_entry::print(){
	cout<<"\t entry: id="<<id<<" name="<<name<<" type="<<type<<" link="<<link<<" reaction="<<reaction<<endl;
	for( size_t i=0; i<componentVec.size(); i++ ){
		componentVec[i].print();
	}
	for( size_t i=0; i<graphicsVec.size(); i++ ){
		graphicsVec[i].print();
	}
}

void
KGML_alt::print(){
	cout<<"\t\t\t alt: name="<<name<<endl;
}

void
KGML_substrate::print(){
	cout<<"\t substrate: id="<<id<<" name="<<name<<endl;
	for( size_t i=0; i<altVec.size(); i++ ){
		altVec[i].print();
	}
}

void
KGML_product::print(){
	cout<<"\t\t product: id="<<id<<" name="<<name<<endl;
	for( size_t i=0; i<altVec.size(); i++ ){
		altVec[i].print();
	}
}

void
KGML_reaction::print(){
	cout<<"\t reaction: id="<<id<<" name="<<name<<" type="<<type<<endl;
	for( size_t i=0; i<substrateVec.size(); i++ ){
		substrateVec[i].print();
	}
	for( size_t i=0; i<productVec.size(); i++ ){
		productVec[i].print();
	}
}

void
KGML_subtype::print(){
	cout<<"\t\t subtype: name="<<name<<" value="<<value<<endl;
}

void
KGML_relation::print(){
	cout<<"\t relation: entry1="<<entry1<<" entry2="<<entry2<<" type="<<type<<endl;
	for( size_t i=0; i<subtypeVec.size(); i++ ){
		subtypeVec[i].print();
	}
}

void
KGML_pathway::print(){
	cout<<"pathway: name="<<name<<" org="<<org<<" number="<<number<<" title="<<title<<" image="<<image<<endl;
	for( size_t i=0; i<entryVec.size(); i++ ){
		entryVec[i].print();
	}
	for( size_t i=0; i<reactionVec.size(); i++ ){
		reactionVec[i].print();
	}
	for( size_t i=0; i<relationVec.size(); i++ ){
		relationVec[i].print();
	}
}

void
KGML_pathway::print_compoundVec(){
	size_t count=0;
	for( size_t i=0; i<entryVec.size(); i++ ){
		KGML_entry en=entryVec[i];
		if( en.type == "compound" ){
			cout<<++count<<endl;
			en.print();
		}
	}
}

void
KGML_pathway::print_mapVec(){
	size_t count=0;
	for( size_t i=0; i<entryVec.size(); i++ ){
		KGML_entry en=entryVec[i];
		if( en.type == "map" ){
			cout<<++count<<endl;
			en.print();
		}
	}
}

void
KGML_pathway::map_KOgeneID_to_Name(const char* fileName){
	ifstream iif;
	iif.open(fileName);
	if( !iif ){
		cerr<<"error! can not open kegg hsa name file!!!"<<endl;
	}
	string line;
	vector<string> KO_ID_vec;
	vector<string> nameVec;
	while(getline(iif,line)){
		vector<string> strVec=tokenByTab(line);
		if( strVec.size()>2 ){
			KO_ID_vec.push_back(strVec[0]);
			nameVec.push_back(strVec[1]);
		}
	}
	for(size_t i=0; i<entryVec.size(); i++){
		vector<string> entryNameVec = tokenBySpace(entryVec[i].name);
		entryVec[i].graphicsVec[0].name="";
		int flag = 0;
		for( size_t j=0; j<entryNameVec.size(); j++ ){
			vector<string> entryNameVecSub = tokenByColon(entryNameVec[j]);
			if( entryNameVecSub.size()>1 ){
				for( size_t k=0; k<KO_ID_vec.size(); k++ ){
					string KO_ID = KO_ID_vec[k];
					if(entryNameVecSub[1]==KO_ID){
						entryVec[i].graphicsVec[0].name=entryVec[i].graphicsVec[0].name+nameVec[k]+", ";
						flag = 1;
					}
				}
			}
		}
		if(!flag){
			cout<<"!!! NO "<<entryVec[i].name<<endl;
		}
	}
}

//---------------------------------------------------------------------

string
get_keggEntryName( const string& url ){

	HttpWindow	htp;
	string		name = htp.get_keggEntryName( QString::fromStdString(url) );

	return		name;
}

KGML_pathway
readKeggXMLfile( const string& fileName , bool getEntryName){
	QDomDocument		doc;
	QFile				file( QString::fromStdString(fileName) );
	KGML_pathway	pathway;
	if( !file.open(QIODevice::ReadOnly) ||  !doc.setContent(&file)  ){
		cerr<<"can not open: "<<fileName<<endl;
		file.close();
		return pathway;
	}
	file.close();


	// print out the element names of all elements that are direct children
	// of the outermost element.
	QDomElement	docElem = doc.documentElement();

	if( docElem.tagName() == "pathway" ){
		string	name = docElem.attribute(QString("name")).toStdString();
		string	org = docElem.attribute(QString("org")).toStdString();
		string	number = docElem.attribute(QString("number")).toStdString();
		string	title = docElem.attribute(QString("title")).toStdString();
		string	image = docElem.attribute(QString("image")).toStdString();
		KGML_pathway p(name, org, number, title, image);
		pathway = p;
	}

	QDomNode	n = docElem.firstChild();

//	vector<pair<string, string> > K_pairVec = get_kegg_K_name("data/keggPathway/kegg_K_name");
//	vector<pair<string, string> > C_pairVec = get_kegg_C_name("data/keggPathway/kegg_C_name");
//	vector<pair<string, string> > Hsa_pairVec= get_kegg_hsa_name("data/keggPathway/kegg_hsa_name");;

	while( !n.isNull() ){
		QDomElement e=n.toElement();
		if( !e.isNull() ){
			if( e.tagName() == "entry" ){
				string	id = e.attribute(QString("id")).toStdString();
				string	name = e.attribute(QString("name")).toStdString();
				string	type = e.attribute(QString("type")).toStdString();
				string	link = e.attribute(QString("link")).toStdString();
				string	reaction = e.attribute(QString("reaction")).toStdString();

				if(getEntryName){
					string s = get_keggEntryName( link );
					if( !s.empty() ){
						name = s;		//replace entry name with url idicated NAME;
					}
				}

				KGML_entry	entry(id, name, type, link, reaction);

				QDomNodeList	component_list = e.elementsByTagName(QString("reaction"));
				QDomNodeList	graphics_list = e.elementsByTagName(QString("graphics"));
				for( int i=0; i<component_list.count(); i++ ){
					QDomNode 	elm = component_list.at(i);
					string		comp_id = elm.toElement().attribute(QString("id")).toStdString();
					KGML_component	comp(comp_id);
					entry.add_component(comp);
				}
				for( int i=0; i<graphics_list.count(); i++ ){
					QDomNode 	elm 			= graphics_list.at(i);
					string		graph_name 		= elm.toElement().attribute(QString("name")).toStdString();
					string		graph_x 		= elm.toElement().attribute(QString("x")).toStdString();
					string		graph_y 		= elm.toElement().attribute(QString("y")).toStdString();
					string		graph_coords 	= elm.toElement().attribute(QString("coords")).toStdString();
					string		graph_type 		= elm.toElement().attribute(QString("type")).toStdString();
					string		graph_width 	= elm.toElement().attribute(QString("width")).toStdString();
					string		graph_height 	= elm.toElement().attribute(QString("height")).toStdString();
					string		graph_fgcolor 	= elm.toElement().attribute(QString("fgcolor")).toStdString();

//					if( type == string("compound") ){
//						graph_name = lookUpItem( C_pairVec, graph_name );
//					}

					KGML_graphics	graphics(graph_name, graph_x, graph_y, graph_coords,
							graph_type, graph_width, graph_height, graph_fgcolor );
					entry.add_graphics(graphics);
				}
				pathway.add_entry(entry);

			}else if( e.tagName() == "reaction" ){
				string	id = e.toElement().attribute(QString("id")).toStdString();
				string	name = e.toElement().attribute(QString("name")).toStdString();
				string	type = e.toElement().attribute(QString("type")).toStdString();
				KGML_reaction	reaction(id, name, type);

				QDomNodeList	substrate_list = e.elementsByTagName(QString("substrate"));
				QDomNodeList	product_list = e.elementsByTagName(QString("product"));
				for( int i=0; i<substrate_list.count(); i++ ){
					QDomNode	elm = substrate_list.at(i);
					string		substrate_id = elm.toElement().attribute(QString("id")).toStdString();
					string		substrate_name = elm.toElement().attribute(QString("name")).toStdString();
					KGML_substrate	substrate(substrate_id, substrate_name);
					QDomNodeList	alt_list = elm.toElement().elementsByTagName(QString("alt"));
					for( int j=0; j<alt_list.count(); j++ ){
						QDomNode		elm_alt=alt_list.at(j);
						string			alt_name = elm_alt.toElement().attribute(QString("name")).toStdString();
						KGML_alt		alt(alt_name);
						substrate.add_alt(alt);
					}
					reaction.add_substrate(substrate);
				}
				for( int i=0; i<product_list.count(); i++ ){
					QDomNode	elm = product_list.at(i);
					string		product_id = elm.toElement().attribute(QString("id")).toStdString();
					string		product_name = elm.toElement().attribute(QString("name")).toStdString();
					KGML_product	product(product_id, product_name);
					QDomNodeList	alt_list = elm.toElement().elementsByTagName(QString("alt"));
					for( int j=0; j<alt_list.count(); j++ ){
						QDomNode		elm_alt=alt_list.at(j);
						string			alt_name = elm_alt.toElement().attribute(QString("name")).toStdString();
						KGML_alt		alt(alt_name);
						product.add_alt(alt);
					}
					reaction.add_product(product);
				}
				pathway.add_reaction(reaction);

			}else if( e.tagName() == "relation" ){
				string	entry1 = e.toElement().attribute(QString("entry1")).toStdString();
				string	entry2 = e.toElement().attribute(QString("entry2")).toStdString();
				string	type = e.toElement().attribute(QString("type")).toStdString();
				KGML_relation	relation(entry1, entry2, type);

				QDomNodeList	subtype_list = e.toElement().elementsByTagName(QString("subtype"));

				for( int i=0; i<subtype_list.count(); i++ ){
					QDomNode	elm = subtype_list.at(i);
					string		name = elm.toElement().attribute(QString("name")).toStdString();
					string		value = elm.toElement().attribute(QString("value")).toStdString();
					KGML_subtype	subtype(name, value);
					relation.add_subtype(subtype);
				}
				pathway.add_relation(relation);
			}
		}
		n=n.nextSibling();
	}
	return pathway;
}

vector<pair<string, string> >
get_kegg_K_name(const char* fileName){
	vector<pair<string, string> > nameVec;
	ifstream	iff;
	iff.open(fileName);
	if( !iff ){
		cerr<<"error! can not open kegg kinase name file!!!"<<endl;
	}
	string line;
	while( getline(iff, line) ){
		vector<string> strVec=tokenByTab(line);
		if( strVec.size() > 2 ){
			string	refName = strVec[0];
			string	name = strVec[1];
			pair<string, string> p = make_pair(refName, name);
			nameVec.push_back( p );
		}
	}

	return nameVec;
}

vector<pair<string, string> >
get_kegg_C_name(const char* fileName){
	vector<pair<string, string> > nameVec;
	ifstream	iff;
	iff.open(fileName);
	if( !iff ){
		cerr<<"error! can not open kegg compound name file!!!"<<endl;
	}
	string line;
	while( getline(iff, line) ){
		vector<string> strVec=tokenByTab(line);
		if( strVec.size() > 2 ){
			string	refName = strVec[0];
			string	name = strVec[1];
			pair<string, string> p = make_pair(refName, name);

			nameVec.push_back( p );
		}
	}

	return nameVec;
}

vector<pair<string, string> >
get_kegg_hsa_name(const char* fileName){
	vector<pair<string, string> > nameVec;
	ifstream	iff;
	iff.open(fileName);
	if( !iff ){
		cerr<<"error! can not open kegg hsa name file!!!"<<endl;
	}
	string line;
	while( getline(iff, line) ){
		vector<string> strVec=tokenByTab(line);
		if( strVec.size() > 2 ){
			string	refName = strVec[0];
			string	name = strVec[1];
			pair<string, string> p = make_pair(refName, name);
			nameVec.push_back( p );
		}
	}

	return nameVec;
}

vector<pair<string, string> >
get_kegg_map_name(const char* fileName){
	vector<pair<string, string> > nameVec;
	ifstream	iff;
	iff.open(fileName);
	if( !iff ){
		cerr<<"error! can not open kegg hsa name file!!!"<<endl;
	}
	string line;
	while( getline(iff, line) ){
		vector<string> strVec=tokenByTab(line);
		if( strVec.size() > 2 ){
			string	refName = strVec[0];
			string	name = strVec[1];
			pair<string, string> p = make_pair(refName, name);
			nameVec.push_back( p );
		}
	}

	return nameVec;
}


string
lookUpItem( vector<pair<string, string> > vec, string item ){
	for( size_t i=0; i<vec.size(); i++ ){

		if( item == vec[i].first ){

			return vec[i].second;
		}
	}
	return item;
}

