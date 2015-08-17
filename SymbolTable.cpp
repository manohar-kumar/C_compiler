#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
using namespace std;

struct lstEntry{
	string Type;
	string name;
	string value;

	lstEntry(string n,string T,string v){
		name=n;
		Type=T;
		value=v;
	}
	void print(){
		cout << Type<<" "<<name<<" "<<value<<endl;	
	}
	
}
struct lst {
	vector<lstEntry> localTable;

	void add(string name,string Type,string value){
		lstEntry temp(name,Type,value);
		localTable.push_back(temp);
		return;
	}
	
	bool check(string name){
		for (vector<int>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->name == name) return true;
		}
		return false;
	}
		
	lstEntry getEntry(string name){
		for (vector<int>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->name == name) return (*it);
		}
	}
	
	void print(){
		cout<< ""<<endl;
		for(int i=0;i<localTable.size();i++){
			localTable[i].print();
		}
		cout<<endl;
	}
	
}

struct gst {
	unordered_map<string, lst> entries;	//hash table of funcName, pointer to lst
	
	void add(string funcName,lst table){
		Entry[funcName]=table;
	}
	
	lst getTable(string funcName){
		return Entry[funcName];
	}
	
	void print(){
		for ( auto it = mymap.begin(); it != mymap.end(); ++it ){
			cout << "Local Symbol Table of " << it->first << ":" << it->second;
		}
	}
	
}

gst globalSymbolTable;
