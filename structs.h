
#include <iostream>
#include <list>
#include <unordered_map>
#include <string>
#include <vector>
using namespace std;
int getCounter();
#define loadsome(some1,some2){\
	gen("load"+string(some1)+"(ind(esp),ebx);");\
	gen("pop"+string(some1)+"(1);");\
	gen("load"+string(some2)+"(ind(esp),eax);");\
	gen("pop"+string(some2)+"(1);");\
}
#define bool_op(jump,iorf){\
	gen("cmp"+string(iorf)+"(eax,ebx);");\
	int c=getCounter();\
	gen(string(jump)+"(l"+to_string(c)+");");\
	gen("pushi(0);");\
	int d=getCounter();\
	gen("j(l"+to_string(d)+");");\
	gen("l"+to_string(c)+":");\
	gen("pushi(1);");\
	gen("l"+to_string(d)+":");\
	\
}
class Type {
public:
	//struct arrayType{
	//int length;
	//Type* t;
	//arrayType(arrayType a){
		
	//}
	//void printType(){
		//cout<<"array("<<length<<",";
		//t->printType();
		//cout<<")";
	//}
	//};
	enum Kind {
		Base, Array, Error, Ok
	};
	enum Basetype {
		Int, Float, Void
	};
	Kind tag;
	int size;
	union {
		Basetype basetype;
		Type* arrayType;
	};
	//Type(Type&);
	 Type() :
		tag(Ok) {
}
 Type(Kind kindval) :
		tag(kindval) {
}

 Type(Kind kindval, Basetype btype) :
		tag(kindval), basetype(btype) {
}

 Type(Kind kindval, Type* type ,int s) :
		tag(kindval){
			arrayType=type;
			size=s;

}

// Type::~Type()
// {switch (tag) {
//    case Pointer: delete pointed;
//    break;
// }}
void  printType()
{ 
  switch (tag) 
   {
   case Base: switch (basetype) 
     {
       case Int: std::cout << "Int" ;
       break;
       case Float: std::cout << "Float" ;
       break;
       case Void: std::cout << "Void" ;
     };
    break;
  case Error:std::cout << "Error"  ;
    break;
  case Ok:std::cout << "Ok" ;
    break;
  case Array:
	std::cout << "array("<<size<<",";
    arrayType->printType();
    std::cout<<")";
  }
}

bool equal(Type t1, Type t2) {
	if (t1.tag == Type::Base && t2.tag == Type::Base)
		return (t1.basetype == t2.basetype);
	else if (t1.tag == Type::Array && t2.tag == Type::Array)
		return (equal(*(t1.arrayType),*(t2.arrayType))&&(t1.size==t2.size));
}

bool compatible(Type t1, Type t2) {
	if ((t1.basetype == Type::Int || t1.basetype == Type::Float)
			&& (t2.basetype == Type::Int || t2.basetype == Type::Float))
		return 1;
		
	else if (t1.tag == Type::Array && t2.tag == Type::Array)
		return (compatible(*(t1.arrayType),*(t2.arrayType))&&(t1.size==t2.size));
	else
		return equal(t1, t2);

}
};
	



struct lstEntry{
	Type t;
	string name;
	string value;
	int width;
	bool initialized;
	bool param;
	int offset;
	
	//lstEntry(string n,string T,string v){
	lstEntry(string n,int of){
		name=n;
		//Type=T;
		//value=v;
		offset=of;
		value="null";
		width=0;
		initialized=false;
		param=false;
	}
	lstEntry(string n,Type T,int W,int of){
		name=n;
		t=T;
		offset=of;
		//value=v;
		width=W;
		value="null";
		initialized=false;
		param=false;
	}
	void print(){
		t.printType();
		cout <<" \t"<<name<<" \t"<<value<<" \t"<<param<<" \t"<<offset<<endl;	
	}
	
};
struct lst {
	vector<lstEntry> localTable;
	Type returnType;
	//void add(string name,string Type,string value){
	void add(string name,int of){
		lstEntry temp(name,of);
		localTable.push_back(temp);
		return;
	}
	void add(string name,Type T,int w,int of){
		lstEntry temp(name,T,w,of);
		
		localTable.push_back(temp);
		//cout<<localTable.size()<<endl;
		return;
	}
	void addParam(string name,Type T,int w,int of){
		lstEntry temp(name,T,w,of);
		temp.param=true;
		localTable.push_back(temp);
		//cout<<localTable.size()<<endl;
		return;
	}
	
	int noParam(){
		int count=0;
		for (vector<lstEntry>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->param) count++;
		}
		return count;
	}
	
	bool check(string name){
		for (vector<lstEntry>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->name == name) return true;
		}
		return false;
	}
	lstEntry getEntryByOffset(int of){
		for (vector<lstEntry>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->offset == of) return (*it);
		}
	}
	lstEntry getEntry(string name){
		for (vector<lstEntry>::iterator it=localTable.begin(); it!= localTable.end(); ++it){
			if(it->name == name) return (*it);
		}
	}
	
	void print(){
		cout<< "Return Type :";
		returnType.printType();
		cout<<endl;
		for(int i=0;i<localTable.size();i++){
			localTable[i].print();
		}
		cout<<endl;
	}
	
};

struct gst {
	unordered_map<string, lst*> entries;	//hash table of funcName, pointer to lst
	
	void add(string funcName){
		lst* table=new lst();
		entries[funcName]=table;
	}
	void add(string funcName,lst* table){
		entries[funcName]=table;
	}
	bool check(string funcName){
		if(entries.find(funcName) != entries.end()) return true;
		return false;
	}
	lst* getTable(string funcName){
		return entries[funcName];
	}
	
	void print(){
		for ( auto it = entries.begin(); it != entries.end(); ++it ){
			cout << "Local Symbol Table of " << it->first << ":";
			(it->second)->print();
		}
		
	}
	
};

struct decla{
	Type* type;
	int width;
	decla(Type* t,int w){
		type=t;
		width=w;
	}
	decla(){
		
	}
	
};

void gen(string s);



class abstract_astnode
{
public:
virtual void print () = 0;
Type type;
virtual void generate_code(lst* SymTab) = 0;
/*
//virtual basic_types getType() = 0;
//virtual bool checkTypeofAST() = 0;
public:
//virtual void setType(basic_types) = 0;
private:
typeExp astnode_type;
* */
};

class Stmtast : public abstract_astnode
{
    public:
    virtual void print () {
		}
    
};
class Expast : public abstract_astnode
{
    public:
    bool isleaf;
    int cost;
    virtual void print (){
    
    
    }
};
class block_statement : public Stmtast
{
    public:
    list <Stmtast*> *l;
    block_statement(list<Stmtast*>* l) : l(l){
	}
    virtual void print () {
		cout<<"(Block [";
		for (list <Stmtast*>::iterator it=l->begin();it!=l->end();it++){
			(*it)->print();
		}
		cout<<"])\n";
	}
	void generate_code(lst* SymTab){
		for (list <Stmtast*>::iterator it=l->begin();it!=l->end();it++){
			(*it)->generate_code(SymTab);
		}
	}
};
class Ass :public Stmtast
{
    public:
    Expast* left;
    Expast* right;
    Ass(Expast* left,Expast* right) : left(left),right(right){
		}
	virtual void generate_code(lst* SymTab){
		right->generate_code(SymTab);
		left->generate_code(SymTab);
	if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
		gen("popi(1);");
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("storei(eax,ind(ebp,edi));");

		}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
		gen("popf(1);");
		gen("loadf(ind(esp),eax);");
		gen("popf(1);");
		gen("storef(eax,ind(ebp,edi));");
			}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
		gen("popi(1);");
		gen("loadf(ind(esp),eax);");
		gen("popf(1);");
		gen("floatToint(eax);");
		gen("storei(eax,ind(ebp,edi));");
				
			}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
		gen("popf(1);");
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("intTofloat(eax);");
		gen("storef(eax,ind(ebp,edi));");
				
			}



	}
    virtual void print(){
		cout<<"(Assign_exp ";
     		if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
 				left->print();
 				cout<<"(TO_INT ";
 				right->print();
 				cout<<")";
 			}
 			else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
 				left->print();
 				cout<<"(TO_FLOAT ";
 				right->print();
 				cout<<")";
 			}
 
 			else {
 				left->print();
 				right->print();
 				cout<<")";
 			}
 			cout<<"\n";
	}
};
class Return : public Stmtast
{
    public:
    Type ret_type;
    int noparam;
    Expast* stmt;
    Return(Expast* stmt,Type ret_type,int noparam) : stmt(stmt),ret_type(ret_type),noparam(noparam){
	}
    virtual void print (){
    cout<<"(Return ";
    if (ret_type.equal(ret_type,Type(Type::Base,Type::Int)) && stmt->type.equal(stmt->type,Type(Type::Base,Type::Float))){
	    cout<<"(TO_INT ";
	    stmt->print();
	    cout<<")";
	    cout<<")\n";
	}
	else if (ret_type.equal(ret_type,Type(Type::Base,Type::Float)) && stmt->type.equal(stmt->type,Type(Type::Base,Type::Int))){
	    cout<<"(TO_FLOAT ";
	    stmt->print();
	    cout<<")";
	    cout<<")\n";
	}
	else {
		 stmt->print();
		 cout<<")\n";
	}
    
    }
    void generate_code(lst* SymTab){
    	stmt->generate_code(SymTab);
    	if (ret_type.equal(ret_type,Type(Type::Base,Type::Int)) && stmt->type.equal(stmt->type,Type(Type::Base,Type::Float))){
    		gen("loadi(ind(esp),eax);");
    		gen("popi(1);");
    		gen("floatToint(eax);");
    		gen("storei(eax,ind(ebp,"+to_string(4*(noparam+1))+"));");
    }
    else if (ret_type.equal(ret_type,Type(Type::Base,Type::Float)) && stmt->type.equal(stmt->type,Type(Type::Base,Type::Int))){
    	gen("loadi(ind(esp),eax);");
    	gen("popi(1);");
    	gen("intTofloat(eax);");
    	gen("storef(eax,ind(ebp,"+to_string(4*(noparam+1))+"));");
    }
    else if (ret_type.equal(ret_type,Type(Type::Base,Type::Float))){
    	gen("loadf(ind(esp),eax);");
    	gen("popf(1);");
    	gen("storef(eax,ind(ebp,"+to_string(4*(noparam+1))+"));");
    }
    else{
    	gen("loadi(ind(esp),eax);");
    	gen("popi(1);");
    	gen("storei(eax,ind(ebp,"+to_string(4*(noparam+1))+"));");

    }
}

};
class If : public Stmtast
{
    public:
    Expast* condition;
    Stmtast* then;
    Stmtast* else_stmt;
    If(Expast* condition,Stmtast* then,Stmtast* else_stmt) : condition(condition),then(then),else_stmt(else_stmt){
    	}
    virtual void generate_code(lst* SymTab){
    	condition->generate_code(SymTab);
    	gen("loadi(ind(esp),eax);");
    	gen("popi(1);");
    	gen("cmpi(0,eax);");
    	int c=getCounter();
    	int d=getCounter();
    	gen("je(l"+to_string(c)+");");	
    	then->generate_code(SymTab);
    	gen("j(l"+to_string(d)+");");
    	gen("l"+to_string(c)+":");
    	else_stmt->generate_code(SymTab);
    	gen("l"+to_string(d)+":");
    	

    }
    virtual void print () {
		cout<<"(If ";
		condition->print();
		cout<<endl;
		then->print();
		cout<<endl;
		else_stmt->print();
		cout<<")\n";
	}
	
    
};
class While : public Stmtast
{
    public:
    Expast* condition;
    Stmtast* stmt;
    While(Expast* condition,Stmtast* stmt) : condition(condition),stmt(stmt){
	}
	virtual void generate_code(lst* SymTab){
		int c=getCounter();
		int d=getCounter();
		gen("l"+to_string(c)+":");
		condition->generate_code(SymTab);
		gen("loadi(ind(esp),eax);");
		gen("cmpi(0,eax);");
    	gen("je(l"+to_string(d)+");");
    	stmt->generate_code(SymTab);
    	gen("j(l"+to_string(c)+");");
    	gen("l"+to_string(d)+":");
    	
	}
    virtual void print () {
		cout<<"(While ";
		condition->print();
		cout<<endl;
		stmt->print();
		cout<<")\n";
	}
	
};
class For : public Stmtast
{
    public:
    Expast* init;
    Expast* guard;
    Expast* step;
    Stmtast* stmt;
    For(Expast* init,Expast* guard,Expast* step,Stmtast* stmt) : init(init),guard(guard),step(step),stmt(stmt){
	}
	virtual	void generate_code(lst* SymTab){
	init->generate_code(SymTab);
	int c=getCounter();
	int d=getCounter();
	gen("l"+to_string(c)+":");
	guard->generate_code(SymTab);
	gen("loadi(ind(esp), eax);");
	gen("cmpi(0,eax);");
    gen("je(l"+to_string(d)+");");
    stmt->generate_code(SymTab);
    step->generate_code(SymTab);
    gen("j(l"+to_string(c)+");");
    gen("l"+to_string(d)+":");
    

	}	
    virtual void print (){
		cout<<"(For ";
		init->print();
		cout<<endl;
		guard->print();
		cout<<endl;
		step->print();
		cout<<endl;
		stmt->print();
		cout<<")\n";
	}	

};
class FuncallStmt : public Stmtast
{
	public:
	string name;
	lst* func;
	list <Expast*> *l;
	FuncallStmt(string name,list <Expast*> *l) : l(l),name(name){
	}
	FuncallStmt(string name,list <Expast*> *l,lst* func) : l(l),name(name),func(func){
	}
	void generate_code(lst* SymTab){
		Type rettype=this->func->returnType;
		Type temp,paratype;
		int i=4*(this->func)->noParam();
		if (rettype.equal(rettype,Type(Type::Base,Type::Int))){
			gen("pushi(0);");
		}
		else if (rettype.equal(rettype,Type(Type::Base,Type::Float))){
			gen("pushf(0.0);");

		}
		for (list<Expast*>::reverse_iterator it=l->rbegin();it!=l->rend();it++){
			(*it)->generate_code(SymTab);
			paratype=this->func->getEntryByOffset(i).t;
			temp=(*it)->type;
			if (temp.equal(temp,Type(Type::Base,Type::Int)) && paratype.equal(paratype,Type(Type::Base,Type::Float))){
			 	gen("loadi(ind(esp),eax);");
			 	gen("popi(1);");
			 	gen("intTofloat(eax);");
			 	gen("pushf(eax);");
			}
			else if (temp.equal(temp,Type(Type::Base,Type::Float)) && paratype.equal(paratype,Type(Type::Base,Type::Int))){
				gen("loadf(ind(esp),eax);");
			 	gen("popf(1);");
			 	gen("floatToint(eax);");
			 	gen("pushi(eax);");
			}
			i-=4;
		}
		i+=4;
		gen(name+"();");
		for (list<Expast*>::iterator it=l->begin();it!=l->end();it++){
			paratype=this->func->getEntryByOffset(i).t;
			if (paratype.equal(paratype,Type(Type::Base,Type::Float)))
				gen("popf(1);");
			else gen("popi(1);");

		}
	}

	virtual void print(){
		Type temp,paratype;
		int i=4;
		cout<<"(Funcall ("<<name<<") ";
		for (list<Expast*> ::iterator it=l->begin();it!=l->end();it++){
			paratype=this->func->getEntryByOffset(i).t;
			temp=(*it)->type;
			if (temp.equal(temp,Type(Type::Base,Type::Int)) && paratype.equal(paratype,Type(Type::Base,Type::Float))){
				cout<<"(TO_FLOAT ";
				(*it)->print();
				cout<<")";
			}
			else if (temp.equal(temp,Type(Type::Base,Type::Float)) && paratype.equal(paratype,Type(Type::Base,Type::Int))){
				cout<<"(TO_INT ";
				(*it)->print();
				cout<<")";
			}
			else {
				(*it)->print();
			}
			i+=4;
			}

		cout<<")\n";
	}
};
class BinaryOp : public Expast
{
	public:
	string operation;
	Expast* left;
	Expast* right;
	BinaryOp(string t,Expast* left,Expast* right) : left(left),right(right),operation(t){
	}
	virtual void generate_code(lst* SymTab){
	if (operation=="AND_OP"){
		left->generate_code(SymTab);
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("cmpi(0,eax);");
		int c=getCounter();
		int d=getCounter();
		gen("je(l"+to_string(c)+");");	
		right->generate_code(SymTab);
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("cmpi(0,eax);");
		gen("je(l"+to_string(c)+");");
		gen("pushi(1);");
		gen("j(l"+to_string(d)+");");
		gen("l"+to_string(c)+":");
		gen("pushi(0);");
		gen("l"+to_string(d)+":");
		
	}
	else if (operation=="OR_OP"){
		left->generate_code(SymTab);
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("cmpi(1,eax);");
		int c=getCounter();
		int d=getCounter();
		gen("je(l"+to_string(c)+");");
		right->generate_code(SymTab);
		gen("loadi(ind(esp),eax);");
		gen("popi(1);");
		gen("cmpi(1,eax);");
		gen("je(l"+to_string(c)+");");
		gen("pushi(0);");
		gen("j(l"+to_string(d)+");");
		gen("l"+to_string(c)+":");
		gen("pushi(1);");
		gen("l"+to_string(d)+":");
		

	}
	else{
	left->generate_code(SymTab);
	right->generate_code(SymTab);


	if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
		loadsome("i","i");
		if (operation=="LT"){
			bool_op("jl","i");
		}
		else if (operation=="GT"){
			bool_op("jg","i");
		}
		else if (operation=="LE_OP"){
			bool_op("jle","i");
		}
		else if (operation=="GE_OP"){
			bool_op("jge","i");
		}
		else if (operation=="EQ_OP"){
			bool_op("je","i");
		}
		else if (operation=="NE_OP"){
			bool_op("jne","i");
		}

		else if (operation=="PLUS"){
			gen("addi(ebx,eax);");
			gen("pushi(eax);");
		}
		else if (operation=="MINUS"){
			gen("muli(-1,ebx);");
			gen("addi(ebx,eax);");
			gen("pushi(eax);");

		}
		else if (operation=="MULT"){
			gen("muli(ebx,eax);");
			gen("pushi(eax);");
		}

		}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
		loadsome("f","f");
		if (operation=="LT"){
			bool_op("jl","f");
		}
		else if (operation=="GT"){
			bool_op("jg","f");
		}
		else if (operation=="LE_OP"){
			bool_op("jle","f");
		}
		else if (operation=="GE_OP"){
			bool_op("jge","f");
		}
		else if (operation=="EQ_OP"){
			bool_op("je","f");
		}
		else if (operation=="NE_OP"){
			bool_op("jne","f");
		}
		else if (operation=="PLUS"){
			gen("addf(ebx,eax);");
			gen("pushf(eax);");
		}
		else if (operation=="MINUS"){
			gen("mulf(-1,ebx);");
			gen("addf(ebx,eax);");
			gen("pushf(eax);");

		}
		else if (operation=="MULT"){
			gen("mulf(ebx,eax);");
			gen("pushf(eax);");
		}
			}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
		loadsome("f","i");
		if (operation=="LT"){
			gen("intTofloat(eax);");
			bool_op("jl","f");
		}
		else if (operation=="GT"){
			gen("intTofloat(eax);");
			bool_op("jg","f");
		}
		else if (operation=="LE_OP"){
			gen("intTofloat(eax);");
			bool_op("jle","f");
		}
		else if (operation=="GE_OP"){
			gen("intTofloat(eax);");
			bool_op("jge","f");
		}
		else if (operation=="EQ_OP"){
			gen("intTofloat(eax);");
			bool_op("je","f");
		}
		else if (operation=="NE_OP"){
			gen("intTofloat(eax);");
			bool_op("jne","f");
		}
		else if (operation=="PLUS"){
			gen("intTofloat(eax);");
			gen("addf(ebx,eax);");
			gen("pushf(eax);");
		}
		else if (operation=="MINUS"){
			gen("intTofloat(eax);");
			gen("mulf(-1,ebx);");
			gen("addf(ebx,eax);");
			gen("pushf(eax);");

		}
		else if (operation=="MULT"){
			gen("intTofloat(eax);");
			gen("mulf(ebx,eax);");
			gen("pushf(eax);");
		}
				
			}
	else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
		loadsome("i","f");
		if (operation=="LT"){
			gen("intTofloat(ebx);");
			bool_op("jl","f");
		}
		else if (operation=="GT"){
			gen("intTofloat(ebx);");
			bool_op("jg","f");
		}
		else if (operation=="LE_OP"){
			gen("intTofloat(ebx);");
			bool_op("jle","f");
		}
		else if (operation=="GE_OP"){
			gen("intTofloat(ebx);");
			bool_op("jge","f");
		}
		else if (operation=="EQ_OP"){
			gen("intTofloat(ebx);");
			bool_op("je","f");
		}
		else if (operation=="NE_OP"){
			gen("intTofloat(ebx);");
			bool_op("jne","f");
		}
		else if (operation=="PLUS"){
			gen("intTofloat(ebx);");
			gen("addf(ebx,eax);");
			gen("pushf(eax);");
		}
		else if (operation=="MINUS"){
			gen("intTofloat(ebx);");
			gen("mulf(-1,ebx);");
			gen("addf(ebx,eax);");
			gen("pushf(eax);");

		}

		else if (operation=="MULT"){
			gen("intTofloat(ebx);");
			gen("mulf(ebx,eax);");
			gen("pushf(eax);");
		}
				
			}

	}



	/*	if (left->isleaf){
			gen("loadi(ind(esp),"+ rstack.top()+");");
			gen("popi(1);");
			if (right->isleaf) {
			gen("(ind(esp),"+ rstack.top());
			gen("popi(1);");
			}
		}
		if (right->isleaf) {
			left->generate_code();
			gen("(ind(esp),"+ rstack.top());

		}
		*/
	}
	virtual void print(){
		if (operation=="Ass"){
 			if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
 				cout<<"("<<operation<<" ";
 				left->print();
 				cout<<"(TO_INT ";
 				right->print();
 				cout<<")";
 			}
 			else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
 				cout<<"("<<operation<<" ";
 				left->print();
 				cout<<"(TO_FLOAT ";
 				right->print();
 				cout<<")";
 			}
 			else {
 				cout<<"("<<operation<< " ";
 				left->print();
 				right->print();
 				cout<<")";
 
 			}
 
 
 		}
 		else if (operation=="OR_OP" || operation=="AND_OP" || operation== "EQ_OP" || operation== "NE_OP"){
		cout<<"("<<operation<< " ";
		left->print();
		right->print();
		cout<<")";
		}
		else {
			if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
				cout<<"("<<operation<<"_INT ";
				left->print();
				right->print();
			}
			else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
					cout<<"("<<operation<<"_FLOAT ";
					left->print();
					right->print();
			}
			else if (left->type.equal(left->type,Type(Type::Base,Type::Int)) && right->type.equal(right->type,Type(Type::Base,Type::Float))){
					cout<<"("<<operation<<"_FLOAT (TO_FLOAT ";
					left->print();
					cout<<") ";
					right->print();
			}
			else if (left->type.equal(left->type,Type(Type::Base,Type::Float)) && right->type.equal(right->type,Type(Type::Base,Type::Int))){
					cout<<"("<<operation<<"_FLOAT ";
					left->print();
					cout << " (TO_FLOAT ";
					right->print();
					cout<<") ";
			}
			cout<<")";
			}
		}
};
class UnaryOp : public Expast
{
	public:
	string operation;
	Expast* var;
	UnaryOp(string t,Expast* var) : operation(t),var(var){
	}
	virtual void print(){
		cout<<"("<<operation<<" ";
		var->print();
		cout<<")";
	}
	void generate_code(lst* SymTab){
		if (operation=="PP"){
			var->generate_code(SymTab);
			if (var->type.equal(var->type,Type(Type::Base,Type::Float))){
				gen("loadf(ind(esp),eax);");
				gen("popf(1);");
				gen("addf(1,eax);");
				gen("pushf(eax);");
		}
		else{
			gen("loadi(ind(esp),eax);");
			gen("popi(1);");
			gen("addi(1,eax);");
			gen("pushi(eax);");
		}
	}
}
};
class Funcall : public Expast
{
	public:
	string name;
	lst* func;
	list <Expast*> *l;
	Funcall(string name,list <Expast*> *l) : l(l),name(name){
	}
	Funcall(string name,list <Expast*> *l,lst* func) : l(l),name(name),func(func){
	}
	void generate_code(lst* SymTab){
		Type rettype=this->func->returnType;
		Type temp,paratype;
		int i=4*(this->func)->noParam();
		if (rettype.equal(rettype,Type(Type::Base,Type::Int))){
			gen("pushi(0);");
		}
		else if (rettype.equal(rettype,Type(Type::Base,Type::Float))){
			gen("pushf(0.0);");

		}
		for (list<Expast*>::reverse_iterator it=l->rbegin();it!=l->rend();it++){
			(*it)->generate_code(SymTab);
			paratype=this->func->getEntryByOffset(i).t;
			temp=(*it)->type;
			if (temp.equal(temp,Type(Type::Base,Type::Int)) && paratype.equal(paratype,Type(Type::Base,Type::Float))){
			 	gen("loadi(ind(esp),eax);");
			 	gen("popi(1);");
			 	gen("intTofloat(eax);");
			 	gen("pushf(eax);");
			}
			else if (temp.equal(temp,Type(Type::Base,Type::Float)) && paratype.equal(paratype,Type(Type::Base,Type::Int))){
				gen("loadf(ind(esp),eax);");
			 	gen("popf(1);");
			 	gen("floatToint(eax);");
			 	gen("pushi(eax);");
			}
			i-=4;
		}
		i+=4;
		gen(name+"();");
		for (list<Expast*>::iterator it=l->begin();it!=l->end();it++){
			paratype=this->func->getEntryByOffset(i).t;
			if (paratype.equal(paratype,Type(Type::Base,Type::Float)))
				gen("popf(1);");
			else gen("popi(1);");

		}
	}

	virtual void print(){
		Type temp,paratype;
		int i=4;
		cout<<"(Funcall ("<<name<<") ";
		for (list<Expast*> ::iterator it=l->begin();it!=l->end();it++){
			paratype=this->func->getEntryByOffset(i).t;
			temp=(*it)->type;
			if (temp.equal(temp,Type(Type::Base,Type::Int)) && paratype.equal(paratype,Type(Type::Base,Type::Float))){
				cout<<"(TO_FLOAT ";
				(*it)->print();
				cout<<")";
			}
			else if (temp.equal(temp,Type(Type::Base,Type::Float)) && paratype.equal(paratype,Type(Type::Base,Type::Int))){
				cout<<"(TO_INT ";
				(*it)->print();
				cout<<")";
			}
			else {
				(*it)->print();
			}
			i+=4;
			}

		cout<<")\n";
	}
};
class FloatConst : public Expast
{
	public:
	float value;
	FloatConst() {}
	FloatConst (float f){
		value=f;
	}
	virtual void generate_code(lst* SymTab){
		gen("pushf("+to_string(value)+");");
	}
	virtual void print(){
		cout<<"(FloatConst ";
		cout<<value;
		cout<<")";
	}
};
class IntConst : public Expast
{
	public:
	int value;
	IntConst() {}
	IntConst (int i){
		value=i;
	}
	virtual void generate_code(lst* SymTab){
		gen("pushi("+to_string(value)+");");
	}
	virtual void print(){
		cout<<"(IntConst ";
		cout<<value;
		cout<<")";
	}
};
class StringConst: public Expast
{
	public:
	string value;
	StringConst(){}
	StringConst (string s){
		value=s;
		}
	virtual void print(){
		cout<<"(StringConst ";
		cout<<value;
		cout<<")";
	}void generate_code(lst* SymTab){}
};
class Identifier: public Expast
{
	public:
	string name;
	Identifier(){}
	Identifier (string s){
		name=s;
		}
	virtual void generate_code(lst* SymTab){
		int offset=SymTab->getEntry(name).offset;
		gen("loadi(ind(ebp,+"+to_string(offset)+"),eax);");
		gen("pushi(eax);");
		gen("move("+to_string(offset)+",edi);");


	}
	virtual void print(){
		cout<<"(Id \""<<name<<"\")";
	}
};

class ArrayRef: public Expast
{
	public:
	Identifier *id;
	list<Expast*> *indices;
	Type full_type;
	ArrayRef(Identifier* id,Type full_type): id(id),full_type(full_type){
		//id= new Identifier();
		indices= new list<Expast*>();
	}

	virtual void generate_code(lst* SymTab){
		if (this->indices->empty()){
		int offset=SymTab->getEntry(id->name).offset;
		if (this->full_type.equal(this->full_type,Type(Type::Base,Type::Float))){
			gen("loadf(ind(ebp,"+to_string(offset)+"),eax);");
			gen("pushf(eax);");
			gen("move("+to_string(offset)+",edi);");
		}
		else{
		gen("loadi(ind(ebp,"+to_string(offset)+"),eax);");
		gen("pushi(eax);");
		gen("move("+to_string(offset)+",edi);");
	}
	}
		
	else{
		int offset=SymTab->getEntry(id->name).offset;
		Type temp=SymTab->getEntry(id->name).t;
		gen("move("+to_string(offset)+",ecx);");
		int i=1;
		for (list<Expast*> :: iterator it=(this->indices)->begin();it!=(this->indices)->end();it++){
			(*it)->generate_code(SymTab);
			gen("loadi(ind(esp),eax);");
			gen("popi(1);");
			if (i!=indices->size())
				gen("muli("+to_string(temp.size)+",eax);");
			gen("addi(eax,ecx);");
			temp=*(temp.arrayType);
			i++;
		}if (this->full_type.equal(this->full_type,Type(Type::Base,Type::Float))){
			gen("loadf(ind(ebp,ecx),eax);");
			gen("pushf(eax);");
			gen("move(ecx,edi);");
		}
		else{
			gen("loadi(ind(ebp,ecx),eax);");
			gen("pushi(eax);");
			gen("move(ecx,edi);");
		}
	}

	}
	virtual void print(){
		if (indices->size()==0){
			id->print();
		}

		else{
		cout<<"(Array ";

		id->print();
		for (list<Expast*> ::iterator it=indices->begin();it!=indices->end();it++){
			cout <<"[";
			(*it)->print();
			cout << "]";
		}
		cout<<")";
		}
	}
};

class empty : public Stmtast
{
    public:
    virtual void print () {
		cout<<"(Empty)";
	}void generate_code(lst* SymTab){}
};

void print_code();



class printCom
{public:
list<Expast*> expr;
printCom(){}
printCom(list<Expast*> expr):expr(expr){
	
}

};






 

    
