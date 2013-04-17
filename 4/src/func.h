#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<cmath>
#include"c.h"
using namespace std;

string calc(IOOperation* io, int n)
{
    string s;
    int tt=0, tm=0, mw=0;
    double at=0, aw=0;
    stringstream ss;

    for(int i=0;i<n;i++)
    {
        if(io[i].get()>tt)
            tt=io[i].get();
        tm+=io[i].get()-io[i].gbt();
        at+=io[i].get()-io[i].gat();
        aw+=io[i].gbt()-io[i].gat();
        if((io[i].gbt()-io[i].gat())>mw)
            mw=io[i].gbt()-io[i].gat();
    }
    tt+=1;
    at=at*1.0/n;
    aw=aw*1.0/n;
    ss << tt << ' ' << tm << ' ' << setiosflags(ios::fixed) << setprecision(2) << at << ' ' << setiosflags(ios::fixed) << setprecision(2) <<  aw << ' ' << mw;
    getline(ss, s);
    return s;
}

int f(IOOperation* io, int n)
{
    bool flag=false;

    int next_time=io[0].cur_time;
    int next_i=0;
    for(int i=0;i<n;i++)
    {
        if(io[i].cs() && next_time>io[i].gat())
        {
            next_time=io[i].gat();
            next_i=i;
            flag=true;
        }
    }
    if(flag)
        return next_i;
    else
        return -1;
}

int s(IOOperation* io, int n)
{
    bool flag=false;

    int next_track=512;
    int next_i=0;
    for(int i=0;i<n;i++)
    {
        if(io[i].cs() && next_track>abs(io[i].cur_track-io[i].gt()))
        {
            next_track=abs(io[i].cur_track-io[i].gt());
            next_i=i;
            flag=true;
        }
    }
    if(flag)
        return next_i;
    else
        return -1;
}

int S(IOOperation* io, int n, bool& direction)
{
    bool flag=false;
    int i;

    int next_track;
    int next_i=0;
    if(direction)//if increasing
    {
        next_track=512;
        for(i=0;i<n;i++)
            if(io[i].cs() && io[i].gt()>=io[i].cur_track && next_track>io[i].gt())
            {
                next_track=io[i].gt();
                next_i=i;
                flag=true;
            }
        if(!flag)
        {
            direction=false;
            next_track=0;
            for(i=0;i<n;i++)
                if(io[i].cs() && io[i].gt()<io[i].cur_track && next_track<io[i].gt())
                {
                    next_track=io[i].gt();
                    next_i=i;
                    flag=true;
                }
        }
    }
    else
    {
        next_track=0;
        for(i=0;i<n;i++)
            if(io[i].cs() && io[i].gt()<=io[i].cur_track && next_track<io[i].gt())
            {
                next_track=io[i].gt();
                next_i=i;
                flag=true;
            }
        if(!flag)
        {
            direction=true;
            next_track=512;
            for(i=0;i<n;i++)
                if(io[i].cs() && io[i].gt()>io[i].cur_track && next_track>io[i].gt())
                {
                    next_track=io[i].gt();
                    next_i=i;
                    flag=true;
                }
        }
    }
    if(flag)
        return next_i;
    else
        return -1;
}

int C(IOOperation* io, int n)
{
    bool flag=false;

    int next_track=512;
    int next_i=0;
    for(int i=0;i<n;i++)
        if(io[i].cs() && io[i].gt()>=io[i].cur_track && next_track>io[i].gt())
        {
            next_track=io[i].gt();
            next_i=i;
            flag=true;
        }
    if(!flag)
    {
        for(int i=0;i<n;i++)
            if(io[i].cs() && io[i].gt()<io[i].cur_track && next_track>io[i].gt())
            {
                next_track=io[i].gt();
                next_i=i;
                flag=true;
            }
    }
    if(flag)
        return next_i;
    else
        return -1;
}

int F(IOOperation* io, int n, bool& direction)
{
    bool flag=false;
    int i;

    int next_track=512;
    int next_i=0;
    if(direction)//increasing
    {
        for(i=0;i<n;i++)
            if(io[i].cs() && !io[i].csc() && io[i].gt()>=io[i].cur_track && next_track>io[i].gt())
            {
                next_track=io[i].gt();
                next_i=i;
                flag=true;
            }
        if(!flag)
        {
            direction=false;
        }
    }
    if(!direction)
    {
        next_track=0;
        for(i=0;i<n;i++)
            if(io[i].cs() && !io[i].csc() && io[i].gt()<io[i].cur_track && next_track<io[i].gt())
            {
                next_track=io[i].gt();
                next_i=i;
                flag=true;
            }
        if(!flag)
        {
            for(i=0;i<n;i++)
                if(io[i].cs() && io[i].csc())
                {
                    direction=true;
                    io[i].ssc();
                }
            if(direction)
                return F(io, n, direction);
            else
                return -1;
        }
    }
    return next_i;
}

void sched(IOOperation* io, int n, int alg, string outfile, bool verbose)
{
    ofstream fout;
    bool direction=true;//increasing

    fout.open(outfile.c_str());
    int next_time=io[0].gat();
    int next_i=0;
    for(int j=1;j<n;j++)
        if(next_time>io[j].gat())
        {
            next_time=io[j].gat();
            next_i=j;
        }
    io[next_i].ss();//set stat
	io[next_i].ssc();
    io[next_i].cur_time=next_time;
    if(verbose)
    {
        fout << "TRACE" << endl;
        fout << io[next_i].add() << endl;
    }
    for(int j=0;j<n;j++)
        if(j!=next_i && io[j].gat()==next_time)
        {
			io[j].ssc();
            io[j].ss();
            if(verbose)
                fout << io[j].add() << endl;
        }
    //begin scheduling
    while(next_i!=-1)
    {
        if(verbose)
            fout << io[next_i].issue() << endl;
        for(int j=0;j<n;j++)
        {
            if(!io[j].gs() && io[j].gat()<=io[j].cur_time)
            {
                io[j].ss();
                if(verbose)
                    fout << io[j].add() << endl;
            }
        }
        if(verbose)
            fout << io[next_i].finish() << endl;
        switch(alg)
        {
        case 1:
            next_i=f(io, n);
            break;
        case 2:
            next_i=s(io, n);
            break;
        case 4:
            next_i=S(io, n, direction);
            break;
        case 8:
            next_i=C(io, n);
            break;
        case 16:
            next_i=F(io, n, direction);
            break;
        }
        if(next_i==-1)
        {
            int min=io[0].cur_time;
            for(int j=0;j<n;j++)
                if(io[j].get()==0 && min<io[j].gat())
                {
                    min=io[j].gat();
                    next_i=j;
                }
            //find the max arr_time
            if(next_i!=-1)
            {
                for(int j=0;j<n;j++)
                    if(io[j].get()==0 && min>io[j].gat())
                    {
                        min=io[j].gat();
                        next_i=j;
                    }
                //find the min arr_time
                io[next_i].ss();
                io[next_i].cur_time=io[next_i].gat();
                if(verbose)
                    fout << io[next_i].add() << endl;
            }
        }
    }
    if(verbose)
    {
        fout << "IOREQS INFO" << endl;
        for(int i=0;i<n;i++)
            fout << io[i].sum() << endl;
    }
    fout << "SUM: " << calc(io, n) << endl;
}
