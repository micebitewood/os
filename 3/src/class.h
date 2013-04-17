#include<iostream>
using namespace std;

class Stats
{
public:
    int unmaps, maps, ins, outs, zeros;
    Stats()
    {
        unmaps=0;
        maps=0;
        ins=0;
        outs=0;
        zeros=0;
    }
};
