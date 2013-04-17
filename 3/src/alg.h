#include<iostream>
#include"class.h"
#include"func.h"
using namespace std;

string alg_f(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int& ptr, char c, int pte_new, Stats& stats)
{
    string s;

    ptr=ptr%frame_count;
    s=O_out(inst_count, pte_new, frame[ptr], ptr, pte, stats);
	mapin(pte, frame, ptr, pte_new, c);
	ptr++;
    return s;
}

string alg_s(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int& ptr, char c, int pte_new, Stats& stats)
{
    string s;

    ptr=ptr%frame_count;
    while(is_referenced(pte[frame[ptr]]))
    {
        pte[frame[ptr]]&=0xdfffffff;
        ptr++;
        ptr=ptr%frame_count;
    }
    s=O_out(inst_count, pte_new, frame[ptr], ptr, pte, stats);
	mapin(pte, frame, ptr, pte_new, c);
	ptr++;
    return s;
}

string alg_c(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int& ptr, char c, int pte_new, Stats& stats)
{
    string s;

    ptr=ptr%frame_count;
    while(is_referenced(pte[frame[ptr]]))
    {
        pte[frame[ptr]]&=0xdfffffff;//set referenced bit to 0
        ptr++;
        ptr=ptr%frame_count;
    }
    s=O_out(inst_count, pte_new, frame[ptr], ptr, pte, stats);
	mapin(pte, frame, ptr, pte_new, c);
	ptr++;
    return s;
}

string alg_l(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int* Arr, char c, int pte_new, Stats& stats)
{
    string s;

    s=O_out(inst_count, pte_new, frame[Arr[0]], Arr[0], pte, stats);
	mapin(pte, frame, Arr[0], pte_new, c);
    Arr_manager(Arr, frame, frame_count, 's', pte_new);//swap
    return s;
}

string alg_r(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int& rnum, char c, int pte_new, Stats& stats)
{
    string s;

    rnum=rnum%frame_count;
    s=O_out(inst_count, pte_new, frame[rnum], rnum, pte, stats);
	mapin(pte, frame, rnum, pte_new, c);
	rnum=0;
    return s;
}

string alg_a(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, unsigned int* aging, char c, int pte_new, Stats& stats)
{
    string s;
    unsigned int min=0xffffffff;
	int i;

    for(i=0;i<frame_count;i++)
    {
        if(is_referenced(pte[frame[i]]))
        {
            pte[frame[i]]&=0xdfffffff;//set reference bit to 0
            aging[i]=(aging[i]>>1);
            aging[i]|=0x80000000;
        }
        else//reference bit is 0
            aging[i]=(aging[i]>>1);
        if(aging[i]<min)
            min=aging[i];
    }
    for(i=0;aging[i]!=min;i++)
    {}
	aging[i]=0;
    s=O_out(inst_count, pte_new, frame[i], i, pte, stats);
	mapin(pte, frame, i, pte_new, c);
    return s;
}

string alg_N(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int* ArrN, int& rnum, char c, int pte_new, Stats& stats)
{
    int i;
    string s;

    int j=0, k[4]={0x80000000, 0xc0000000, 0xa0000000, 0xe0000000};
    while(ArrN[j]==0)
        j++;
    rnum=rnum%ArrN[j];
    for(i=0;i<64;i++)
        if((pte[i]&0xe0000000)==k[j])
        {
            if(rnum==0)
                break;
            else
                rnum--;
        }
    rnum=pte[i]&0x0fffffff;//extract the frame_num
    s=O_out(inst_count, pte_new, i, rnum, pte, stats);
    ArrN[j]--;
	if(stats.unmaps%10==0)//the 10th unmap
    {
        for(i=0;i<64;i++)
            if((pte[i]&0x80000000)!=0)//if present
                pte[i]&=0xdfffffff;//set reference bit to 0
        ArrN[0]+=ArrN[2];
        ArrN[1]+=ArrN[3];
        ArrN[2]=0;
        ArrN[3]=0;
    }
	mapin(pte, frame, rnum, pte_new, c);
    ArrN_manager(ArrN, pte[pte_new], c, 'c');//create
	rnum=0;
    return s;
}

string alg_C(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, int& ptr, char c, int pte_new, Stats& stats)
{
    string s;

    while(true)
    {
        ptr=ptr%64;
        if((pte[ptr]&0xa0000000)==0x80000000)//present bit is 1 & reference bit is 0
                break;
        else if((pte[ptr]&0xa0000000)==0xa0000000)//present bit is 1 & reference bit is 1
            pte[ptr]&=0xdfffffff;//set reference bit to 0
        ptr++;
    }
    int rnum=pte[ptr]&0x0fffffff;//extract the frame_num
    s=O_out(inst_count, pte_new, ptr, rnum, pte, stats);
	mapin(pte, frame, rnum, pte_new, c);
    ptr++;
    return s;
}

string alg_A(unsigned long int inst_count, unsigned int* pte, int frame_count, int* frame, unsigned int* aging, char c, int pte_new, Stats& stats)
{
    string s;
    unsigned int min=0xffffffff;
	int i;

    for(i=0;i<64;i++)
    {
        if((pte[i]&0x80000000)!=0)//present bit is 1
        {
            if(is_referenced(pte[i]))//reference bit is 1
            {
                pte[i]&=0xdfffffff;//set reference bit to 0
                aging[i]=aging[i]>>1;
                aging[i]|=0x80000000;
            }
            else
                aging[i]=aging[i]>>1;
            if(aging[i]<min)
                min=aging[i];
        }
    }
    i=0;
    while(true)
    {
        if((pte[i]&0x80000000)!=0)//present bit is 1
        {
            if(aging[i]==min)
                break;
        }
        i++;
    }
	aging[i]=0;
    int rnum=pte[i]&0x0fffffff;//extract the frame_num
    s=O_out(inst_count, pte_new, i, rnum, pte, stats);
	mapin(pte, frame, rnum, pte_new, c);
    return s;
}

