#include<iostream>
#include<string>
#include<sstream>
#include<iomanip>
using namespace std;

class IOOperation
{
    private:
        bool stat;//added or not
        int arr_time;
        int track;
        int op_num;
        int beg_time;
        int end_time;
        bool second;//second queue, for FSCAN
    public:
        IOOperation* next;
        static int cur_time;
        static int cur_track;
        void set(int, int, int);
        int gat(){return arr_time;}//get arr_time
        int gon(){return op_num;}//get op_num
        int gt(){return track;}//get track
        int gbt(){return beg_time;}
        int get(){return end_time;}//get end_time
        bool gs(){return stat;}//get stat
        void ss(){stat=true;}//set stat
        void ssc(){second=false;}//set second
        void set_et(int et){end_time=et;}//set end_time
        void set_bt(int bt){beg_time=bt;}
        string add();//print add
        string issue();//print issue
        string finish();//print finish
        string sum();//print summary lines
        bool cs();//check status
        bool csc();//check second
};

bool IOOperation::csc()
{
    if(second)
        return true;
    return false;
}
bool IOOperation::cs()
{
    if(stat && end_time==0)
        return true;
    return false;
}

string IOOperation::sum()
{
    stringstream ss;
    string s;

    ss << setw(5) << op_num << ':' << setw(6) << arr_time << setw(6) << beg_time << setw(6) << end_time;
    getline(ss, s);
    return s;
}

string IOOperation::finish()
{
    stringstream ss;
    string s;

    end_time=cur_time;
    ss << cur_time << ':' << setw(6) << op_num << " finish " << end_time-arr_time;
    getline(ss, s);
    return s;
}

string IOOperation::issue()
{
    stringstream ss;
    string s;

    ss << cur_time << ':' << setw(6) << op_num << " issue " << track << ' ' << cur_track;
    beg_time=cur_time;
    cur_time=abs(track-cur_track)+cur_time;
    cur_track=track;
    getline(ss, s);
    return s;
}

string IOOperation::add()
{
    stringstream ss;
    string s;

    ss << arr_time << ':' << setw(6) << op_num << " add " << track;
    getline(ss, s);
    return s;
}

void IOOperation::set(int i, int a, int b)
{
    stat=false;//not in the waiting list
    op_num=i;
    arr_time=a;
    track=b;
    beg_time=0;
    end_time=0;
    second=true;
}

int IOOperation::cur_time=1;
int IOOperation::cur_track=0;
