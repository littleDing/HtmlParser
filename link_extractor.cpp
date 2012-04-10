#include <iostream>
#include <cstdio>
#include "HtmlParser.hpp"
#include <map>
#include <string>
#define LOG(var) cerr<<#var<<"="<<var<<endl;
using namespace std;

void print_help(){
	cout<<"Usage: link_extractor [OPTIONS]"<<endl
		<<"OPTIONS:"<<endl
		<<"\t"<<"-b=baseurl Set base url"<<endl;
}
void exitWithHelp(const int& st=1){
	print_help();
	exit(st);
}

class Begin{
	string baseurl;
   	vector<string> links;
   	bool hasHead(const string& url){
   		return url.find("http://")==0||url.find("https://")==0;
   	}
   	bool getUrl(const string& urlString,string& url){
		bool doGet=false;
		if(urlString[0]=='\"'){
			if(urlString[1]!='#'){
				url=urlString.substr(1,urlString.length()-2);
				doGet=true;
			}
		}else if(urlString[1]!='#'){
			url=urlString;
			doGet=true;
		}
		return doGet;
	}
	void printUrl(const string& url){
		if(hasHead(url)){
   			cout<<url<<endl;
   		}else{
   			int slash=0;
   			if(baseurl.length()>0&&baseurl[baseurl.length()-1]=='/') slash++;
   			if(url[0]=='/') slash++;   				
   			if(slash==0){
   				cout<<baseurl<<'/'<<url<<endl;
   			}else{
   				cout<<baseurl<<url<<endl;
   			}
   		}
	}
   	bool pushUrl(const string& link){
   		string url;
   		if(getUrl(link,url)){
   			//links.push_back(url);
   			printUrl(url);
   			return true;
   		}
   		return false;
   	}
public:
	Begin(const string& _baseurl=""):baseurl(_baseurl){
	}
    void operator()(HtmlNode* node){
		map<string,string>::iterator it =node->properties.find("href");
		if(it!=node->properties.end()){
			if(node->tag=="base"){
				getUrl(it->second,baseurl);
			}else{
				pushUrl(it->second);
			}
		}
    }
    void print(){
    	//LOG(baseurl);
		//for(int i=0;i<links.size();++i){
		//	
		//}
    }
};

int main(int argc,char *argv[]){
	if(argc!=3){
		exitWithHelp();
	}
	char args[]="b:";
	char ch=-1;
	string baseurl;
	while((ch=getopt(argc,argv,args))!=-1){
		switch(ch){
			case 'b' :
				baseurl=optarg;
			break;
			default:
				exitWithHelp();
			break;
		}
	}
	string tmp,html;
	while(cin>>tmp){
		html+=tmp;
		html+=" ";
	}
	HtmlParser parser;
	HtmlNode *node = parser.parse(html);
	Begin begin(baseurl);
	EndVisitorExample end;
	node->visit(begin,end);
	begin.print();
    return 0;
}
