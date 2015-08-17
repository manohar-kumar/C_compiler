%scanner Scanner.h
%scanner-token-function d_scanner.lex()


%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic Stmt: Stmtast*; Exp : Expast* ; listStatements: std::list<Stmtast*>* ; strings: std::string; Explist: std::list<Expast*>* ; fltn:float; integer:int; arr:ArrayRef* ; Decl:decla;
%type <Stmt> compound_statement statement assignment_statement selection_statement iteration_statement
%type <Exp> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression postfix_expression primary_expression  
%type <listStatements> statement_list 
%type <Explist> expression_list 
%type <strings> unary_operator IDENTIFIER STRING_LITERAL type_specifier
%type <fltn> FLOAT_CONSTANT
%type <integer> INT_CONSTANT constant_expression
%type <Decl> declarator
%type <arr> l_expression



%%
translation_unit
	: function_definition 
	{	offset=0;
		if(globalSymbolTable.check(chosenFunction) && !(done)){
			done=true;
			lst* temp=globalSymbolTable.getTable(chosenFunction);
			cout<<"Symbol Table of "<<chosenFunction<< " :"<<endl;
			temp->print();
		}

	}
	| translation_unit function_definition 
	{	offset=0;
		if(globalSymbolTable.check(chosenFunction) && !(done)){
			done=true;
			lst* temp=globalSymbolTable.getTable(chosenFunction);
			cout<<"Symbol Table of "<<chosenFunction<< " :"<<endl;
			temp->print();
		}

	}
        ;

function_definition
	: type_specifier fun_declarator {
		gen("void " +currentFunction +"(){");
		gen("pushi(ebp);");
		gen("move(esp,ebp);");
	}   
	compound_statement 
	{
	($4)->generate_code(tempLocalSymbolTable);
	gen("loadi(ind(ebp),ebp);");
	gen("popi(1);");
	gen("return;");
	gen("}");
	//tempLocalSymbolTable->print();
	}
	
	;

type_specifier
	: VOID 	
	{$$="void";type=Type(Type::Base,Type::Void);width=0;}
        | INT  
        {$$="int";type=Type(Type::Base,Type::Int);width=4;} 
	| FLOAT 
	{$$="float";type=Type(Type::Base,Type::Float);width=4;}
        ;

fun_declarator
	: funcname '(' parameter_list ')' 
	{offset=0;
	}
	
    | funcname
	'(' ')'     
	{offset=0;
	}
	;
funcname
	:IDENTIFIER 
    {
		currentFunction=$1;
		tempLocalSymbolTable=new lst();
		tempLocalSymbolTable->returnType=type;
		globalSymbolTable.add($1,tempLocalSymbolTable);
		offset=4;
	}
	;
parameter_list
	: parameter_declaration 

	| parameter_list ',' parameter_declaration 

	;

parameter_declaration
	: type_specifier declarator 
	{	Type temp2=*(($2).type);
		int w=($2).width;
		//lst* temp=globalSymbolTable.getTable(currentFunction);
		tempLocalSymbolTable->addParam(varName,temp2,w,offset);
		offset+=w;
	}
        ;

declarator
	: IDENTIFIER 
	{	
		varName=$1;
		decla temp(&(type),width);
		$$=temp;
	}
	| declarator '[' constant_expression ']' 
	{	
		decla temp(new Type(Type::Array,($1).type,$3),(($1).width)*($3));
		$$=temp;
	}
        ;

constant_expression 
        : INT_CONSTANT {$$=$1;}
        | FLOAT_CONSTANT {cerr<<"Array Index Error at "<<d_scanner.lineNr()<<endl;ABORT();}
        ;

compound_statement
	: '{' '}' 
	{
	($$)= new block_statement(new list<Stmtast*>());
	($$)->print();
	}
	| '{' statement_list '}' 
	{
	($$)= new block_statement($2);
	($$)->generate_code(tempLocalSymbolTable);
	($$)->print();
	}
    | '{' declaration_list statement_list '}' 
    {
	($$)= new block_statement($3);
	//($$)->generate_code(tempLocalSymbolTable);
	($$)->print();

	}
	;

statement_list
	: statement	
	{
	list<Stmtast*> *temp=new list <Stmtast*>();
	temp->push_back($1);
	($$) =temp;
	
	}
    | statement_list statement	
    {
    //cerr<<"sta"<<endl;
	($$)=($1);
	($$)->push_back(($2));
	}
	;

statement
        : '{' statement_list '}'  //a solution to the local decl problem
        {
        ($$)= new block_statement($2);
        }
        | selection_statement 	
        {
        ($$)=($1);
        }
        | iteration_statement 	
        {
        ($$)=($1);
        }
		| assignment_statement	
		{
		($$)=($1);
		}
        | RETURN expression ';'	
        {
	        if (($2)->type.compatible(($2)->type,(globalSymbolTable.getTable(currentFunction))->returnType)){  //recent
				($$)=new Return($2,(globalSymbolTable.getTable(currentFunction))->returnType,(globalSymbolTable.getTable(currentFunction))->noParam());
	        }
	        else {
				cerr<<"Return type mismatch error at "<<d_scanner.lineNr()<<endl;
				ABORT();
	        }
        }
        |IDENTIFIER '(' expression_list ')' ';' 
        {	
			if(tempLocalSymbolTable->check($1)) {
				cerr<<"variable  "<< ($1) <<"is used as a functioncall in Line "<<d_scanner.lineNr()<<endl;
				ABORT();
			}
	        if(globalSymbolTable.check($1)){
		        lst* temp=globalSymbolTable.getTable($1);

					if(($3)->size() != (temp->noParam())) {
						cerr<<"Wrong no of arguments for function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
						ABORT();
					}
					int i=0;
				for (list<Expast*> :: iterator it=($3)->begin();it!=($3)->end();it++){
					lstEntry temp2=(globalSymbolTable.getTable($1))->getEntryByOffset(4*(i+1));
			        if(temp2.param){
				        if (! ((*it)->type.compatible((*it)->type,temp2.t))){  //recent
							cerr<<"Type mismatch error for param "<<i+1 <<" in Line "<<d_scanner.lineNr()<<endl;
							ABORT();
					    }
				    }
				    i++;
				   }
			($$)=new FuncallStmt($1,$3,globalSymbolTable.getTable($1));
				//($$)->ret_type=globalSymbolTable.getTable(($1))->returnType;
			}

			else if ($1=="printf"){
				
			}
			else {
			cerr<<"Non declared Function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
			ABORT();
			}
		}
        ;

assignment_statement
	: ';'
	{
	//empty temp;
	($$)= new empty();

	} 								
	|  
	l_expression '=' expression ';'
	{	
		if (($1)->type.compatible(($1)->type,($3)->type)){   //changed
           ($$)=new Ass($1,$3);

        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
     }	
	;

expression
        : logical_and_expression 
        {
        ($$)=($1);
        }
        | expression OR_OP logical_and_expression
        {	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("OR_OP",$1,$3);
        	($$)->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
        }
	;

logical_and_expression
        : equality_expression
        {
        ($$)=($1);
        }
        | logical_and_expression AND_OP equality_expression 
        {	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("AND_OP",$1,$3);
        	$$->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
        }
	;

equality_expression
	: relational_expression 
	{
    ($$)=($1);
    }
    | equality_expression EQ_OP relational_expression 
    {	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("EQ_OP",$1,$3);
        	($$)->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }	
	| equality_expression NE_OP relational_expression
	{	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("NE_OP",$1,$3);
        	($$)->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
        }
	;
relational_expression
	: additive_expression
	{
    ($$)=($1);
    }
    | relational_expression '<' additive_expression 
    {	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("LT",$1,$3);
        	$$->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }	
	| relational_expression '>' additive_expression 
	{	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("GT",$1,$3);
        	($$)->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }		
	| relational_expression LE_OP additive_expression 
	{	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("LE_OP",$1,$3);
        	$$->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }	
    | relational_expression GE_OP additive_expression 
    {	
        if (($1) ->type.compatible(($1)->type,($3)->type)){
        	($$)= new BinaryOp("GE_OP",$1,$3);
        	$$->type=Type(Type::Base,Type::Int);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }	
	;

additive_expression 
	: multiplicative_expression
	{
    ($$)=($1);
    }
	| additive_expression '+' multiplicative_expression
	{
		int temp;
		Type temptype=$1->type;
    	if (($1)->type.compatible(($1)->type,($3)->type)){   //changed
    		if (($1)->cost>($3)->cost)						//lab5
   				temp=$1->cost;
   			else if ($1->cost<$3->cost) temp=$3->cost;
   			else temp=$1->cost+1;
 
    		($$) = new BinaryOp("PLUS",$1,$3);

    		$$->cost=temp;
    		
    		if ( ($1)->type.equal(temptype,Type(Type::Base,Type::Float))|| ($3)->type.equal(($3)->type,Type(Type::Base,Type::Float))){
       			$$->type=Type(Type::Base,Type::Float);
       			}
       		else 
       			$$->type=Type(Type::Base,Type::Int);
        }
        else {
       		cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;
       		ABORT();
    	}	
    }	 	 
	| additive_expression '-' multiplicative_expression 
	{
		int temp;
		Type temptype=$1->type;
    	if (($1) ->type.compatible(($1)->type,($3)->type)){   //changed
    		if (($1)->cost>($3)->cost)						//lab5
   				temp=$1->cost;
   			else if ($1->cost<$3->cost) temp=$3->cost;
   			else temp=$1->cost+1;

    		($$)= new BinaryOp("MINUS",$1,$3);
    		$$->cost=temp;
    		if ( ($1)->type.equal(temptype,Type(Type::Base,Type::Float))|| ($3)->type.equal(($3)->type,Type(Type::Base,Type::Float)))
       			{
       			$$->type=Type(Type::Base,Type::Float);}
       		else 
       			$$->type=Type(Type::Base,Type::Int);
        }
        else {
       		cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;
       		ABORT();
    	}	
    }	
	;

multiplicative_expression
	: unary_expression
	{
    ($$)=($1);
    }
	| multiplicative_expression '*' unary_expression
	{
		int temp;
		Type temptype=$1->type;
    	if (($1) ->type.compatible(($1)->type,($3)->type)){   //changed

    	if (($1)->cost>($3)->cost)						//lab5
    		temp=$1->cost;
   		else if ($1->cost<$3->cost) temp=$3->cost;
		else temp=$1->cost+1;

    		($$)= new BinaryOp("MULT",$1,$3);
    		($$)->cost=temp;
    		if ( ($1)->type.equal(temptype,Type(Type::Base,Type::Float))|| ($3)->type.equal(($3)->type,Type(Type::Base,Type::Float)))
       			$$->type=Type(Type::Base,Type::Float);
       		else 
       			$$->type=Type(Type::Base,Type::Int);
        }
        else {
       		cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;
       		ABORT();
    	}	
    }	 
	| multiplicative_expression '/' unary_expression 
	{
		int temp;
		Type temptype=$1->type;
    	if (($1) ->type.compatible(($1)->type,($3)->type)){   //changed
   			if (($1)->cost>($3)->cost)						//lab5
   				temp=$1->cost;
   			else if ($1->cost<$3->cost) temp=$3->cost;
   			else temp=$1->cost+1;

    		($$)= new BinaryOp("DIV_OP",$1,$3);
    		$$->cost=temp;
    		if ( ($1)->type.equal(temptype,Type(Type::Base,Type::Float))|| ($3)->type.equal(($3)->type,Type(Type::Base,Type::Float)))
       			$$->type=Type(Type::Base,Type::Float);
       		else 
       			$$->type=Type(Type::Base,Type::Int);
        }
        else {
       		cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;
       		ABORT();
    	}	
    }	
	;
	
unary_expression
	: postfix_expression  
	{
	($$)=($1);
	}				
	| unary_operator postfix_expression
	{
	($$)=new UnaryOp($1,$2);
	}
	;

postfix_expression
	: primary_expression
	{
	($$)=($1);
	}
    | IDENTIFIER '(' ')'
	{
		if(tempLocalSymbolTable->check($1)) {
				cerr<<"variable  "<< ($1) <<"is used as a functioncall in Line "<<d_scanner.lineNr()<<endl;
				ABORT();
			}
		if(globalSymbolTable.check($1)){
			lst* temp=globalSymbolTable.getTable($1);
				if((temp->noParam())!= 0) {
					cerr<<"Wrong no of arguments for function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
					ABORT();
				}
		    ($$)=new Funcall($1,new list <Expast*>());
		    ($$)->type=temp->returnType;
	    }
	    else {
	    cerr<<"Non declared Function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
	    ABORT();
	    }
    }
	| IDENTIFIER '(' expression_list ')' 
	{
		if(tempLocalSymbolTable->check($1)) {
				cerr<<"variable  "<< ($1) <<"is used as a functioncall in Line "<<d_scanner.lineNr()<<endl;
				ABORT();
			}
		if(globalSymbolTable.check($1)){
	        lst* temp=globalSymbolTable.getTable($1);
				if(($3)->size() != (temp->noParam())) {
					cerr<<"Wrong no of arguments for function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
					ABORT();
				}
				int i=0;
			for (list<Expast*> :: iterator it=($3)->begin();it!=($3)->end();it++){
				lstEntry temp2=(globalSymbolTable.getTable($1))->getEntryByOffset(4*(i+1));
		        if(temp2.param){
			        if (! ((*it)->type.compatible((*it)->type,temp2.t))){  //recent
						cerr<<"Type mismatch error for param "<<i+1 <<" in Line "<<d_scanner.lineNr()<<endl;
						ABORT();
				    }
			    }
			    i++;
			  }
			($$)=new Funcall($1,$3,temp);
			//($$)->ret_type=temp->returnType;
			($$)->type=temp->returnType;
		}
		else {
		cerr<<"Non declared Function "<< ($1) <<" in Line "<<d_scanner.lineNr()<<endl;
		ABORT();
		}
	}
	| l_expression INC_OP
	{
    ($$)=new UnaryOp("PP",$1);
	}
	;

primary_expression
	: l_expression
	{
	($$)=($1);
	}
    | l_expression '=' expression // added this production
    {	
    	if (($1) ->type.compatible(($1)->type,($3)->type)){   //changed
        ($$)= new BinaryOp("Ass",$1,$3);
        }
        else {cerr<<"Type mismatch at Line "<<d_scanner.lineNr()<<endl;ABORT();}
    }		
	| INT_CONSTANT
	{

    $$=new IntConst(($1));
    $$->cost=0;
    ($$)->type=Type(Type::Base,Type::Int);
    }
	| FLOAT_CONSTANT
	{

    $$=new FloatConst(($1));
    $$->cost=0;  //lab5
    ($$)->type=Type(Type::Base,Type::Float);
    }
    | STRING_LITERAL
    {
    $$=new StringConst(($1));
    }
	| '(' expression ')' 	
	;

l_expression
        : IDENTIFIER
        {
        Identifier *id=new Identifier($1);
        Type temp=tempLocalSymbolTable->getEntry($1).t;
        $$=new ArrayRef(id,temp);
        $$->cost=1;
        if (tempLocalSymbolTable->check($1)){
        $$->type=tempLocalSymbolTable->getEntry($1).t;        }
        else {cerr<<"Undefined variable "<<$1<<" at "<<d_scanner.lineNr()<<endl; 
        
        ABORT();
        
        }
        
        
        }
        | l_expression '[' expression ']' 
		{
		($$)=($1);
		$$->type=*(($1)->type.arrayType);
		($$)->indices->push_back(($3));
		}
        ;

expression_list
        : expression
        {
		list <Expast*> *l =new list <Expast*>();
		($$)=l;
		($$)->push_back(($1));
		}
        | expression_list ',' expression
        {
		($$)=($1);
		($$)->push_back(($3));
		}
        ;
unary_operator
        : '-'
        {
        ($$)="UNIMINUS";
        }
		| '!' 	
		{
		($$)="NOT";
		}
		;

selection_statement
        : IF '(' expression ')' statement ELSE statement 
        {
        ($$)=new If($3,$5,$7);
        }
	;

iteration_statement
	: WHILE '(' expression ')' statement 	
	{
	($$)=new While($3,$5);
	}
    | FOR '(' expression ';' expression ';' expression ')' statement  //modified this production
    {
    ($$)=new For($3,$5,$7,$9);
    }
    ;

declaration_list
        : declaration  		
        | declaration_list declaration
	;

declaration
	: type_specifier declarator_list ';'
	{
		if($1=="void"){
			cerr<<"Variable declared as Void type at "<<d_scanner.lineNr()<<endl; 
			ABORT();
		}
	}
	;

declarator_list
	: declarator
	{
		Type temp2=*(($1).type);
		int w=($1).width;
		//lst *temp=globalSymbolTable.getTable(currentFunction);
		if(tempLocalSymbolTable->check(varName)) {
			cerr<<"Variable "<<varName<<" Redeclared at "<<d_scanner.lineNr()<<endl; 
			ABORT();
		}
		offset-=w;
		tempLocalSymbolTable->add(varName,temp2,w,offset);
		
	}
	| declarator_list ',' declarator
	{
		Type temp2=*(($3).type);
		int w=($3).width;
		
		if(tempLocalSymbolTable->check(varName)) {
			cerr<<"Variable "<<varName<<" Redeclared at "<<d_scanner.lineNr()<<endl; 
			ABORT();
		}
		
		//lst* temp=globalSymbolTable.getTable(currentFunction);
		offset-=w;
		tempLocalSymbolTable->add(varName,temp2,w,offset);
		
	}
	;
