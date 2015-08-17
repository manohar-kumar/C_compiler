
integer 		[0-9]+
float 			([0-9]+)\.[0-9]+
comment 		\/\*[^\*]*(\*([^\/\*][^\*]*)?)*\*\/
identifier 		[[:alpha:]_][[:alpha:][:digit:]_]*
integer-part		[0-9]+
fractional-part		[0-9]+
exponent-part		[eE][+-][0-9]*|[eE][0-9]*
integer-constant	0|([1-9][0-9]*)|(0[bB][01]+|0[0-7]+)|(0[xX][0-9a-fA-F]+)
whitespaces 		[ \t\n]+
%%

{whitespaces}                   {}
{comment}  				{}
return					{return Parser::RETURN;}
void					{ return Parser::VOID;}
int					{return Parser::INT;}
float					{return Parser::FLOAT;}
"("                                    	{ return '(';}
")"                                     { return ')';}
","                                    	{ return ',';}
"["					{ return '[';}
"]"					{ return ']';}
"{"					{ return '{';}
"}"					{ return '}';}

";"					{ return ';'; }

"="					{ return '=';}
"||"					{ return Parser::OR_OP; }
"&&"					{ return Parser::AND_OP;}
"=="					{ return Parser::EQ_OP; }
"!="					{ return Parser::NE_OP; }
"<"					{ return '<'; }
">"					{ return '>'; }
"<="					{ return Parser::LE_OP; }
">="					{ return Parser::GE_OP; }

"+"					{ return '+'; }
"-"					{ return '-'; }
"*"					{ return '*'; }
"/"					{ return '/'; }

"++"					{ return Parser::INC_OP; }


(([0-9]*)\.([0-9])+)|(([0-9]+)\.([0-9])*)([eE][+-][0-9]+|[eE][0-9]+)?	{ Parser::d_val__= (float) std::atof(matched().c_str());return Parser::FLOAT_CONSTANT;}
([0-9][0-9]*)|(0[bB][01]+|0[0-7]+)|(0[xX][0-9a-fA-F]+) { Parser::d_val__=std::atoi(matched().c_str());return Parser::INT_CONSTANT;}
\"(\\.|\\\\.|[^"\n])*\"						{ Parser::d_val__=std::string(matched());return Parser::STRING_LITERAL;}

"!"					{ return '!'; }
"~"					{ return '~'; }

"if"					{ return Parser::IF; }
"else"					{ return Parser::ELSE; }
"while"					{ return Parser::WHILE; }
"for"					{ return Parser::FOR; }

{identifier} 				{ Parser::d_val__=std::string(matched());return Parser::IDENTIFIER;}
	
