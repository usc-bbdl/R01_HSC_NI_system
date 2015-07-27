#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "include\MM_HT_Biewener.h"
#include "include\Utility.h"

using namespace std;
using namespace MM;

int main3()
{

	cout <<"time,egm,lenght"<<endl;
    double time_ti = 0.0;
    Utility util(2.0);
    double* arr = util.gen_emg(10,1024);
    double* i = arr;

    double* lengs = util.gen_length(5,1024);
    double* j = lengs;

    while ((j-lengs) != (2 * 1024)){
        cout << time_ti <<","<<*i<<","<<*j << endl;
        ++j;
        ++i;
        time_ti = time_ti + (1.0/1024.0);
    }

    return 0;
}

