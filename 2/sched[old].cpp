#include<iostream>
#include"c.h"
#include<fstream>
#include<string>
#include<iomanip>
using namespace std;

extern bool is_run;
extern int endtime;
extern int bn;
extern double iu;

process* swap(process* a, process* b)
{
    process* t;
	process* temp;

	t=a->getnext();
    if(b->getnext()==NULL)
        a->snext(NULL);
    else
        a->snext(b->getnext());
    b->snext(a);
	return t;
}

int main(int argc, char** argv)
{
    int n, i, j, r_num, flag, a, b, c, d, rand, randnum, temp, ttemp;
    string s, r_sym;
    process* head,* ptemp,* pptemp;
    ifstream fin, randfile;
    ofstream fout;
	//printout related below
	int ftmax;
	double cu, att, acw, tp;
	//ftmax:finishing time of the last event
	//cu:CPU utilization
	//att:average turnaround time among processes
	//acw:average cpu waiting time among processes
	//tp:throughput of number processes per 100 time units

    n=1;
    s=*(argv+n);
    flag=0;
    n++;
    if(s=="-v")
    {
        flag=10;
        s=*(argv+n);
        n++;
    }
    if(s[2]=='L')
    {
        flag+=2;
    }
    else if(s[2]=='S')
    {
        flag+=3;
    }
    else if(s[2]=='R')
    {
        flag+=4;
        r_sym=s.substr(3);
        i=0;;
        r_num=0;
        while(r_sym[i]!=0)
        {
            r_num=r_num*10;
            r_num=r_num+r_sym[i]-'0';
            i++;
        }
    }
	else
    {
        flag+=1;
    }
    s=*(argv+n);
    fin.open(s.c_str());
    n++;
    s=s.substr(5);
    s="output"+s;
    if(flag%10==1)
        s=s+"_F";
    else if(flag%10==2)
        s=s+"_L";
    else if(flag%10==3)
        s=s+"_S";
    else if(flag%10==4)
        s=s+"_R"+r_sym;
    if(flag>9)
        s=s+"_t";
    fout.open(s.c_str());
    s=*(argv+n);//n is useless now
    randfile.open(s.c_str());//s is useless now
    randfile >> randnum;
    n=0;
    while(!fin.eof())
    {
        getline(fin, s);
        n++;
    }//s useless
	n--;
    fin.clear();
    fin.seekg(0, ios::beg);
    process* p=new process[n];
    i=0;
    for(j=0;j<n;j++)
    {
        fin >> a >> b >> c >> d;
        (p+i)->set(i, a, b, c, d);
        if(j>0)
            (p+i-1)->snext(p+i);
        i++;
    }
	(p+i-1)->snext(NULL);
    head=p;
	if(flag>9)
	{
		while(head!=NULL)
		{
			if(head->getstat()==-1)//if new
            {
                fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt()-head->getbt() << " READY  dur=" << head->getbt() << endl;
				fout << "T(" << head->getpid() << ":" << head->getnt() << "): READY -> READY" << endl;
            }
            else if(head->getstat()==0)//if ready
            {
                if (!is_run)
                {
                    if(flag%10==4 && head->getbt()!=0)
                    {
						fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt() << " RUNNG  dur=" << head->getdur() << endl;
						fout << "T(" << head->getpid() << ":" << head->getnt() << "): READY -> RUNNG  cb=";
					}
                    else
                    {
                        fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt()-head->getbt() << " RUNNG  dur=" << head->getbt() << endl;
						fout << "T(" << head->getpid() << ":" << head->getnt() << "): READY -> RUNNG  cb=";
						randfile >> rand;
                        randnum--;
                        head->gcb(rand);
                    }
                    fout << head->getbt() << " rem=" << head->getrem() << endl;
                }
            }
            else if(head->getstat()==1)//if run
            {
				if (flag%10==4)
				{
					head->crem(r_num);
					fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt()-head->getdur();
				}
				else
				{
					head->crem(0);
					fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt()-head->getbt();
				}
                if (flag%10==4 && head->getbt()>r_num)//Preempt
				{
						fout << " PREEMPT  dur=" << head->getdur() << endl << "T(" << head->getpid() << ":" << head->getnt() << "): RUNNG -> READY  cb=" << head->getbt()-r_num << " rem=" << head->getrem() << endl;
				}
                else if (head->getrem()!=0)
                {
					randfile >> rand;
					randnum--;
                    fout << " BLOCK  dur=" << head->getbt() << endl << "T(" << head->getpid() << ":" << head->getnt() << "): RUNNG -> BLOCK  ib=";
                    head->gib(rand);
                    fout << head->getbt() << " rem=" << head->getrem() << endl;
                }
                else//termination
				{
                    fout << " BLOCK  dur=" << head->getbt() << endl << "==> T(" << head->getpid() << "): Done" << endl;
				}
            }
            else if(head->getstat()==2)//if block
            {
                fout << endl << "==> " << head->getnt() << " " << head->getpid() << " ts=" << head->getnt()-head->getbt() << " READY  dur=" << head->getbt() << endl << "T(" << head->getpid() << ":" << head->getnt() << "): BLOCK -> READY" << endl;
            }
			if(flag%10==4)
				head->rstat(r_num);
			else
	            head->cstat();
			if(head->getstat()==2 && head->getrem()==0)//termination
			{
	            head=head->getnext();
			}
			else//scheduling
			{
	            ptemp=head;
				temp=ptemp->getnt();
				while(ptemp->getnext()!=NULL && temp>ptemp->getnextnt())
					ptemp=ptemp->getnext();
				if(ptemp->getnext()!=NULL && temp==ptemp->getnextnt())
				{
	                while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ptemp->getnextstat()==-1)//creation first
						ptemp=ptemp->getnext();
					while (ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && head->getstat()<ptemp->getnextstat())//block>run>ready
						ptemp=ptemp->getnext();
					if (flag%10==1)//F
					{
						ttemp=head->getbt();
						while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ttemp<=ptemp->getnextbt())
	                        ptemp=ptemp->getnext();
					}
					else if(flag%10==2)//L
					{
						ttemp=head->getbt();
						if(head->getstat()==0 && head->getflag()==true)
							while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ttemp>ptemp->getnextbt())
								ptemp=ptemp->getnext();
						else if (head->getstat()==0 && head->getflag()==false)
							while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ttemp>=ptemp->getnextbt())
								ptemp=ptemp->getnext();
						else
							while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ttemp<=ptemp->getnextbt())
								ptemp=ptemp->getnext();
					}
					else if(flag%10==3)//S
					{
	                    ttemp=head->getbt();
						if (head->getstat()==0)
							while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && head->getrem()>=ptemp->getnextrem())
								ptemp=ptemp->getnext();
						else
							while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && ttemp<=ptemp->getnextbt())
								ptemp=ptemp->getnext();
					}
					else if(flag%10==4)//R
					{
						while(ptemp->getnext()!=NULL && temp==ptemp->getnextnt() && head->getstat()<=ptemp->getnextstat())
	                        ptemp=ptemp->getnext();
					}
				}
				if (head!=ptemp)
					head=swap(head, ptemp);
			}
		}
	}
	else
	{
		if(flag%10==4)
        {}
        else if(head->getstat()==0 && is_run==false)
        {
            randfile >> rand;
            randnum--;
            head->gcb(rand);
        }
        else if(head->getstat()==1)
        {
            randfile >> rand;
            randnum--;
            head->crem(0);
            head->gib(rand);
        }
	}
	ftmax=0;
	cu=0;
	att=0;
	acw=0;
	if(flag%10==1)
		fout << "FCFS" << endl;
	else if(flag%10==2)
		fout << "LCFS" << endl;
	else if(flag%10==3)
		fout << "SJF" << endl;
	else
		fout << "RR " << r_num << endl;
	i=0;
	while(i<n)
	{
		if(i<10)
			s="000";
		else if(i<100)
			s="00";
		else if(i<1000)
			s="0";
		else
			s="";
		if (ftmax<(p+i)->getnt())
			ftmax=(p+i)->getnt();
		cu+=(p+i)->gettc();
		att+=(p+i)->getnt()-(p+i)->getat();
		acw+=(p+i)->getcw();
		fout << s << i << ":" << setw(5) << (p+i)->getat() << setw(5) << (p+i)->gettc() << setw(5) << (p+i)->getcb() << setw(5) << (p+i)->getib() << " |";
		fout << setw(5) << (p+i)->getnt() << setw(5) << (p+i)->getnt()-(p+i)->getat() << setw(5) << (p+i)->getnt() - (p+i)->getat() - (p+i)->gettc() - (p+i)->getcw() <<  setw(5) << (p+i)->getcw() << endl;
		i++;
	}
	cu=cu*100.0/ftmax;
	iu=iu*100.0/ftmax;
	att=att/i;
	acw=acw/i;
	tp=i*100.0/ftmax;
	fout << "SUM: " << ftmax << " " << fixed << setprecision(2) << cu << " " << fixed << setprecision(2) << iu << " " << fixed << setprecision(2) << att << " " << fixed << setprecision(2) << acw << " " << fixed << setprecision(3) << tp << endl;
	fin.close();
	fout.close();
	randfile.close();
    delete[] p;
    return 0;
}
