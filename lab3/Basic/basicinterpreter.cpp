#include"basicinterpreter.h"
#include<algorithm>
#include<cctype>
void changestring(std::string&dest)
{
	std::transform(dest.begin(),dest.end(),dest.begin(),::tolower);
}


Basicline::Basicline(){}
Basicline::~Basicline(){}
//class Rem
Remline::Remline(std::string raw):linenum(0){
	std::stringstream token(raw);
	token>>linenum;
}
Remline::~Remline(){}
void Remline::Eval(){}
enum Basicinstruction Remline::kind(){Basicinstruction kd=REM;return kd;}
unsigned int Remline::Linenum(){return linenum;}

//class Let
Letline::Letline(std::string raw):linenum(0){
	std::stringstream token(raw);
	token>>linenum;
	if(!token)
	{
		token.clear();
		token.str(raw);
	}
	std::string temp;
	char tempc;
	token>>temp;
	vname=getvaluename(token);
	token>>tempc;
	if(tempc!='=')
	{
		Usererror ur=WE;
		throw ur;
	}
	exp=readE(token,0);
}
Letline::~Letline()
{
	delete exp;
}
enum Basicinstruction Letline::kind(){Basicinstruction kd=LET;return kd;}
unsigned int Letline::Linenum(){return linenum;}
void Letline::Eval(){
	std::map<std::string,int>::iterator it;
	if((it=registers.find(vname))==registers.end())
		registers[vname]=0;
	registers[vname]=exp->Eval();
}

//class Print
Printline::Printline(std::string raw):linenum(0){
	std::stringstream token(raw);
	token>>linenum;
	if(!token)
	{
		token.clear();
		token.str(raw);
	}
	std::string temp;
	token>>temp;
	exp=readE(token,0);
}
Printline::~Printline(){}
void Printline::Eval(){
	std::cout<<exp->Eval()<<'\n';
}
enum Basicinstruction Printline::kind(){Basicinstruction kd=PRINT;return kd;}
unsigned int Printline::Linenum(){return linenum;}

//class Input
Inputline::Inputline(std::string raw):linenum(0){
	std::stringstream token(raw);
	token>>linenum;
	if(!token)
	{
		token.clear();
		token.str(raw);
	}
	std::string temp;
	token>>temp;
	vname=getvaluename(token);
	if(vname=="")
	{
		Usererror ur=SE;
		throw ur;
	}
}
Inputline::~Inputline(){}
void Inputline::Eval(){
	std::map<std::string,int>::iterator it;
	if((it=registers.find(vname))==registers.end())
		registers[vname]=0;
	while(true){
		std::cout<<" ? ";
		std::string tmp;
		std::getline(std::cin,tmp);
		if(isNum(tmp))
		{
			registers[vname]=atoi(tmp.c_str());
			break;
		}
		else
		{
			std::cout<<"INVALID NUMBER\n";
		}
	}
}
enum Basicinstruction Inputline::kind(){Basicinstruction kd=INPUT;return kd;}
unsigned int Inputline::Linenum(){return linenum;}

//class End
Endline::Endline(std::string raw):linenum(0){
	std::stringstream token(raw);
	token>>linenum;
}
Endline::~Endline(){}
void Endline::Eval(){}
enum Basicinstruction Endline::kind(){Basicinstruction kd=END;return kd;}
unsigned int Endline::Linenum(){return linenum;}

//class Goto
Gotoline::Gotoline(std::string raw,void*basic):linenum(0),dest(0),basicitp(basic){
	std::stringstream token(raw);
	token>>linenum;
	std::string temp;
	token>>temp;
	dest=readE(token,0);
}
Gotoline::~Gotoline(){}
void Gotoline::Eval(){
	((Basicinterpreter*)basicitp)->changepc(dest->Eval());
}
enum Basicinstruction Gotoline::kind(){Basicinstruction kd=GOTO;return kd;}
unsigned int Gotoline::Linenum(){return linenum;}

//class If
Ifline::Ifline(std::string raw,void*basic):linenum(0),dest(0),basicitp(basic){
	std::stringstream token(raw);
	token>>linenum;
	std::string temp;
	token>>temp;
	left=readE(token,0);
	token>>op;
	if(!(op=='='||op=='>'||op=='<'))
	{
		Usererror kd=WE;
		throw kd;
	}
	right=readE(token,0);
	token>>temp;
	if(temp!="then")
	{
		Usererror kd=WE;
		throw kd;
	}
	token>>dest;
	if(!token)
	{
		Usererror kd=WE;
		throw kd;
	}
}
Ifline::~Ifline(){
	delete left;
	delete right;
}
void Ifline::Eval(){
	bool resualt=false;
	switch(op)
	{
	case'<':
		if((left->Eval())<(right->Eval()))
			resualt=true;
		break;
	case'=':
		if((left->Eval())==(right->Eval()))
			resualt=true;
		break;
	case'>':
		if((left->Eval())>(right->Eval()))
			resualt=true;
		break;
	}
	if(resualt)
	{
		((Basicinterpreter*)basicitp)->changepc(dest);
	}
}
enum Basicinstruction Ifline::kind(){Basicinstruction kd=IF;return kd;}
unsigned int Ifline::Linenum(){return linenum;}

//class Basicinterpreter
Basicinterpreter::Basicinterpreter():pc(0){}
void Basicinterpreter::deletelines(){
	for(int i=0;i<lines.size();i++)
	{
		delete lines[i];
	}
	lines.clear();
}
Basicinterpreter::~Basicinterpreter(){deletelines();}
void Basicinterpreter::addrawline(std::string raw){
	unsigned int templinenum;
	unsigned int currentlinenum;
	std::stringstream rawstrm(raw);
	rawstrm>>currentlinenum;
	for(int i=0;i<rawlines.size();i++)
	{
		std::stringstream tempstrm(rawlines[i]);
		tempstrm>>templinenum;
		if(templinenum==currentlinenum)
		{
			rawlines[i]=raw;
			return;
		}
		if(templinenum>currentlinenum)
		{
			rawlines.insert(rawlines.begin()+i,raw);
			return;
		}
	}
	rawlines.push_back(raw);
}
void Basicinterpreter::addline(Basicline* line){
	unsigned int linenum=line->Linenum();
	for(int i=0;i<lines.size();i++)
	{
		if(lines[i]->Linenum()==linenum)
		{
			delete lines[i];
			lines[i]=line;
			return;
		}
		if(lines[i]->Linenum()>linenum)
		{
			lines.insert(lines.begin()+i,line);
			return;
		}
	}
	//if linenum > all lines linenum,add this line to the end of lines
	lines.push_back(line);
}
void Basicinterpreter::deleteline(unsigned int linenum){
	for(int i=0;i<lines.size();i++)
	{
		if(lines[i]->Linenum()==linenum)
		{
			delete lines[i];
			lines.erase(lines.begin()+i);
		}
	}
	for(int i=0;i<rawlines.size();i++)
	{
		std::stringstream strm(rawlines[i]);
		unsigned int templinenum;
		strm>>templinenum;
		if(templinenum==linenum)
		{
			rawlines.erase(rawlines.begin()+i);
		}
	}
}
void Basicinterpreter::changepc(unsigned int dest){pc=dest;}
void Basicinterpreter::Eval(){
	if(lines.size()==0)
		return;
	pc=lines[0]->Linenum();
	Basicinstruction lastkind=REM;
	while(true)
	{
		Basicline * evalline=0;
		for(int i=0;i<lines.size();i++)
		{
			if(lines[i]->Linenum()==pc)
			{
				evalline=lines[i];
				if(i<lines.size()-1)
				{
					pc=lines[i+1]->Linenum();
				}
				else
				{
					pc++;
				}
				break;
			}
		}
		if(evalline==0)
		{
			if(lastkind==GOTO)
			{
				Usererror ur=LNE;
				throw ur;
			}
			return;
		}
		else
		{
			if((lastkind=evalline->kind())==END)
				return;
			evalline->Eval();
		}
	}
}
void Basicinterpreter::run(){
	while(true)
	{
		try{
			std::string temp;
			while(temp.empty())
			{
				getline(std::cin,temp);
			}
			std::string rawtemp=temp;
			changestring(temp);
			if(temp=="quit")
				break;
			if(temp=="list")
			{
				for(int i=0;i<rawlines.size();i++)
				{
					std::cout<<rawlines[i]<<'\n';
				}
				continue;
			}
			if(temp=="clear")
			{
				rawlines.clear();
				deletelines();
				registers.clear();
				continue;
			}
			if(temp=="help")
			{
				std::cout<<"姓名: 李晟\n学号:5130379017\n";
				continue;
			}
			if(temp=="run")
			{
				Eval();
				continue;
			}
			std::string inst;
			unsigned int linenum;
			std::stringstream strm(temp);
			Basicline *templine;
			strm>>inst;
			if(inst=="print")
			{
				Printline tmpline(temp);
				tmpline.Eval();
				continue;
			}
			if(inst=="input")
			{
				Inputline tmpline(temp);
				tmpline.Eval();
				continue;
			}
			if(inst=="let")
			{
				Letline tmpline(temp);
				tmpline.Eval();
				continue;
			}
			if(isNum(inst))
			{
				linenum=atoi(inst.c_str());
			}
			else
			{
				Usererror ur=SE;
				throw ur;
			}
			strm>>inst;
			if(!strm)
			{
				deleteline(linenum);
				continue;
			}
			if(inst=="rem")
			{
				templine=new Remline(temp);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="let")
			{
				templine=new Letline(temp);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="print")
			{
				templine=new Printline(temp);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="input")
			{
				templine=new Inputline(temp);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="end")
			{
				templine=new Endline(temp);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="goto")
			{
				templine=new Gotoline(temp,this);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			if(inst=="if")
			{
				templine=new Ifline(temp,this);
				addline(templine);
				addrawline(rawtemp);
				continue;
			}
			Usererror ur=WE;
			throw ur;
		}
		catch(Usererror error)
		{
			switch(error)
			{
			case DBZ:
				std::cout<<"DIVIDE BY ZERO\n";
				break;
			case NN:
				std::cout<<"NVALID NUMBER\n";
				break;
			case VND:
				std::cout<<"VARIABLE NOT DEFINED\n";
				break;
			case LNE:
				std::cout<<"LINE NUMBER ERROR\n";
				break;
			case SE:
				std::cout<<"SYNTAX ERROR\n";
				std::cin.clear();
				break;
			case WE:
				std::cout<<"WRONG EXPRESSION\n";
				std::cin.clear();
				break;
			}
		}
	}
}
