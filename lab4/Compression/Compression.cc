#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include<map>
#include<vector>
#include<list>
using namespace std;

/*-----------HuffmanCode-----------
*store the Huffman code of a byte,the length is the bit length of the Huffman code
*/
class HuffmanCode{
public:
	int length;
	string code;
	HuffmanCode(int len=0,string co=""):length(len),code(co){}
	void add(unsigned char c)//c is just 0 or 1
	{
		int i=length/8;
		int j=length%8;
		c=c<<(7-j);
		if(j==0)
			code.push_back(c);
		else
			code[i]|=c;
		length++;
	}
	void clear(){length=0;code="";}
	void print(){
		for(int len=0;len<length;len++)
		{
			int i=len/8;
			int j=len%8;
			unsigned char mask=1<<(7-j);
			unsigned char c=(code[i]&mask)>>(7-j);
			int out=c;
			printf("%u",out);
		}
	}
};
bool operator<(const HuffmanCode& left,const HuffmanCode& right){return left.length<right.length;}
bool operator==(const HuffmanCode& left,const HuffmanCode& right){return left.length==right.length&&left.code==right.code;}

/*-----------HuffmanTree----------
*the Huffman tree struct
*use the getcode() function to get a char's Huffman code
*/
class HuffmanTree{
public:
	HuffmanTree* leftchild;
	HuffmanTree* rightchild;
	char content;
	int weight;
	HuffmanTree(char c,int w,HuffmanTree* left=0,HuffmanTree* right=0):content(c),weight(w),leftchild(left),rightchild(right){}
	~HuffmanTree(){
		if(leftchild!=0)
			delete leftchild;
		if(rightchild!=0)
			delete rightchild;
	}
	HuffmanCode getcode(char c,HuffmanCode& path){
		if(leftchild==0&&rightchild==0&&content==c)
			return path;
		else
		{
			HuffmanCode right,left;
			if(leftchild!=0)
			{
				left=path;
				left.add(0);
				left=leftchild->getcode(c,left);
			}
			if(rightchild!=0)
			{
				right=path;
				right.add(1);
				right=rightchild->getcode(c,right);
			}
			if(left.length!=0)
				return left;
			if(right.length!=0)
				return right;
			HuffmanCode temp;
			return temp;
		}
	}
};

/*-----------BitToken-----------
*read from a string or a file
*each time read a single bit
*/
class BitToken{
	long long point;
	string buffer;
	long long length;
public:
	BitToken(string str,long long length):point(0),length(length){
		buffer=str;
	}
	BitToken(ifstream&ifs):point(0){
		ifs.read((char *)&length,sizeof(long long));
		char c;
		while(ifs.get(c))
		{
			buffer.push_back(c);
		}
	}
	~BitToken(){}
	//get single bit
	bool get(unsigned char &c){

		if(point>=length)
			return false;
		long long i=point/8;
		long long j=point%8;
		unsigned char mask=1<<(7-j);
		c=(buffer[i]&mask)>>(7-j);
		point++;
		return true;
	}
	BitToken& operator ++(){
		point++;
		return (*this);
	}
	BitToken& operator --(){
		point--;
		return (*this);
	}
	BitToken& operator +=(int index){
		for(int i=0;i<index;i++)
		{
			++(*this);
		}
		return *this;
	}
	BitToken& operator -=(int index){
		for(int i=0;i<index;i++)
		{
			--(*this);
		}
		return *this;
	}
	bool eof(){
		return point>=length;
	}
	bool overbegin(){return point<0;}
};

/*-----------BitWriter-----------
*write the Huffman code into a file
*/
class BitWriter{
	long long point;
	string buffer;
public:
	BitWriter():point(0),buffer(""){}
	void writefile(ofstream&ofs){
		ofs.write((char*)&point,sizeof(long long));
		for(int i=0;i<buffer.size();i++)
		{
			ofs.put(buffer[i]);
		}
	}
	void write(HuffmanCode&hf)
	{
		write(hf.length,hf.code);
	}
	void write(long long length,string code)
	{
		BitToken bt(code,length);
		unsigned char c;
		while(bt.get(c))
		{
			int i=point/8;
			int j=point%8;
			c=c<<(7-j);
			if(j==0)
				buffer.push_back(c);
			else
				buffer[i]=buffer[i]|c;
			point++;
		}
	}
};

/*-----------HuffmanCodebindChar-----------
*use to search the char by its Huffman code
*/
class HuffmanCodebindChar{
public:
	HuffmanCode code;
	char content;
	HuffmanCodebindChar(HuffmanCode hfmcd,char c):code(hfmcd),content(c){}
};

void naiveCopy(string inputFilename, string outputFilename) {
  ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
  ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
  char c;
  while (ifs.get(c)) ofs.put(c);
  ofs.close();
  ifs.close();
}
list<HuffmanTree*>::iterator findmin(list<HuffmanTree*>&forest)
{
	int weight=(*forest.begin())->weight;
	list<HuffmanTree*>::iterator dest,it;
	dest=forest.begin();
	for(it=forest.begin();it!=forest.end();it++)
	{
		if((*it)->weight<weight)
		{
			weight=(*it)->weight;
			dest=it;
		}
	}
	return dest;
}
void compress(string inputFilename, string outputFilename) {
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	map<char,int>bytemap;
	char c;
	//count the byte show times
	while(ifs.get(c))
	{
		map<char,int>::iterator it;
		if((it=bytemap.find(c))==bytemap.end())
		{
			bytemap[c]=1;
		}
		else
			bytemap[c]++;
	}
	list<HuffmanTree*> forest;
	map<char,int>::iterator bmpit;
	//create subtrees
	for(bmpit=bytemap.begin();bmpit!=bytemap.end();bmpit++)
	{
		HuffmanTree* hfmtree=new HuffmanTree((*bmpit).first,(*bmpit).second);
		forest.push_back(hfmtree);
	}
	//create huffman tree
	while(forest.size()>1)
	{
		list<HuffmanTree*>::iterator left,right;
		HuffmanTree *lefttree,*righttree,*newtree;
		right=findmin(forest);
		righttree=(*right);
		forest.erase(right);
		left=findmin(forest);
		lefttree=(*left);
		forest.erase(left);
		newtree=new HuffmanTree(0,lefttree->weight+righttree->weight,lefttree,righttree);
		forest.push_back(newtree);
	}
	HuffmanTree* finaltree=(*forest.begin());
	// get huffman code
	map<char,HuffmanCode> codes;
	for(bmpit=bytemap.begin();bmpit!=bytemap.end();bmpit++)
	{
		HuffmanCode tempcode;
		codes[(*bmpit).first]=finaltree->getcode((*bmpit).first,tempcode);
	}
	//put information to the head of output file
	int size=bytemap.size();
	ofs.write((char *)(&size),sizeof(int));
	for(bmpit=bytemap.begin();bmpit!=bytemap.end();bmpit++)
	{
		ofs.put((*bmpit).first);
		ofs.write((char *)(&(*bmpit).second),sizeof(int));
	}
	//compress the file
	ifs.close();
	ifs.open(inputFilename.c_str(), ios::in | ios::binary);
	BitWriter buffer;
	while(ifs.get(c))
	{
		buffer.write(codes[c]);
	}
	buffer.writefile(ofs);
	ofs.close();
	ifs.close();
};

void decompress(string inputFilename, string outputFilename) {
	ifstream ifs(inputFilename.c_str(), ios::in | ios::binary);
	ofstream ofs(outputFilename.c_str(), ios::out | ios::binary);
	map<char,int>bytemap;
	int size;
	ifs.read((char *)(&size),sizeof(int));
	for(int i=0;i<size;i++)
	{
		char tempc;
		int tempi=0;
		ifs.get(tempc);
		ifs.read((char*)&tempi,sizeof(int));
		bytemap[tempc]=tempi;
	}
	list<HuffmanTree*> forest;
	map<char,int>::iterator bmpit;
	//create subtrees
	for(bmpit=bytemap.begin();bmpit!=bytemap.end();bmpit++)
	{
		HuffmanTree* hfmtree=new HuffmanTree((*bmpit).first,(*bmpit).second);
		forest.push_back(hfmtree);
	}
	//create huffman tree
	while(forest.size()>1)
	{
		list<HuffmanTree*>::iterator left,right;
		HuffmanTree *lefttree,*righttree,*newtree;
		right=findmin(forest);
		righttree=(*right);
		forest.erase(right);
		left=findmin(forest);
		lefttree=(*left);
		forest.erase(left);
		newtree=new HuffmanTree(0,lefttree->weight+righttree->weight,lefttree,righttree);
		forest.push_back(newtree);
	}
	HuffmanTree* finaltree=(*forest.begin());
	// get huffman code
	vector<HuffmanCodebindChar> codes;
	for(bmpit=bytemap.begin();bmpit!=bytemap.end();bmpit++)
	{
		HuffmanCode tempcode;
		tempcode=finaltree->getcode((*bmpit).first,tempcode);
		codes.push_back(HuffmanCodebindChar(tempcode,(*bmpit).first));
		//cout<<(*bmpit).first<<':';
		//tempcode.print();
		//cout<<'\n';
	}
	BitToken bt(ifs);
	ifs.close();
	unsigned char bit;
	HuffmanCode hfmcd;
	while(bt.get(bit))
	{
		hfmcd.add(bit);
		for(int i=0;i<codes.size();i++)
		{
			if(hfmcd==codes[i].code)
			{
				ofs.put(codes[i].content);
				hfmcd.clear();
				break;
			}
		}
	}
	ofs.close();
}

void useage(string prog) {
  cerr << "Useage: " << endl
      << "    " << prog << "[-d] input_file output_file" << endl;
  exit(2);
}

int main(int argc, char* argv[]) {
  int i;
  string inputFilename, outputFilename;
  bool isDecompress = false;
  for (i = 1; i < argc; i++) {
    if (argv[i] == string("-d")) isDecompress = true;
    else if (inputFilename == "") inputFilename = argv[i];
    else if (outputFilename == "") outputFilename = argv[i];
    else useage(argv[0]);
  }
  if (outputFilename == "") useage(argv[0]);
  if (isDecompress) decompress(inputFilename, outputFilename);
  else compress(inputFilename, outputFilename);
  return 0;
}
