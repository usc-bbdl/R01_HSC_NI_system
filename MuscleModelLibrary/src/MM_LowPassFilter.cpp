#include "..\include\MM_LowPassFilter.h"
#include <sys/time.h>
#include <stdio.h>
#include <iostream>

/*
 * LOWPASSFILTER MUSCLE MODEL
 * Gw^2/(s^2+2szw+w^2)
 */

namespace MM{
    //Constructor
    MM_LowPassFilter::MM_LowPassFilter()
    {

        //Inicializing values of x1(t) and x2(t)
        x1_0 = 0.0;
        x2_0 = 0.0;
        //x1_1 = 0.0;
        //x2_1 = 0.0;

        //Inicializing comulative value of integral x1 and x2
        int_x1 = 0.0;
        int_x2 = 0.0;
        int_input = 0.0;

        //Inicializing values of time
        gettimeofday(&t_1, NULL);
        gettimeofday(&t_0, NULL);

        //Actual time inicialized
        time = 0.0;
        tval_1 = 0.0;
        tval_0 = 0.0;

        //Auxiliary integral variables inicialized
        aux_int_input = 0.0;
        aux_int_x1 = 0.0;
        aux_int_x2 = 0.0;
    }

    //Distructor
    MM_LowPassFilter::~MM_LowPassFilter()
    {

    }

    /*
     *Sim function using computer clock for timing
     */
    double MM_LowPassFilter::LPF_MM_Sim(double spike, double omega, double zeta, double G)
    {
        //Updating time
        t_1 = t_0;
        gettimeofday(&t_0, NULL);
        double delta_sec = (t_0.tv_sec - t_1.tv_sec) + 0.000001 * (t_0.tv_usec - t_1.tv_usec);
        time += delta_sec;
        //printf("time (sec) =%8.4f\n", time);
        /*
        * Integral:
        * x_1 = Int{x_2(t)}
        * x_2 = Int{-w^2*x_1(t)-2*z*w*x_2(t)+G*w^2*I(t)}
        */
        double T = 0;

        int_input = int_input + (spike * delta_sec);
        aux_int_input = int_input * G * (omega * omega);

        int_x1 = int_x1 + (x1_0 * delta_sec);
        aux_int_x1 = int_x1 * (omega * omega);

        int_x2 = int_x2 + (x2_0 * delta_sec);
        aux_int_x2 = int_x2 * (2 * zeta * omega);

        T = x1_0;
        x1_0 = x1_0 + (x2_0 * delta_sec);
        x2_0 = aux_int_input - aux_int_x1 - aux_int_x2;

        return T;
    }

    /*
     * Function with time added as input.
     */
    double MM_LowPassFilter::LPF_MM_Sim(double spike, double omega, double zeta, double G, double t)
    {
        //Updating time
        tval_1 = tval_0;
        tval_0 = t;
        double delta_sec = tval_0 - tval_1;
        time += delta_sec;

        /*
        * Integral:
        * x_1 = Int{x_2(t)}
        * x_2 = Int{-w^2*x_1(t)-2*z*w*x_2(t)+G*w^2*I(t)}
        */
        double T = 0;

        /*x1_0 = x1_1;
        x2_0 = x2_1;

        int_x2 = int_x2 + (((G*omega*omega*spike)-(omega*omega*x1_0)-(2*zeta*omega*x2_0))*delta_sec);
        x2_1 = int_x2;
        int_x1 = int_x1 + x2_0*delta_sec;
        x1_1 = int_x1;
        T = int_x1;*/

        int_input = int_input + (spike * delta_sec);
        aux_int_input = int_input * G * (omega * omega);

        int_x1 = int_x1 + (x1_0 * delta_sec);
        aux_int_x1 = int_x1 * (omega * omega);

        int_x2 = int_x2 + (x2_0 * delta_sec);
        aux_int_x2 = int_x2 * (2 * zeta * omega);

        T = x1_0;
        x1_0 = x1_0 + (x2_0 * delta_sec);
        x2_0 = aux_int_input - aux_int_x1 - aux_int_x2;

        return T;
    }

}
