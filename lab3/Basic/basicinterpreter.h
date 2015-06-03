#ifndef _BASICINTERPRETER_
#define _BASICINTERPRETER_
#include<vector>
#include<string>
#include<map>
#include<iostream>
#include"expression.h"
enum Basicinstruction {REM,LET,PRINT,INPUT,END,GOTO,IF};
class Basicline{
public:
	Basicline();
	virtual ~Basicline();
	virtual void Eval()=0;
	virtual Basicinstruction kind()=0;
	virtual unsigned int Linenum()=0;
};

class Remline:public Basicline{
	unsigned int linenum;
public:
	Remline(std::string raw);
	virtual ~Remline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Letline:public Basicline{
	std::string vname;
	Expression *exp;
	unsigned int linenum;
public:
	Letline(std::string raw);
	virtual ~Letline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Printline:public Basicline{
	Expression *exp;
	unsigned int linenum;
public:
	Printline(std::string raw);
	virtual ~Printline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Inputline:public Basicline{
	std::string vname;
	unsigned int linenum;
public:
	Inputline(std::string raw);
	virtual ~Inputline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Endline:public Basicline{
	unsigned int linenum;
public:
	Endline(std::string raw);
	virtual ~Endline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Gotoline:public Basicline{
	void *basicitp;	//a point to Basicinterpreter 
	Expression *dest;
	unsigned int linenum;
public:
	Gotoline(std::string raw,void*basic);
	virtual ~Gotoline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Ifline:public Basicline{
	void *basicitp;
	unsigned int linenum;
	unsigned int dest;
	char op;
	Expression*left;
	Expression*right;
public:
	Ifline(std::string raw,void*basic);
	virtual ~Ifline();
	virtual void Eval();
	virtual Basicinstruction kind();
	virtual unsigned int Linenum();
};

class Basicinterpreter{
	std::vector<std::string> rawlines;
	std::vector<Basicline*> lines;
	unsigned int pc;
	void deletelines();
public:
	Basicinterpreter();
	~Basicinterpreter();
	void addline(Basicline *line);
	void addrawline(std::string raw);
	void deleteline(unsigned int linenum);
	void changepc(unsigned int pc);
	void Eval();
	void run();
};
#endif
