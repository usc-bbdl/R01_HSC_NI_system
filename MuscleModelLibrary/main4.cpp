#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include "include\MM_LowPassFilter.h"
#include "include\MM_HillType.h"
#include "include\Utility.h"

using namespace std;
using namespace MM;

int main4()
{
    MM_LowPassFilter mm;

    FILE *f;
    float time;
    float input;
    //float output;
    //float length;
    //float vel;
    //float spike;
    double d_input;

    f = fopen("spiketrain.txt","r");

	if(f != NULL) {
		for(int i = 0; i < 75000; i++) {
			fscanf(f, "%f,%f\n", &time, &input);
			d_input = static_cast<double>(input);
			double d_time = static_cast<double>(time);

			/*Calling Low-pass filter function
            * mm.LPF_MM_Sim(spike, omega, zeta, Gain, time)
            */
            float T = mm.LPF_MM_Sim(d_input,5.0,1.0,63.0, d_time);
            cout << mm.time << "," << (T * 30 + 1) << endl;

		}
		fclose(f);
	}

    return 0;
}

