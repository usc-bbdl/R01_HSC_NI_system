#ifndef MM_HT_BIEWENER_H
#define MM_HT_BIEWENER_H
#include <sys/time.h>
#include <utility> //std::pair
#include <queue>

/*
 * Muscle Model from:
 * Accuracy of gastrocnemius muscles forces in walking
 * and running goats predicted by one-element Hill-type models.
 * Lee, Arnold, Miara, Biewener, et al.
 */

using namespace std;

namespace MM{
    class MM_HT_Biewener
    {
        public:
            MM_HT_Biewener();
            virtual ~MM_HT_Biewener();

            /*
             * force-length relationship
             * Eq 4 and 5. Active and pasive force
             */
            double forcelength_active(double length);
            double forcelength_pasive(double length);

            /*
             * Force-velocity relationship
             * Eq 6 and 7.
             */
            double forcevelocity(double vel, double v0, double k);

            /*
             * Muscle fiber force
             * Input: Activation a(t), active force F_a(l), force velocity F_v(vel)
             * Eq 3.
             */
            double fiber_force(double a, double fa, double fv);

            /*
             * Total active force:
             * Adding normalized forces for slow and fast fibers.
             * Eq 8.
             */
             double total_active_force(double Ff_slow, double Ff_fast);

            /*
             * Compute total muscle force Fm
             * Eq 2.
             */
            double total_muscle_force(double Ff, double Fp, double theta);

            /*
             * Activation function
             * Eq 1.
             */
            double activation(double EMGoff, double tau1, double tau2, double tau3, double beta1, double beta2, double beta3, double delta);

            /*
             * Simulation
             */
            double HT_MM_Sim(double EMG, double length);
            double HT_MM_Sim(double EMG, double length, double t);

            //Auxiliaries
            double tval_0;
            double tval_1;
            //Length
            double x_0, x_1;
            //Last time spick measured
            struct timeval t_1;
            //Actual spick measured
            struct timeval t_0;
            double act_time;

            //Activation auxiliaries
            double inta1, inta2, inta3;
            double a1_1, a1_0, a2_1, a2_0, a3_1, a3_0;
            double vel;

            queue<pair<double, double>> timeLineSlow;
            queue<pair<double, double>> timeLineFast;

            pair<double ,double> ts_0;
            pair<double ,double> tf_0;

            double actS;
            double actF;
            double eq8;
            double eq5;

    };
}
#endif // MM_HT_BIEWENER_H
