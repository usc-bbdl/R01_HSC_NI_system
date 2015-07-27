#ifndef MM_LOWPASSFILTER_H
#define MM_LOWPASSFILTER_H

#include <sys/time.h>

/*
 * LOWPASSFILTER MUSCLE MODEL
 *
 */

namespace MM{
    class MM_LowPassFilter
    {
        public:
            //Constructor
            MM_LowPassFilter();

            //Destructor
            ~MM_LowPassFilter();

            //Sim function using computer clock for the timing
            double LPF_MM_Sim(double spike, double omega, double zeta, double G);

            //Sim function with time in double
            double LPF_MM_Sim(double spike, double omega, double zeta, double G, double t);

            //Actual time in experiment
            double time;
            double tval_0;
            double tval_1;

        protected:

            //Actual values of x1 and x2
            double x1_0;
            double x2_0;
            //double x1_1;
            //double x2_1;

            //Integral comulative value for integral of x1 and integral x2
            double int_input;
            double int_x1;
            double int_x2;

            //Last time spick measured
            struct timeval t_1;
            //actual spick measured
            struct timeval t_0;

            //Auxiliary integral variables
            double aux_int_input;
            double aux_int_x1;
            double aux_int_x2;

    };
}

#endif // MM_LOWPASSFILTER_H
