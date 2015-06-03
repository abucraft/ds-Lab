#include<sstream>
#include<string>
#include<stdexcept>
#include<map>
#include<iostream>
#include<stdlib.h>
extern std::map<std::string,int> registers;
enum ExpressionType{IDENTIFIER,COMPOUND};
enum Usererror{DBZ,NN,VND,LNE,SE,WE};
bool isNum(std::string str);
std::string getvaluename(std::stringstream& token);
class Expression{
public:
	Expression();
	virtual ~Expression();
	virtual int Eval()=0;
	virtual std::string toString()=0;
	virtual ExpressionType type()=0;
};

class IdentifierExp:public Expression{

public:

   IdentifierExp(std::string name);
   virtual int Eval();
   virtual std::string toString();
   virtual ExpressionType type();

   std::string getName();

private:

   std::string name;
};
class CompoundExp: public Expression {
public:
   CompoundExp(char op, Expression *lhs, Expression *rhs);
   virtual ~CompoundExp();
   virtual std::string toString();
   virtual ExpressionType type();
   virtual int Eval();
   char getOp();
   Expression *getLHS();
   Expression *getRHS();
private:

   char op;
   Expression *lhs, *rhs;
};
int precedence(std::string token);
Expression *readE(std::stringstream&scanner, int prec);
Expression *readT(std::stringstream&scanner);
