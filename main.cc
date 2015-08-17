#include <iostream>
#include "Scanner.h"
#include "Parser.h"
using namespace std;
int main (int argc, char** arg)
{	
	Parser parser;
	if(argc>1){
		parser.chosenFunction=arg[1];
	}
	parser.parse();
	print_code();
}


