#include <iostream>
#include <cstdio>
#include "HtmlParser.hpp"
#include <map>
#include <string>
using namespace std;

class Begin{
    map<string,int> cnt;
    bool isEmpty(char ch){
		return ch==' '||ch=='\n'||ch=='\t';
	}
   	void removeBlanks(string& s){
		int f=0,e=s.length()-1;
		while(f<=e&&isEmpty(s[f])) f++;
		while(f<=e&&isEmpty(s[e])) e--;
		s=s.substr(f,e-f+1);   		
   	}
public:
    void operator()(HtmlNode* node){
    	removeBlanks(node->text);
        if(node->type==HtmlNodeTypeNormal&&node->text.length()){
            cnt[node->text]++;
        }
    }
    void print(){
		for(map<string,int>::iterator it=cnt.begin();it!=cnt.end();it++){
			cout<<it->first<<" "<<it->second<<endl;
		}
    }
};

int main(){
	string tmp,html;
	while(cin>>tmp){
		html+=tmp;
		html+=" ";
	}
	HtmlParser parser;
///*
	queue<string> qqq;
	parser.split(html,qqq);
	while(!qqq.empty()){
		cout<<qqq.front()<<endl;
		qqq.pop();
	}
	return 0;
//*/	
	HtmlNode *node = parser.parse(html);
	Begin begin;
	EndVisitorExample end;
	node->visit(begin,end);
	begin.print();
    return 0;
}
