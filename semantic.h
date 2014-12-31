#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <string.h>
#include<cstdio>
#include <llvm/IR/Value.h>
extern "C"
{
#include "mylang.h"
}
using namespace std;


struct SYMBOL
{
	typedef enum {VAR,FUNC,CLASS,ARRAY} ACCTYPE;
	ACCTYPE acc;
	int type;//val type,0:int,1:boolean
	int size;
	string name;
	SYMBOL(ACCTYPE _acc);
};

struct Expnode:public Node
{
	enum NDTYPE{MID,STR,INT,EXP,FUNC,VAR,ARR};
	Expnode(Node*,NDTYPE);
	//type=3
	int valtype;
	virtual ~Expnode();
	llvm::Value* codeGen(class CodeContext&);
};
typedef string ERRMSG;

struct symbase
{
	map<string,SYMBOL*> m_var;
	virtual SYMBOL* find(string);
	virtual bool exists(string);
	void add_func(Node* f,class symclass* s,ERRMSG& e);
	protected:
	bool isempty(Node *nd);
	void unrolling(const Node* nd,vector<Node*>& v);
	void add_var(Node *args,ERRMSG& e);
	void space(int n);
	SYMBOL* getarg(Node*,ERRMSG&);
	void print(int n);
};
class symclass:public SYMBOL,public symbase
{
	symclass* m_extends;
	public:
	symclass();
	symclass(Node*,ERRMSG&);
	bool typecheck(ERRMSG&);
	SYMBOL* find(string);
	void print(int n);
};
struct symcode:public symbase
{
	Node* code;
	SYMBOL* getsym(Node*,ERRMSG&);
	bool isname(Node*,const char*);
	bool check(const Expnode*,int vtype,ERRMSG&);
	void setvar(Node*&,ERRMSG&);
	void setnum(Node*& b,ERRMSG& e);
	void setboolean(Node*& b,ERRMSG& e);
	void setfcall(Node*&,ERRMSG&);
	void traversal(Node*& nd,ERRMSG& e);	
	bool typecheck(ERRMSG&);
};
struct symfunc:public SYMBOL,public symcode
{
	vector<SYMBOL*> m_parameter;
	void parainit(Node*,Node*,ERRMSG&);
	symfunc(Node *f,symclass* c,ERRMSG& e);
	void print(int n);
	SYMBOL* find(string);
	symclass* parent;
};

class symprog:public symcode
{
	void sem_declar(Node*,string& e);
	void add_class(Node *c,ERRMSG& e);
	void add_array(Node *arr,ERRMSG& e);
	public:
	symclass* program;
	void add_func(Node* f,class symclass* s,ERRMSG& e);
	symprog();
	void run(Node*,bool);
	void print();
	bool typecheck(ERRMSG&);
};
struct Funcnode:public Expnode
{
	symfunc* sym;
	Funcnode(Node*,symfunc*);
	void getParameter(vector<Expnode*>& vec);
};
struct Varnode:public Expnode
{
	Varnode(Node*,SYMBOL*);
	SYMBOL* sym;
};
struct Arrnode:public Varnode
{
	Arrnode(Node*,SYMBOL*);
	void getIndices(vector<Node*>& vec);
};
symprog* semantic(bool ifprint);
#endif