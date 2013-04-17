#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<unistd.h>
#include<stdlib.h>
#include"alg.h"
using namespace std;

string map(unsigned long int inst_count, char alg, unsigned int* pte, int frame_count, int* frame, char c, int pte_new, int op, Stats& stats, int& rnum, unsigned int* aging)
     //choose algorithm and control output of pfO
{
	int i;
	static int ptr=0, Arr[64], ArrN[4]={0,0,0,0};//Arr:for l, ArrN:for N
    string s="", temp, st;
    stringstream ss;

    ss << "==> inst: " << c << " " << pte_new;
    getline(ss, temp);
    st=st+temp+'\n';
    for (i=0;i<frame_count;i++)
		if(frame[i]==pte_new)
		{
			if(alg=='N')
                ArrN_manager(ArrN, pte[frame[i]], c, 'u');//update array list for N
            modify(pte[pte_new], c);
            if(alg=='l')
                Arr_manager(Arr, frame, frame_count, 'u', i);//update array list for l
			break;
		}
        else if(frame[i]==-1)
        {
            temp=O_out_part(inst_count, "ZERO", pte_new, i, stats.zeros);
            st=st+temp+'\n';
            temp=O_out_part(inst_count, "MAP", pte_new, i, stats.maps);
            st=st+temp+'\n';
            frame[i]=pte_new;
			page_in(pte[pte_new], i);
			modify(pte[pte_new], c);
            if(alg=='l')
                Arr_manager(Arr, frame, frame_count, 'c', i);//create an element in array list for l
            else if(alg=='N')
                ArrN_manager(ArrN, pte[pte_new], c, 'c');//create an element in array list for N
            break;
        }
    if(i==frame_count)//choose algorithm for replacement
    {
        if (alg=='f')
            temp=alg_f(inst_count, pte, frame_count, frame, ptr, c, pte_new, stats);
        else if(alg=='s')
            temp=alg_s(inst_count, pte, frame_count, frame, ptr, c, pte_new, stats);
        else if(alg=='c')
            temp=alg_c(inst_count, pte, frame_count, frame, ptr, c, pte_new, stats);
        else if(alg=='l')
            temp=alg_l(inst_count, pte, frame_count, frame, Arr, c, pte_new, stats);
        else if(alg=='r')
            temp=alg_r(inst_count, pte, frame_count, frame, rnum, c, pte_new, stats);
        else if(alg=='a')
            temp=alg_a(inst_count, pte, frame_count, frame, aging, c, pte_new, stats);
        else if(alg=='N')
            temp=alg_N(inst_count, pte, frame_count, frame, ArrN, rnum, c, pte_new, stats);
        else if(alg=='C')
            temp=alg_C(inst_count, pte, frame_count, frame, ptr, c, pte_new, stats);
        else if(alg=='A')
            temp=alg_A(inst_count, pte, frame_count, frame, aging, c, pte_new, stats);
		st=st+temp;
    }
    if((op&8)!=0)//if O, print
        s=s+st;
    if((op&2)!=0)//if p, print
        s=s+print_pte(pte);
    if((op&4)!=0)//if f, print
    {
        s=s+print_frame(frame, frame_count);
        if(alg=='f' || alg=='s')
        {
            ss.clear();
            ss << " || ";
            for(i=0;i<frame_count;i++)
				if(frame[(ptr+i)%frame_count]!=-1)
					ss << (ptr+i)%frame_count << ' ';
				else 
					break;
            getline(ss, temp);
            s=s+temp+'\n';
        }
        else if(alg=='C')
        {
            ss.clear();
            ss << " || hand = " << ptr%64;
            getline(ss, temp);
            s=s+temp+'\n';
        }
        else if(alg=='c')
        {
            ss.clear();
            ss << " || hand = " << ptr%frame_count;
            getline(ss, temp);
            s=s+temp+'\n';
        }
		else if(alg=='a')
		{
			ss.clear();
			ss << " || ";
			for(i=0;i<frame_count;i++)
				if(frame[i]!=-1)
					ss << dec << i << ':' << hex << aging[i] << ' ';
				else 
					break;
			getline(ss, temp);
            s=s+temp+'\n';
		}
        else if(alg=='A')
        {
            ss.clear();
            ss << " ||";
            for(i=0;i<64;i++)
                if((pte[i]&0x80000000)==0)
                    ss << " *";
                else
                    ss << " " << dec << i << ":" << hex << aging[i];
            getline(ss, temp);
            s=s+temp+'\n';
        }
		else
			s=s+'\n';
    }
    return s;
}

int main(int argv, char** argc)
{
    int i, frame_count=32, op=0, rcount, j;
    int inst_count=0;
    unsigned int pte[64];
    char alg='l';
    string options;
    char** files;
    string infile, outfile="out_", s;
    ifstream fin, rfin;
	ofstream fout;
    int opt;
    stringstream ss;

    while((opt=getopt(argv, argc, "a:o:f:"))!=-1)
        switch(opt)
        {
            case 'a':
                alg=optarg[0];
                break;
            case 'o':
                ss.clear();
                ss << optarg;
                ss >> options;
                j=0;
                while(options[j])
                {
                    switch(options[j])
                    {
                        case 'a':
                            op|=1;
                            break;
                        case 'p':
                            op|=2;
                            break;
                        case 'f':
                            op|=4;
                            break;
                        case 'O':
                            op|=8;
                            break;
                        case 'P':
                            op|=16;
                            break;
                        case 'F':
                            op|=32;
                            break;
                        case 'S':
                            op|=64;
                            break;
                    }
                    j++;
                }
                break;
            case 'f':
                frame_count=atoi(optarg);
                break;
        }
    files=argc+optind;
    ss.clear();
    ss << *files;
    ss >> infile;
    if(*(files+1)!=NULL)
    {
        ss.clear();
        ss << *(files+1);
        ss >> s;
        rfin.open(s.c_str());
        rfin >> rcount;
    }
    //input arguments
    fin.open(infile.c_str());
	if(!fin.is_open())
		return 0;
    i=0;
	s=infile;
    while(infile[i]!=0)
    {
        if(infile[i]=='/')
            s=infile.substr(i+1);
        i++;
    }
    outfile=outfile+s+'_'+alg+'_';
    ss.clear();
    ss << frame_count;
    ss >> s;
    outfile=outfile+s+'_'+options;
    fout.open(outfile.c_str());
    //output set
	for(i=0;i<64;i++)
		pte[i]=0;
    //initialize pte
    int* frame=new int[frame_count];
    for (i=0; i<frame_count; i++)
        frame[i]=-1;
    //create and initialize frames
    unsigned int aging[64];
    for(i=0;i<64;i++)
        aging[i]=0;
    //for aging algorithm
    Stats stats;
    int rnum=0;
    while(!fin.eof())
    {
        char c;
        do
        {
            fin >> c;
        }while(c=='\n');
		if(fin.eof())
			break;
        if(c=='#')
            getline(fin, s);
        else
        {
            int pte_new;
            fin >> pte_new;
            if(alg=='r' || alg=='N')
            {
                if(rcount==0)
                {
                    rfin.clear();
                    rfin.seekg(0, ios::beg);
                    rfin >> rcount;
                }
				if(rnum==0)
				{
					rfin >> rnum;
					rcount--;
				}
            }
            s=map(inst_count, alg, pte, frame_count, frame, c, pte_new, op, stats, rnum, aging);
            if((op&14)!=0)//if p|f|O
                fout << s;
            inst_count++;
       }
    }
    if((op&16)!=0)//if P
        fout << print_pte(pte);
    if((op&32)!=0)//if F
        fout << print_frame(frame, frame_count) << endl;
    if((op&64)!=0)//if S
        fout << print_stat(inst_count, stats) << print_totalcost(inst_count, stats) << endl;
    delete[] frame;
    fin.close();
    fout.close();
    if(rfin.is_open())
        rfin.close();
}
