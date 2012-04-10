#include <iostream>
#include <cstdio>
#include "HtmlParser.hpp"
#include <map>
#include <string>
using namespace std;

void print_help(){
	cout<<"";
}

class Begin{
   	vector<string> links;
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
    	
    }
    void print(){
		
    }
};

int main(int argc,char *argv[]){
	char args[]="abc:d:";
	char ch=-1;
	while((ch=getopt(argc,argv,args))!=-1){
		cerr<<ch<<" "<<optarg<<endl;
	}
	
	string tmp,html;
	while(cin>>tmp){
		html+=tmp;
		html+=" ";
	}
	HtmlParser parser;
	HtmlNode *node = parser.parse(html);
	Begin begin;
	EndVisitorExample end;
	node->visit(begin,end);
	begin.print();
    return 0;
}
