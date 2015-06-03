
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
using namespace std;
/*
 * Your code here.
 */
void initialgraph(int nodenum,int **&graph)
{
	graph=new int*[nodenum];
	for(int i=0;i<nodenum;i++)
	{
		graph[i]=new int[nodenum];
	}
	for(int i=0;i<nodenum;i++)
	{
		for(int j=0;j<nodenum;j++)
		{
			if(i==j)
				graph[i][j]=0;
			else
				graph[i][j]=-1;
		}
	}
}
void deletegraph(int nodenum,int**graph)
{
	for(int i=0;i<nodenum;i++)
	{
		delete [] graph[i];
	}
	delete [] graph;
}
bool near(int a,int b)
{
	if(b==-1)
		return true;
	if(a==-1)
		return false;
	return a<b;
}
int addpath(int a,int b)
{
	if(a==-1||b==-1)
		return -1;
	return a+b;
}
void findminpath(int nodenum,int*nodes,int**graph,int*minpaths)
{
	bool* original,*produced;
	int minnode;
	int min;
	original=new bool[nodenum];
	produced=new bool[nodenum];
	for(int i=0;i<nodenum;i++)
	{
		original[i]=true;
		produced[i]=false;
		minpaths[i]=-1;
	}
	minpaths[0]=0;
	min=-1;
	for(int i=0;i<nodenum;i++)
	{
		min=-1;
		//seek the nearest node to the zero
		for(int j=0;j<nodenum;j++)
		{
			//not in the set
			if(!produced[j])
			{
				if(near(minpaths[j],min))
				{
					min=minpaths[j];
					minnode=j;
				}
			}
		}
		//add the node to the set
		produced[minnode]=true;
		original[minnode]=false;
		//update the path
		for(int k=0;k<nodenum;k++)
		{
			if(original[k])
			{
				int path=addpath(minpaths[minnode],graph[minnode][k]);
				if(near(path,minpaths[k]))
				{
					minpaths[k]=path;
				}
			}
		}
	}
	delete [] original;
	delete [] produced;
}
int main(int argc,char**argv)
{
	if(argc<2)
	{
		cout<<"Usage: tidy [FILENAME]\n";
		return 0;
	}
	ifstream ifs(argv[1]);
	if(!ifs)
	{
		cout<<"can't open the input file";
	}
	int nodenum;
	int arcnum;
	int* nodes;
	int** graph;
	int* minpaths;
	string temp;
	stringstream tempsstr;
	getline(ifs,temp);
	tempsstr.str(temp);
	tempsstr>>nodenum;
	nodenum++;
	nodes=new int[nodenum];
	minpaths=new int[nodenum];
	initialgraph(nodenum,graph);
	getline(ifs,temp);
	tempsstr.clear();
	tempsstr.str(temp);
	tempsstr>>arcnum;
	for(int i=0;i<arcnum;i++)
	{
		tempsstr.clear();
		getline(ifs,temp);
		tempsstr.str(temp);
		int node1,node2,arc;
		tempsstr>>node1;
		tempsstr>>node2;
		tempsstr>>arc;
		graph[node1][node2]=arc;
	}
	findminpath(nodenum,nodes,graph,minpaths);
	for(int i=0;i<nodenum;i++)
	{
		cout<<minpaths[i]<<' ';
	}
	cout<<'\n';
	delete [] nodes;
	delete [] minpaths;
	deletegraph(nodenum,graph);
	return 0;
}


