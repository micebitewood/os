#include<iostream>
#include<fstream>
#include<string>
using namespace std;

int gln(string fname, int temp)//get-line-number, input:file name, tellg(). output: line number;
{
    int k;
    string s;
    ifstream fin;

    k=0;
    fin.open(fname.c_str());
	if (temp==-1)
	{
		fin.seekg(0, ios::end);
		temp=fin.tellg();
		fin.seekg(0, ios::beg);
	}
    while (fin.tellg()<temp && !fin.eof())
    {
        getline(fin, s, '\n');
        k++;
    }
	if (k==0)
		k=1;
	fin.close();
    return k;
}

int gon(string fname, int temp, int linenum)//get-offset-number, input: file number, tellg(), line number. output: offset of the line.
{
    int offsetnum, n;
    ifstream fin;
    string s;

    fin.open(fname.c_str());
	if (temp==-1)
	{
		fin.seekg(0, ios::end);
		temp=fin.tellg();
		fin.seekg(0, ios::beg);
	}
    while (linenum>1)
    {
        getline(fin, s);
        linenum--;
    }
    n=fin.tellg();
	offsetnum=n;
    while (fin.tellg()<temp && !fin.eof())
    {
		s="";
        fin >> s;
		if (s=="")
			break;
        offsetnum=fin.tellg();
    }
    offsetnum=offsetnum-s.length();
    offsetnum=offsetnum-n+1;
	fin.close();
    return offsetnum;
}

bool is_sym(string s)//if s is a valid symbol, return true, else return false.
{
    int k;
    char c;

    if (s[0]<'a' || s[0]>'z')
        if(s[0]<'A' || s[0]>'Z')
            return false;
    k=1;
    while (s[k]!=0)
    {
        c=s[k];
        if (c<'0' || c>'9')
            if (c<'a' || c>'z')
                if (c<'A' || c>'Z')
                    return false;
        k++;
    }
    return true;
}

bool is_num(string s)//if s is a valid number, return true, else return false.
{
    int k;
    
    k=0;
    while (s[k]!=0)
    {
        if (s[k]<'0' || s[k]>'9')
            return false;
		k++;
    }
    return true;
}





int main(int argc, char** argv)
{
	int i, n, j, temp, mo[256], moc[256], c, mc, k, m, linenum, offsetnum, l;
	//mo[256] the base addr of each def, 256 is the worst case
	//moc[256] is the module number for each def, 256 is the worst case
	//n is the number of defs
	string name[9472], stemp, fin_name, s, fout_name;
	//name[9472] includes defs, uses & codes, 9472 is the worst case in total
	int value[9472];
	//value[9472] includes the abs addr for defs, rela addr for uses, instruc for codes
	bool flag[256];
	//check def is used or not
	ifstream fin;

	fin_name=*(argv+1);
//Part1================================================================================================
	fin.open(fin_name.c_str());
	if (!fin.is_open())
    {
		cout << "Not a valid inputfile <" << fin_name << ">" << endl;
        return 0;
    }
	else
	{
		fin >> i;
        n=0;//count of defs, uses, codes in total
        c=0;//count of codes in total
		temp=0;//offset of input
		mc=1;//count of module
        if(fin.eof())
        {
            cout << "Parse Error line 1 offset 1: NUM_EXPECTED" << endl;
			fin.close();
            return 0;
        }
		while(!fin.eof())
		{
//deflist
			if (i>16)
			{
                temp=fin.tellg();
                linenum=gln(fin_name,temp);
                offsetnum=gon(fin_name,temp,linenum);
				cout << "Parse Error line " << linenum << " offset " << offsetnum << ": TO_MANY_DEF_IN_MODULE" << endl;
				fin.close();
				return 0;
			}
			for (j=0;j<i;j++)
			{
				temp=fin.tellg();
                name[n]="";
				fin >> name[n];
				if (is_sym(name[n]))//if name[n] is a valid symbol
				{
					if (name[n].length()>16)
					{
                        temp=fin.tellg();
                        linenum=gln(fin_name, temp);
                        offsetnum=gon(fin_name, temp, linenum);
						cout << "Parse Error line " << linenum << " offset " << offsetnum << ": SYM_TOLONG" << endl;
						fin.close();
						return 0;
					}
                }
                else//if name[n] is an invalid symbol
                {
                    temp=fin.tellg();
                    linenum=gln(fin_name, temp);
                    offsetnum=gon(fin_name, temp, linenum);
                    cout << "Parse Error line " << linenum << " offset " << offsetnum << ": SYM_EXPECTED" <<endl;
					fin.close();
					return 0;
                }
                temp=fin.tellg();
				fin >> stemp;
                if (is_num(stemp))//if value[n] is a number
                {
                    k=0;
                    value[n]=0;
                    while (stemp[k]!=0)
                    {
                        value[n]=value[n]*10;
                        value[n]=value[n]+stemp[k]-'0';
                        k++;
                    }
                    value[n]=value[n]+c;
                    mo[n]=c;
					moc[n]=mc;
                    n++;
                }
                else//if value[n] is not a number
                {
                    temp=fin.tellg();
                    linenum=gln(fin_name, temp);
                    offsetnum=gon(fin_name, temp, linenum);
                    cout << "Parse Error line " << linenum << " offset " << offsetnum << ": NUM_EXPECTED" <<endl;
					fin.close();
                    return 0;
               }
			}//end of deflist
//uselist
			fin >> i;
			if (i>16)
			{
                temp=fin.tellg();
                linenum=gln(fin_name, temp);
                offsetnum=gon(fin_name, temp, linenum);
                cout << "Parse Error line " << linenum << " offset " << offsetnum << ": TO_MANY_USE_IN_MODULE" <<endl;
				fin.close();
                return 0;
			}
			for (j=0;j<i;j++)
			{
				fin >> stemp;
				if(!is_sym(stemp))
				{
					temp=fin.tellg();
                    linenum=gln(fin_name, temp);
                    offsetnum=gon(fin_name, temp, linenum);
                    cout << "Parse Error line " << linenum << " offset " << offsetnum << ": SYM_EXPECTED" <<endl;
					fin.close();
                    return 0;
				}
			}//end of uselist
//codes
			fin >> i;
            c=c+i;
            if (c>512)
            {
                temp=fin.tellg();
                linenum=gln(fin_name, temp);
                offsetnum=gon(fin_name, temp, linenum);
                cout << "Parse Error line " << linenum << " offset " << offsetnum << ": TO_MANY_INSTR" <<endl;
				fin.close();
                return 0;
            }
			for (j=0;j<i;j++)
       		{
				stemp="";
				fin >> stemp;
				if (stemp!="A" && stemp!="I" && stemp!="R" && stemp!="E")
				{
                    temp=fin.tellg();
                    linenum=gln(fin_name, temp);
                    offsetnum=gon(fin_name, temp, linenum);
                    cout << "Parse Error line " << linenum << " offset " << offsetnum << ": ADDR_EXPECTED" <<endl;
					fin.close();
                    return 0;
				}
				fin >> stemp;//skip codes
				if (!is_num(stemp))
				{
					temp=fin.tellg();
                    linenum=gln(fin_name, temp);
                    offsetnum=gon(fin_name, temp, linenum);
                    cout << "Parse Error line " << linenum << " offset " << offsetnum << ": ADDR_EXPECTED" <<endl;
					fin.close();
                    return 0;
				}
			}
			fin >> i;
			mc++;
		}
		mo[n]=c;
//print of part1
        i=0;
        mc=1;
        while (i<n)
        {
            if (value[i]>c)
            {
                cout << "Warning: Module " << mc << ": " << name[i] << " to big " << value[i]-mo[i] << " (max=" << c-mo[i]-1 << ") assume zero relative" << endl;
                value[i]=mo[i];
				i++;
            }
			else
            {
				j=i+1;
				while(j<n)
				{
	                if (mo[i]<mo[j])
					{
	                    k=i;
						while (value[k]<mo[j] && k<j)
						{
	                        k++;
						}
						while (k<j)
						{
	                        cout << "Warning: Module " << mc << ": " << name[k] << " to big " << value[k]-mo[i] << " (max=" << mo[j]-mo[i]-1 << ") assume zero relative" << endl;
							value[k]=mo[i];
							k++;
						}
						mc++;
						break;
					}
					else
						j++;
				}
				i=j;
			}
        }
		i=0;
		m=0;
        cout << "Symbol Table" << endl;
        while(i<n)
        {
            flag[i]=true;
            for (k=0;k<m;k++)
            {
                if (name[k]==name[i])
                {
                    flag[k]=false;
                    flag[i]=false;
                    break;
                }
            }
            if (flag[i]==true)
            {
                name[m]=name[i];
                value[m]=value[i];
                flag[m]=true;
                m++;
            }
            i++;
        }//end of while(i<n)
		if (m>=256)
        {
            cout << "TO_MANY_DEF" << endl;
        }
		for(i=0;i<m;i++)
        {
            if (flag[i]==true)
                cout << name[i] << '=' << value[i] << endl;
            else
                cout << name[i] << '=' << value[i] << " Error: This variable is multiple times defined; first value used" << endl;
        }
	    fin.close();
	}//end of else "if (!fin.is_open())":Part 1
	//c:count of codes
	//m:number of defs
	//mo[n]:base addr of each module
//Part 2==================================================================================================
	fin.open(fin_name.c_str());
	for (i=0;i<n;i++)
    {
        flag[i]=false;//turns true if used
    }
	n=m;//count in total
    fin >> i;
    while (!fin.eof())
    {
	//defs
		for (j=0;j<i;j++)
		{
			fin >> stemp;
			fin >> temp;
		}
	//uses
		fin >> i;
		for (j=0;j<i;j++)
		{
			fin >> name[m];
			value[m]=-1;
			for (l=0;l<n;l++)
			{
				if (name[l]==name[m])
				{
					value[m]=value[l];
					flag[l]=true;
					break;
				}
			}
			m++;
		}
		name[m]="0";
		value[m]=-2;
		m++;
	//codes
		fin >> i;
		for (j=0;j<i;j++)
		{
			fin >> name[m];
			fin >> value[m];
			m++;
		}
		name[m]="1";
		value[m]=-3;
		m++;
		fin >> i;
	}
	//m: the total count including defs, uses, module-separate signs & codes
//print 2
    j=0;//count of codes
	k=n-1;//count in total
    stemp="00";
    mc=0;
    cout << endl << "Memory Map" << endl;
    for(i=n;i<m;i++)
    {
		if (j>9)
			stemp="0";
		else if(j>99)
			stemp="";
        if (name[i]=="1")//separate of different modules
        {
			k=i;//total lines untill this module
        }
        else if (name[i]=="A")
		{
			temp=value[i]/1000;
			temp=temp*1000;
			temp=value[i]-temp;
			cout << stemp << j << ": ";
			j++;
			if (temp>c)//if addr of 'A' is larger than the total memory
			{
				value[i]=value[i]-temp;
				if (value[i]<10)
					s="000";
				else if (value[i]<100)
					s="00";
				else if (value[i]<1000)
					s="0";
				else
					s="";
				cout << s << value[i];
				cout << " Error: Absolute address exceeds machine size; zero used" << endl;
			}
			else//if addr of 'A' is valid
			{
				if (value[i]<10)
					s="000";
				else if (value[i]<100)
					s="00";
				else if (value[i]<1000)
					s="0";
				else
					s="";
				cout << s << value[i] << endl;
			}
		}
		else if (name[i]=="R")
		{
			cout << stemp << j << ": ";
			j++;
			temp=value[i]/1000;
			temp=temp*1000;
			temp=value[i]-temp;
			l=k;
			while(value[l]!=-2)//find the base line of this module
				l++;
			mc=l+1;//find the base line of codes in this module
			while(value[l]!=-3)//find the base line of next module
				l++;
			if (temp>l-mc)//l-mc is the size of this module, if relative addr is larger than l-mc
			{
				value[i]=value[i]-temp+j-i+mc-1;
				if (value[i]<10)
					s="000";
				else if (value[i]<100)
					s="00";
				else if (value[i]<1000)
					s="0";
				else
					s="";
				cout << s << value[i];
				cout << " Error: Relative address exceeds module size; zero used" << endl;
			}
			else//if relative addr is valid
			{
				value[i]=value[i]+j-i+mc-1;
				if (value[i]<10)
					s="000";
				else if (value[i]<100)
					s="00";
				else if (value[i]<1000)
					s="0";
				else
					s="";
				cout << s << value[i] << endl;
			}
		}
		else if (name[i]=="I")
		{
			cout << stemp << j << ": ";
			if (value[i]<10)
				s="000";
			else if (value[i]<100)
				s="00";
			else if (value[i]<1000)
				s="0";
			else
				s="";
			cout << s << value[i] << endl;
			j++;
		}
		else if (name[i]=="E")
		{
			temp=value[i]/1000;
			temp=temp*1000;
			temp=value[i]-temp;
			temp=1+k+temp;
			l=k;
			cout << stemp << j << ": ";
			j++;
			while (value[l]!=-2)//find the last line of uses
			{
				l++;
			}
			if (temp>=l)
			{
				if (value[i]<10)
					s="000";
				else if (value[i]<100)
					s="00";
				else if (value[i]<1000)
					s="0";
				else
					s="";
				cout << s << value[i] << " Error: External address exceeds length of uselist; treated as immediate" << endl;
			}
			else
			{
				if (name[temp][0]!='0')//in order to mark if uselist is used
					name[temp]="0"+name[temp];
				if (value[temp]==-1)//uselist not defined
				{
					if (value[i]<10)
						s="000";
					else if (value[i]<100)
						s="00";
					else if (value[i]<1000)
						s="0";
					else
						s="";
					cout << s << value[i] << " Error: " << name[temp].substr(1) << " is not defined; zero used" << endl;
				}
				else//valid use
				{
					value[i]=value[i]-temp+1+k;
					value[i]=value[i]+value[temp];
					if (value[i]<10)
						s="000";
					else if (value[i]<100)
						s="00";
					else if (value[i]<1000)
						s="0";
					else
						s="";
					cout << s << value[i] << endl;
				}
			}
		}
    }//end of memory map
    mc=1;
    for (i=n;i<m;i++)
    {
        if (name[i]=="1")
            mc++;
        if (name[i][0]!='0' && name[i]!="1" && name[i]!="I" && name[i]!="R" && name[i]!="A" && name[i]!="E")
            cout << "Warning: In Module " << mc << " " << name[i] << " appeared in the uselist but was not actually used" << endl;
    }
	cout << endl;
    for (i=0;i<n;i++)
    {
        if (flag[i]==false)
            cout << "Warning: " << name[i] << " was defined in module " << moc[i] << " but never used"<< endl;
    }
	cout << endl;
	fin.close();
	return 0;
}
