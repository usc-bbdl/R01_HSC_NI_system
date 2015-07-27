#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "include\MM_HT_Biewener.h"

using namespace std;
using namespace MM;

int main2()
{
    FILE *f;
    float time;
    float length;
    float emg;
    MM_HT_Biewener bie;
    f = fopen("data.csv","r");
    //cout << "time,T"<<endl;
	if(f != NULL) {
		for(int i = 0; i < 60000; i++) {
			fscanf(f, "%f,%f,%f\n", &time, &emg, &length);
			double d_emg = static_cast<double>(emg);
			double d_length = static_cast<double>(length);
			double d_time = static_cast<double>(time);
			double T = bie.HT_MM_Sim(d_emg*10.0,d_length, d_time);
            cout << d_time << ","  << T << endl;
		}
		fclose(f);
	}

    return 0;
}
