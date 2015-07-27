#ifndef MM_HILLTYPE_H
#define MM_HILLTYPE_H

#include <sys/time.h>
#include <utility> //std::pair

/*
 * Hill-Type Muscle Model
 * Generate the tension curve in Eq.3 of Shadmehr and S.P.Wise Eric W. Sohn
 */

using namespace std;

namespace MM{
    class MM_HillType
    {
        public:
            MM_HillType();
            ~MM_HillType();

            //Actual time in experiment
            double act_time;
            //Duration of the experiment
            double exp_time;

            //Sampling and timing auxiliaries
            double tval_0;
            double tval_1;
            double T; //Period
            double sampling_rate;

            //Last time spick measured
            struct timeval t_1;
            //actual spick measured
            struct timeval t_0;

            //HT_MM_Sim auxiliaries
            double spike_i1, spike_i2;
            double h_i1, h_i2;
            double T_i, T_i1;

            //Sim function using computer clock for the timing
            double HT_MM_Sim(double spike,double x, double d_x);

            //Sim function with time in double
            double HT_MM_Sim(double spike, double x, double d_x, double t);

            /*
             * s(x) function
             * Scales the active force produced in Eq. (5) in Shadmehr
             * (Active force component)
             */
            double s(double x);

            /*
             * Eq. (3) in Shedmehr, derivative of Tension in the muscle.
             * lce : length of muscle, x
             * vel : slope of x.
             */
            double d_force(double T_0, double lce, double vel, double A);

            /*
             * Function for creating h(t)
             */

            double gen_h_diff_eq (double spike);

            /*
             * Generate active state component in the muscle stretch.
             * gen_h_diff_eq(.) is the function that needs this
             * x_i1: spike [n-1]
             * x_i2: spike [n-2]
             * y_i1: A[n-1]  (prev active state #)
             * y_i2: A[n-2]  (prev prev active state #)
             * Transfer function:
             * 2299 z^2 - 2364 z
             * ----------------------
             * z^2 - 1.944 z + 0.9445
             */
            double active_state(double x_i0, double x_i1, double x_i2, double y_i1, double y_i2);

            double active_state_fuglevand(double x_i1, double x_i2, double y_i1, double y_i2);


    };
}

#endif // MM_HILLTYPE_H
