#include<iostream>
#include<sstream>
#include<iomanip>
using namespace std;

bool is_modified(unsigned int n)
{
    if((n&0x40000000)!=0)
        return true;
    return false;
}
bool is_referenced(unsigned int n)
{
    if((n&0x20000000)!=0)
        return true;
    return false;
}
void unmap(unsigned int& pte_old)
{
        pte_old&=0x50000000;//set present bit and reference bit
}
void page_out(unsigned int& pte_old)
{
        pte_old|=0x10000000;//set pagedout bit
        unmap(pte_old);
}
void page_in(unsigned int& pte_new, int ptr)
{
    pte_new&=0xf0000000;
    pte_new|=ptr;//set frame#
    pte_new|=0xa0000000;//set present bit & reference bit
}
void modify(unsigned int& pte_new, char c)//set modified bit & referenced bit
{
    if(c=='1')
        pte_new|=0x60000000;//set modified bit & referenced bit to 1
	else
		pte_new|=0x20000000;
}

string O_out_part(unsigned long int inst_count, string operation, int pte, int frame, int& i)//for output
{
    stringstream ss;
    string s;
    
    if(operation=="ZERO")
        ss << inst_count << ": " << left << setw(7) << operation << right << setw(4) << " " << setw(2) << frame;
    else
        ss << inst_count << ": " << left << setw(7) << operation << right << setw(2) << pte << "  " << setw(2) << frame;
    getline(ss, s);
    i++;
    return s;
}

string O_out(unsigned long int inst_count, int pte_new, int pte_old, int frame, unsigned int* pte, Stats& stats)//control output
{
    string s, temp;

    temp=O_out_part(inst_count, "UNMAP", pte_old, frame, stats.unmaps);
    s=s+temp+'\n';
    if(is_modified(pte[pte_old]))
    {
        page_out(pte[pte_old]);
        temp=O_out_part(inst_count, "OUT", pte_old, frame, stats.outs);
        s=s+temp+'\n';
    }
	else
        	unmap(pte[pte_old]);
    if((pte[pte_new]|0)==0)//frame need be zerod
        temp=O_out_part(inst_count, "ZERO", pte_old, frame, stats.zeros);
    else
        temp=O_out_part(inst_count, "IN", pte_new, frame, stats.ins);
    s=s+temp+'\n';
    temp=O_out_part(inst_count, "MAP", pte_new, frame, stats.maps);
    s=s+temp+'\n';
    return s;
}

void mapin(unsigned int* pte, int* frame, int i, int pte_new, char c)//map procedure
{
	pte[frame[i]]&=0xbfffffff;//set modified bit to 0
    frame[i]=pte_new;//frame->pte
    page_in(pte[pte_new], i);
    modify(pte[pte_new], c);
}
string print_pte(unsigned int* pte)//print page table
{
    stringstream ss;
    string s;

    for(int i=0; i<64; i++)
    {
        if ((pte[i]&0x80000000)!=0)
        {
            ss << i << ":";
            if ((pte[i]&0x20000000)!=0)
                ss << 'R';
            else
                ss << '-';
            if ((pte[i]&0x40000000)!=0)
                ss << 'M';
            else
                ss << '-';
            if ((pte[i]&0x10000000)!=0)
                ss << 'S';
            else
                ss << '-';
        }
        else if ((pte[i]&0x10000000)!=0)
            ss << '#';
        else
            ss << '*';
        ss << ' ';
    }
    getline(ss, s);
    s+="\n";
    return s;
}

string print_frame(int* frame, int frame_count)//print frame table
{
    stringstream ss;
    string s;

    for(int i=0; i<frame_count; i++)
    {
        if(frame[i]==-1)
            ss << "* ";
        else
            ss << frame[i] << ' ';
    }
    getline(ss, s);
    return s;
}

string print_stat(int inst_count, Stats stats)//print stats
{
    stringstream ss;
    string s;

    ss << "SUM " << inst_count << " U=" << stats.unmaps << " M=" << stats.maps;
    ss << " I=" << stats.ins << " O=" << stats.outs << " Z=" << stats.zeros;
    getline(ss, s);
    return s;
}

string print_totalcost(int inst_count, Stats stats)
{
    unsigned long int totalcost=0;

    totalcost+=inst_count;
    totalcost+=400*(stats.maps+stats.unmaps);
    totalcost+=3000*(stats.ins+stats.outs);
    totalcost+=150*stats.zeros;
    stringstream ss;
    ss.clear();
    ss << " ===> " << totalcost;
    string s;
    getline(ss, s);
    return s;
}

void Arr_manager(int* Arr, int* frame, int frame_count, char c, int i)//for 
{
    int temp, j, k;

    if(c=='u')//update
    {
		for(j=0;j<frame_count;j++)
			if(Arr[j]==i)
				break;
        temp=i;
        for(k=j+1;frame[k]!=-1 && k<frame_count;k++)
            Arr[k-1]=Arr[k];
        Arr[k-1]=temp;
    }
    else if(c=='c')//create
    {
        Arr[i]=i;
    }
    else if(c=='s')//swap
    {
		temp=Arr[0];
        for(j=1;j<frame_count;j++)
            Arr[j-1]=Arr[j];
        Arr[j-1]=temp;
    }
}

void ArrN_manager(int* ArrN, unsigned int pte, char c, char command)//for N
{
    if(command=='u')
    {
        if((pte&0x60000000)==0)//R==0, M==0
		{
			ArrN[0]--;
			if(c=='1')
				ArrN[3]++;
			else
				ArrN[2]++;
		}
        else if((pte&0x60000000)==0x40000000)//R==0, M==1
		{
            ArrN[1]--;
			ArrN[3]++;
		}
        else if((pte&0x60000000)==0x20000000)//R==1, M==0
			if(c=='1')
			{
				ArrN[2]--;
				ArrN[3]++;
			}
    }
    else if(command=='c')
        if(c=='1')
            ArrN[3]++;
        else
            ArrN[2]++;
}
