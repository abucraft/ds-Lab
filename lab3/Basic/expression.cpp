#include"expression.h"
bool isNum(std::string str)
{
	for(int i=0;i<str.size();i++)
	{
		if(!((str[i]>='0'&&str[i]<='9')||str[i]=='-'))
			return false;
	}
	return true;
}
std::string getvaluename(std::stringstream& token)
{
	char c=0;
	std::string name;
	while(true)
	{
		token.get(c);
		if(c!=' ')
		{
			token.putback(c);
			break;
		}
	}
	while(true)
	{
		token.get(c);
		if(c=='='||c==' '||c=='('||c==')'||c=='+'||c=='-'||c=='*'||c=='/'||c=='<'||c=='>'||c=='\n'||token.eof())
		{
			if(c=='('||c==')')
			{
				if(name.empty())
				{
					name.push_back(c);
					break;
				}
			}
			token.putback(c);
			break;
		}
		name.push_back(c);
	}
	return name;
}
std::map<std::string,int> registers;

Expression::Expression(){}

Expression::~Expression(){}

IdentifierExp::IdentifierExp(std::string name):name(name){}

int IdentifierExp::Eval(){
	if(!isNum(name))
	{
	std::map<std::string,int>::iterator it;
	if((it=registers.find(name))!=registers.end())
		return registers[name];
	else
	{
		Usererror ur=VND;
		throw ur;
	}
	}
	else
	{
		return atoi(name.c_str());
	}
	return 0;
}

std::string IdentifierExp::toString(){return name+' ';}

ExpressionType IdentifierExp::type(){return IDENTIFIER;}

std::string IdentifierExp::getName(){return name;}

CompoundExp::CompoundExp(char op, Expression *lhs, Expression *rhs):op(op),lhs(lhs),rhs(rhs){}

CompoundExp::~CompoundExp(){delete lhs;delete rhs;}

std::string CompoundExp::toString()
{
	return lhs->toString()+rhs->toString()+op+' ';
}

int CompoundExp::Eval()
{
	int left=lhs->Eval();
	int right=rhs->Eval();
	switch(op)
	{
	case'-':
		return left-right;
	case'+':
		return left+right;
	case '*':
		return left*right;
	case'/':
		if(right==0)
		{
			Usererror ur=DBZ;
			throw ur;
		}
		else
			return left/right;
	}
	return 0;
}

ExpressionType CompoundExp::type(){return COMPOUND;}

char CompoundExp::getOp(){return op;}

Expression* CompoundExp::getLHS(){return lhs;}

Expression* CompoundExp::getRHS(){return rhs;}

int precedence(char token)
{
	//if (token == '=') return 1;
	if (token == '+' || token == '-') return 2;
	if (token == '*' || token == '/') return 3;
	return 0;
}

Expression *readE(std::stringstream&scanner, int prec)
{
	Expression *exp=0;
	exp=readT(scanner);
	char token='\0';
	while(true)
	{
		if(!scanner)
			break;
		scanner>>token;
		int newPrec=precedence(token);
		if(newPrec<=prec)
		{
			scanner.putback(token);
			break;
		}
		Expression* rhs=readE(scanner,newPrec);
		exp=new CompoundExp(token,exp,rhs);
	}
   return exp;
}

Expression *readT(std::stringstream&scanner) {
   std::string token;
   Expression *exp=0;
   token=getvaluename(scanner);
   if(token.size()!=0)
   {
	//std::cout<<token<<'\n';
	   if(token[0]!='(')
		   return new IdentifierExp(token);
	   else
		   exp = readE(scanner,0);
	   token=getvaluename(scanner);
	   if(token.size()==0)
	   {
		   std::runtime_error("Unbalanced parentheses in expression");
	   }
	   else
	   {
		   if (token[0] != ')') {
			   std::runtime_error("Unbalanced parentheses in expression");
		   }
	   }
   }
   return exp;
}
