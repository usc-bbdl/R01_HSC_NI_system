#ifndef UTILITY_H
#define UTILITY_H
#include <utility> //std::pair
#include <iostream>
#include <math.h>

using namespace std;

namespace MM{
    class Utility
    {
        public:
            Utility(double exp_time_0);
            ~Utility();
            /* Generate a step-like x and its dereivative dx for a fixed L2.
             * These lists will be used to generate a force-length curve by plugging in them into shadmehr eq 3.
             * For passive force generation, L1 = rest_length = 1.0 (cm).
             * Abstract: Generates length of muscle values in t.
             */
            pair <double, double>* gen_waveform(double L1, double L2, double T, double sample_rate);

             /*
              * Generating spikes (auxiliary of  generating h(t))
              */
            int* spike_train(double T, double firing_rate, double sampling_rate);

            /*
             * Generate EMG signal
             */
             double* gen_emg(double T, double sampling_rate);

            /*
             * Generate length function
             */
            double* gen_length(double T, double sampling_rate);

            //Variables
            double exp_time;

    };
}
#endif // UTILITY_H
