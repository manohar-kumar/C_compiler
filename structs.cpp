#include <fstream>
#include "structs.h"
vector<string> global_code;
int counter = 0;
void gen(string s){
//	cout<<s<<endl;
	global_code.push_back(s);
}
void print_code(){
	ofstream outfile;
   	outfile.open("mycode.asm",ios::out);
	for (int i=0;i<global_code.size();i++){
		outfile << global_code[i] <<endl;
		cout<<global_code[i]<<endl;

	}
	outfile.close();
}

void printf(){

}

int getCounter(){
	return counter++;
}