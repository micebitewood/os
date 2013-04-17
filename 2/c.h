#include<iostream>
using namespace std;

bool is_run=false;//judgement of processor status
int endtime=1;//the time when processor is free
int bn=0;//blocked processes' number
double iu=0;//I/O utilization

class process
{
private:
    int pid;//process id
    int at;//arrival time
    int tc;//total CPU time
    int cb;//CPU burst
    int ib;//IO burst
    int stat;//status number:0ready,1run,2block,-1create
    int rem;//remaining time
    int bt;//burst time
	int ibt;//io burst time
    int nt;//next event time
    int cw;//CPU waiting time
	int dur;//during time between events
	bool flag;//state change
    process* next;//pointer to the next process
public:
    void set(int,int,int,int,int);
    int getpid(){return pid;}
    int getat(){return at;}
    int gettc(){return tc;}
    int getcb(){return cb;}
    int getib(){return ib;}
    int getstat(){return stat;}
    int getnt(){return nt;}
    int getbt(){return bt;}
    int getrem(){return rem;}
	int getcw(){return cw;}
	int getdur(){return dur;}
	bool getflag(){return flag;}
    int getnextnt(){return next->getnt();}
    int getnextbt(){return next->getbt();}
    int getnextstat(){return next->getstat();}
    int getnextrem(){return next->getrem();}
    process* getnext(){return next;}
    void gcb(int);//generate cb
    void gib(int);//generate ib
    void cstat();//change stat
    void rstat(int);//change stat run to ready
    void crem(int);//calculate remaining time
    void snt(int);//set next time
    void snext(process*);//set pointer to the next process
};

void process::rstat(int r_num)
{
    if (stat==0 && is_run)
    {
        cw+=endtime-nt;
		dur+=endtime-nt;
        nt=endtime;
		flag=false;
    }
    else
    {
		flag=true;
        if(stat==1 && ibt==0 && rem!=0)
        {
            stat=0;
            bt=bt-r_num;
			endtime=nt;
			is_run=false;
			dur=0;
        }
        else
        {
			if(stat==2)
			{
				bn--;
				if(bn==0)
					iu+=nt;
			}
            stat++;
            stat=stat%3;
            if (stat==0)
            {
				ibt=0;
				bt=0;
                if (is_run)
                {
                    cw+=endtime-nt;
                    dur=endtime-nt;
                    nt=endtime;
                }
                else
                {
                    dur=0;
                }
            }
            else
            {
                if(stat==1)
                {
                    is_run=true;
					if(bt>r_num)
					{
						endtime=nt+r_num;
						dur=r_num;
					}
					else
					{
						endtime=nt+bt;
						dur=bt;
					}
					nt=endtime;
                }
                else
				{
                    is_run=false;
					if(bn==0 && rem!=0)
						iu+=-nt;
					if(rem!=0)
						bn++;
					if(rem!=0)
						nt=nt+bt;
				}
            }
        }
    }
}

void process::cstat()
{
    if (stat==0 && is_run)
    {
        cw+=endtime-nt;
        bt+=endtime-nt;
		dur=bt;
        nt=endtime;
		flag=false;
    }
    else
    {
		flag=true;
		if(stat==2)
		{
			bn--;
			if(bn==0)
				iu+=nt;
		}
        stat++;
        stat=stat%3;
        if (stat==0)
        {
            if (is_run)
            {
                cw+=endtime-nt;
                bt=endtime-nt;
                nt=endtime;
				dur=bt;
            }
            else
            {
                bt=0;
				dur=bt;
            }
        }
        else
        {
            if(stat==1)
            {
                is_run=true;
                endtime=nt+bt;
            }
            else
			{
                is_run=false;
				if(bn==0 && rem!=0)
					iu+=-nt;
				if(rem!=0)
					bn++;
			}
			if(rem!=0)
				nt=nt+bt;
        }
    }
}

void process::snext(process* p)
{
    next=p;
}

void process::set(int a, int b, int c, int d, int e)
{
    pid=a;
    at=b;
    tc=c;
    cb=d;
    ib=e;
    stat=-1;
    rem=tc;
	bt=0;
    nt=at;
    cw=0;
}

void process::snt(int n=0)
{
    if(n==0)
        nt=nt+bt;
    else
    {
        nt=nt+n;
        bt=bt-n;
    }
}

void process::crem(int r_num)
{
	if(r_num!=0 && r_num<bt)
		rem=rem-r_num;
	else
		rem=rem-bt;
}

void process::gib(int rand)
{
    bt = rand%ib+1;
	ibt = bt;
}

void process::gcb(int rand)
{
    bt = rand%cb+1;
	if (bt>rem)
		bt=rem;
}
