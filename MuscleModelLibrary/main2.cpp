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

int main()
{
    MM_LowPassFilter mm;

    FILE *f;
    float time;
    //float input;
    //float output;
    float length;
    float vel;
    float spike;
    double d_input;

    //f = fopen("spiketrain.txt","r");
    //cout << "Test_time, my_time, input, error, test_output, real_output"<<endl;
    //cout << "Test_time, input, T"<<endl;
	//if(f != NULL) {
	//	for(int i = 0; i < 2; i++) {
			//fscanf(f, "%f,%f,%f\n", &time, &input, &output);
			//d_input = static_cast<double>(input);
			//double d_time = static_cast<double>(time);
			//cout << d_input << endl;
			/*Calling Low-pass filter function
            * mm.LPF_MM_Sim(spike, omega, zeta, Gain, time)
            */
            //float T = mm.LPF_MM_Sim(d_input,5.0,1.0,63.0, d_time);
            //cout << mm.time << "," <<time << "," << input<<"," << (output - T) << "," << output << ","<< T << endl;

            /*Calling Low-pass filter function
            * mm.LPF_MM_Sim(spike, omega, zeta, Gain)
            */
            //float T = mm.LPF_MM_Sim(d_input,5.0,1.0,63.0);
            //cout << mm.time << "," << input<<"," << T << endl;
            //usleep(1000);

            //Validation test
            /*fscanf(f, "%f,%f\n", &time, &input);
			d_input = static_cast<double>(input);
			double d_time = static_cast<double>(time);
			float T = mm.LPF_MM_Sim(d_input,5.0,1.0,63.0, d_time);
            cout << mm.time << "," << input<<"," << T << endl;*/


		//}
		//fclose(f);
	//}

    /*cout <<"time,input,lenght,vel"<<endl;
    double time_ti = 0.0;
    Utility util(1.0);
    int* arr = util.spike_train(2,5,1024);
    int* i = arr;

    pair <double,double>* lengths = util.gen_waveform(1.0, 2.0, 1.0, 1024.0);
    pair <double, double>* j = lengths;

    while ((j-lengths) != 1024){
        cout << time_ti <<","<<*i<<","<<j->first << "," << j->second << endl;
        ++j;
        ++i;
        time_ti = time_ti + (1.0/1024.0);
    }*/

    f = fopen("file.txt","r");
    cout << "time,spike,length,vel,T"<<endl;
	if(f != NULL) {
		for(int i = 0; i < 1024; i++) {
			fscanf(f, "%f,%f,%f,%f\n", &time, &spike, &length, &vel);
			double d_spike = static_cast<double>(spike);
			double d_length = static_cast<double>(length);
			double d_vel = static_cast<double>(vel);
			double d_time = static_cast<double>(time);

			MM_HillType hill;
			double T = hill.HT_MM_Sim(d_spike,d_length, d_vel, d_time);
            cout << d_time << "," << d_spike << "," << d_length<<"," << d_vel << "," << T << endl;

		}
		fclose(f);
	}

    return 0;
}
