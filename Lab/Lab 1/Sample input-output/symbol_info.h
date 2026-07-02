#include<bits/stdc++.h>
using namespace std;

class symbol_info
{
private:
    string symbol_name;
    string symbol_type;

public:

    symbol_info(string n, string t)
    {
        symbol_name = n;
        symbol_type = t;
    }

    string getnameofsymbol()
    {
        return symbol_name;
    }

    string gettypeofsymbol()
    {
        return symbol_type;
    }

};