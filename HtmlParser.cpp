//
//  HtmlParser.cpp
//  TestCPP
//
//  Created by  on 12-3-26.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "HtmlParser.hpp"
#include "boost/pool/object_pool.hpp"
#include "sstream"
using boost::object_pool;
using std::stringstream;
using std::pair;
using std::endl;
boost::object_pool<HtmlNode> HtmlNodePool;

void HtmlNode::printPrefixTabs(std::ostream &out){
    for (int i=0; i<this->depth; i++) {
        out<<"\t";
    }
}

void HtmlNode::addSon(HtmlNode *son){
	//std::cerr<<__FUNCTION__<<" "<<*this<<" "<<*son<<endl;
    this->sons.push_back(son);
    son->depth=this->depth+1;
    if(this->type==HtmlNodeTypeComment) son->type=this->type;
    if(this->type==HtmlNodeTypeScript) son->type=this->type;
}

std::ostream& operator << (std::ostream& fout,HtmlNode& node){
    node.printPrefixTabs(fout);
    if(node.tag.length()){
        fout<<"<"<<node.tag;
    }
    for(std::map<std::string, std::string>::iterator it=node.properties.begin();it!=node.properties.end();it++){
        fout<<" "<<it->first<<"="<<it->second;
    }
    if(node.tag.length()) fout<<">";
    if(node.sons.size()){
        fout<<endl;
        for(std::list<HtmlNode*>::iterator it=node.sons.begin();it!=node.sons.end();it++){
            fout<<*(*it)<<endl;
        }
        node.printPrefixTabs(fout);
    }else {
        fout<<node.text;
    }
    if(node.tag.length()&&node.type==HtmlNodeTypeNormal){
        fout<<"</"<<node.tag<<">";
    }
    return fout;
}

HtmlParser::HtmlParser(){
    std::vector<std::string> stdSingles;
    stdSingles.push_back("meta");
    stdSingles.push_back("br");
    stdSingles.push_back("area");
    stdSingles.push_back("base");
    stdSingles.push_back("basefont");
    stdSingles.push_back("col");
    stdSingles.push_back("frame");
    stdSingles.push_back("!DOCTYPE");
    stdSingles.push_back("hr");
    stdSingles.push_back("input");
    stdSingles.push_back("link");
    stdSingles.push_back("param");
    singleTags=std::set<std::string>(stdSingles.begin(),stdSingles.end());
    
    escapes.push_back( std::pair<std::string, std::string>("\"","\"") );
    escapes.push_back( std::pair<std::string, std::string>("<script",">") );
    escapes.push_back( std::pair<std::string, std::string>("<",">") );
}

HtmlNode* HtmlParser::parse(const string &htmlString){
    while (!tags.empty()) {
        tags.pop();
    }
    this->split(htmlString,tags);
    while(!state.empty()){
        state.pop();
    }
    HtmlNode* root=HtmlNodePool.construct();
    root->depth=0;
    state.push(root);
    while (!tags.empty()) {
        std::string tmp=tags.front(); tags.pop();
        HtmlNode *current=NULL;
        if(!state.empty()){
            current=state.top();
        }
        HtmlNode *node=NULL;
        if(tmp[0]=='<'){
            if(tmp.substr(0,4)=="<!--"){
                //comment
                node=HtmlNodePool.construct();
                node->type=HtmlNodeTypeComment; 
                if(current) {
                    current->addSon(node);
                }
            }else if(tmp.substr(0,2)=="</"){
                //end node
                state.pop(); 
            }else if(tmp.substr(tmp.length()-2,2)=="/>"){
                //self end node
                node=getNode(tmp);
                if(current) {
                    current->sons.push_back(node);
                    node->depth =current->depth+1;
                }
                //state.push(node);
            }else{
                //begin node
                node=getNode(tmp);
                if(current) {
                    current->sons.push_back(node);
                    node->depth =current->depth+1;
                }
                if(node->type!=HtmlNodeTypeSingle)
                    state.push(node);
            }
        }else{
            current=state.top();
            node=HtmlNodePool.construct();
            node->text=tmp;
            current->addSon(node);
        }
    }
    return root;
}

inline bool isBlank(const char& ch){
    return ch==' '||ch=='\t'||ch=='\n';
}

void removeBlanks(string& s){
	int f=0,e=s.length()-1;
	while(f<=e&&isBlank(s[f])) f++;
	while(f<=e&&isBlank(s[e])) e--;
	s=s.substr(f,e-f+1);   		
}

inline void getPrefix(string& source,string& target,const int& len=0){
	target+=source.substr(0,len); source=source.substr(len);
}

void HtmlParser::split(const std::string& s,std::queue<std::string>& _tags){
    enum SplitStateEnum {
        splitStateLeft = 0
        ,splitStateRight
        ,splitStateInString
        ,splitStateInScript
        ,splitStateTag
        ,splitStateNotDefine
    };
    string tmp;
    std::stringstream sin(s);
    string atag;
    SplitStateEnum st=splitStateLeft,lastSt=splitStateLeft;  //0:<   1:>  2:"   3:script 4:tag
    string lastTag;
    while(sin>>tmp){
        tmp+=" ";
        while(tmp.length()){
        	size_t spos=tmp.find("\"");
            switch (st) {
                case splitStateLeft:{    //looking for <
                    size_t pos=tmp.find("<");
                    if(pos==tmp.npos){
                    	if(spos==tmp.npos){
                        	atag+=tmp; tmp="";
                        }else{
                        	getPrefix(tmp,atag,spos+1); //atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;  
                        }
                    }else{
                        if(spos<pos){
                            getPrefix(tmp,atag,spos+1);//atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;   
                        }else{
                            getPrefix(tmp,atag,pos);//atag+=tmp.substr(0,pos); tmp=tmp.substr(pos);
                            removeBlanks(atag);
                            if(atag.length()){
                                _tags.push(atag); atag="";
                            }
                            st=splitStateTag;
                        }
                    }
                    break;
                }case splitStateRight:{   //looking for >
                    size_t pos=tmp.find(">");
                    if(pos==tmp.npos){
                        if(spos==tmp.npos){
                        	atag+=tmp; tmp="";
                        }else{
                        	getPrefix(tmp,atag,spos+1);//atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;  
                        }
                    }else{
                        size_t spos=tmp.find("\"");
                        if(spos<pos){
                            getPrefix(tmp,atag,spos+1);//atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;
                        }else{
                            getPrefix(tmp,atag,pos+1);//atag+=tmp.substr(0,pos+1); tmp=tmp.substr(pos+1);
                            _tags.push(atag); atag="";
                            if(lastTag=="<script"){
                                st=splitStateInScript;
                            }
                            else
                                st=splitStateLeft;
                        }
                    }
                    break;
                }case splitStateInString:{   //inside string
                    size_t pos=tmp.find("\"");
                    if(pos==tmp.npos){
                        if(spos==tmp.npos){
                        	atag+=tmp; tmp="";
                        }else{
                        	atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;  
                        }
                    }else{
                        getPrefix(tmp,atag,pos+1);//atag+=tmp.substr(0,pos+1); tmp=tmp.substr(pos+1);
                        st=lastSt;  lastSt=splitStateLeft;
						if(tmp[0]=='\"'){	tmp=tmp.substr(1);	}
                    }
                    break;
                }case splitStateInScript:{   //inside script
                    size_t pos=tmp.find("</script>");
                    if(pos==tmp.npos){
                        if(spos==tmp.npos){
                        	atag+=tmp; tmp="";
                        }else{
                        	getPrefix(tmp,atag,spos+1);//atag+=tmp.substr(0,spos+1); tmp=tmp.substr(spos+1);
                            lastSt=st;   st=splitStateInString;  
                        }
                    }else{
                        getPrefix(tmp,atag,pos+1);//atag+=tmp.substr(0,pos); tmp=tmp.substr(pos);
                        _tags.push(atag); atag="";
                        st=splitStateLeft;
                    }
                    break;
                }case splitStateTag:{   //just after <
                    size_t pos=tmp.find(">");
                    if(pos==tmp.npos){
                        lastTag=tmp;    atag+=tmp; tmp="";
                        st=splitStateRight;
                        removeBlanks(lastTag);
                    }else{
                        lastTag=tmp.substr(0,pos);    atag+=lastTag; tmp=tmp.substr(pos);
                        st=splitStateRight;
                    }
                    break;
                }default:
                    break;
            }
        }
    }
}

HtmlNode* HtmlParser::getNode(const std::string& s){
    HtmlNode* node=HtmlNodePool.construct();
    node->type =HtmlNodeTypeNormal;
    stringstream sin;
    if(s[s.length()-2]=='/'){
        sin<<s.substr(1,s.length()-3);
    }else{
        sin<<s.substr(1,s.length()-2);
    }
    string tmp;
    //get the tag
    sin>>node->tag;
    removeBlanks(node->tag);
    if(singleTags.count(node->tag)>0){
        node->type =HtmlNodeTypeSingle;
    }
    if(node->tag=="script"){
    	node->type=HtmlNodeTypeScript;
    }
    
    string key,value;  
    std::stack<int> st; //0:key 1:= 2:value  3:"
    st.push(0);
    while (sin>>tmp) {
        while(tmp.length()){
            switch (st.top()) {
                case 0: {
                    size_t pos=tmp.find("=");
                    if(pos==tmp.npos){
                        key=tmp; tmp="";
                        st.pop();   st.push(1);
                    }else{
                        key=tmp.substr(0,pos);
                        st.pop();   st.push(2);
                        tmp=tmp.substr(pos+1);
                    }
                    break;
                }case 1: {
                    size_t pos=tmp.find("=");
                    tmp=tmp.substr(pos+1);
                    st.pop(); st.push(2); 
                    break;
                }case 2: {
                    size_t pos=tmp.find("\"");
                    if(pos==tmp.npos){
                        value+=tmp; tmp="";
                        st.pop(); st.push(0);
                        node->properties[key]=value;
                        key=value="";
                    }else{
                        value+=tmp.substr(0,pos+1); tmp=tmp.substr(pos+1);
                        st.push(3);
                    }
                    break;
                }case 3: {
                    size_t pos=tmp.find("\"");
                    if(pos==tmp.npos){
                        value+=tmp;  tmp="";
                    }else{
                        value+=tmp.substr(0,pos+1); tmp=tmp.substr(pos+1);
                        st.pop();
                        if(tmp.length()==0){
                            st.pop(); st.push(0);
                            node->properties[key]=value;
                            key=value=""; 
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
    return node;
}

