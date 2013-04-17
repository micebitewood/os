#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include"func.h"
using namespace std;

int main(int argv, char** argc)
{
    int alg;
    ifstream fin;
	stringstream ss;

    string s=*(argc+1);
    switch(s[2])
    {
        case 'f': alg=1; break;
        case 's': alg=2; break;
        case 'S': alg=4; break;
        case 'C': alg=8; break;
        case 'F': alg=16; break;
    }
    string infile=*(argc+2);
    fin.open(infile.c_str());
    //fin
    int i=0;
    string outfile=infile;
    while(infile[i]!=0)
        if(infile[i++]=='/')
            outfile=infile.substr(i);
    outfile=outfile.substr(5);
    outfile="out"+outfile+"_"+s[2]+"_long";
    int n=-1;
    while(!fin.eof())
    {
        getline(fin, s);
		if(s[0]!='#')
			n++;
    }
    fin.clear();
    fin.seekg(0, ios::beg);
    char c;
    fin >> c;
    while(c=='#')
    {
        getline(fin, s);
        fin >> c;
        while(c=='\n')
            fin >> c;
    }
    ss << c;
	int a, b;
	ss >> a;
	fin >> b;
    IOOperation* io=new IOOperation[n];
	io[0].set(0, a, b);
    for(i=1;i<n;i++)
    {
        fin >> a >> b;
        io[i].set(i, a, b);
    }
    fin.close();
    bool verbose=true;//long
    sched(io, n, alg, outfile, verbose);
    delete[] io;
    return 0;
}
