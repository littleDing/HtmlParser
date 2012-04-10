//
//  HtmlParser.h
//  TestCPP
//
//  Created by  on 12-3-26.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef __HtmlParser_h__
#define __HtmlParser_h__
#include <string>
#include <map>
#include <stack>
#include <list>
#include <queue>
#include <iostream>
#include <set>
#include "sstream"
using std::string;
using std::stack;
using std::list;
using std::map;
using std::queue;
using std::ostream;
using std::stringstream;

enum HtmlNodeType {
    HtmlNodeTypeNormal = 1
    ,HtmlNodeTypeComment
    ,HtmlNodeTypeSingle
    ,HtmlNodeTypeScript
};

class BeginVisitorExample;
class EndVisitorExample;

struct HtmlNode{
    string tag;
    map<string,string> properties;
    list<HtmlNode*> sons; 
    std::string text;
    HtmlNodeType type;
    int depth;
    void printPrefixTabs(std::ostream& out);
    HtmlNode():depth(0),type(HtmlNodeTypeNormal){}
    
    void addSon(HtmlNode* son);
    template <class BeginVisitor=BeginVisitorExample,class EndVisitor=EndVisitorExample>
    void visit(BeginVisitor& begin,EndVisitor& end);
};

template <class BeginVisitor,class EndVisitor>
void HtmlNode::visit(BeginVisitor& begin,EndVisitor& end){
    begin(this);
    for(std::list<HtmlNode*>::iterator it=this->sons.begin();it!=this->sons.end();it++){
        (*it)->visit(begin,end);
    }
    end(this);
}

class BeginVisitorExample{
public:
    bool operator()(HtmlNode* node){
        return true;
    }
};
class EndVisitorExample{
public:
    bool operator()(HtmlNode* node){
        return true;
    }
};
ostream& operator << (std::ostream& fout,HtmlNode& node);

class HtmlParser {
protected:
    std::stack<HtmlNode*> state;
    std::queue<std::string> tags;
    std::set<std::string> singleTags;
    std::vector< std::pair<std::string, std::string> > escapes;
public:
    HtmlParser();
    HtmlParser(const std::vector<std::string>& _singleTags) : singleTags(_singleTags.begin(),_singleTags.end())  {
    }
    HtmlParser(const std::vector< std::pair<std::string, std::string> >& _escapes) : escapes(_escapes){
    }
    HtmlParser(const std::vector<std::string>& _singleTags,const std::vector< std::pair<std::string, std::string> >& _escapes) : singleTags(_singleTags.begin(),_singleTags.end()),escapes(_escapes){
    }
    void split(const std::string& s,std::queue<std::string>& _tags);
    HtmlNode* getNode(const std::string& s);
public:
    HtmlNode* parse(const string& htmlString);
};

#endif

