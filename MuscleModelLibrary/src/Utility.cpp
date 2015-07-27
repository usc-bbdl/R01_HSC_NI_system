#include "..\include\Utility.h"

namespace MM{
Utility::Utility(double exp_time_0)
    {
        exp_time = exp_time_0;
    }

    Utility::~Utility()
    {

    }

    pair <double, double>* Utility::gen_waveform(double L1, double L2,double T, double sample_rate){
        const int max_n = T * sample_rate * exp_time;
        pair<double, double>* lengths = new pair<double, double>[max_n];
        double dt;
        double ramp_speed;
        double rising_range;

        double x_0 = 0.0;
        double x_1 = 0.0;
        double dx_0 = 0.0;

        dt = 1.0 / sample_rate;
        ramp_speed = 20.0;
        rising_range = (L2 - L1) / ramp_speed / dt;

        for(int i = 0; i < max_n; i++){
            x_1 = x_0;
            if(i <= rising_range){
                x_0 = L1 + i*(L2 - L1) / rising_range;
                dx_0 = (x_0 - x_1) / dt;
                lengths[i].first = x_0;
                lengths[i].second = dx_0;
            }
            else if(i >= max_n - rising_range){
                x_0 = L1 + (max_n - i)* (L2 -L1)/rising_range;
                dx_0 = (x_0 - x_1) / dt;
                lengths[i].first = x_0;
                lengths[i].second = dx_0;
            }else{
                x_0 = L2;
                dx_0 = (x_0 - x_1) / dt;
                lengths[i].first = x_0;
                lengths[i].second = dx_0;
            }
        }

        return lengths;
    }

    //Llamar esta funcion cada vez que se requiera
    int* Utility::spike_train(double T, double firing_rate, double sampling_rate){
        const int max_n = T * sampling_rate * exp_time;
        static int* dtrain = new int[max_n];

        for(int i = 0; i < max_n; i++){
            if((i - 1) % int(1.0 / firing_rate*sampling_rate) == 0){
                dtrain[i] = 1;
            }
            else{
                dtrain[i] = 0;
            }
        }
        return dtrain;
    }

    double* Utility::gen_emg(double T, double sampling_rate){
        const int max_n = sampling_rate * exp_time;
        static double* res = new double[max_n];
        double step = exp_time / max_n;
        for(int i = 0; i < max_n ; i++){
            double aux = sin(i * step * T) * 0.5 ;
            if (aux < 0){
                res[i] = 0.0;
            }else{
                res[i] = aux;
            }
        }
        return res;
    }

    double* Utility::gen_length(double T, double sampling_rate){
        const int max_n = sampling_rate * exp_time;
        static double* res = new double[max_n];
        double step = exp_time / max_n;
        for(int i = 0; i < max_n ; i++){
            double aux = (sin((i * step * T) - 1.0) * 2.5) + 1;
            if (aux < 1){
                res[i] = 1.0;
            }else if (aux > 2){
                res[i] = 2.0;
            }else{
                res[i] = aux;
            }
        }
        return res;
    }
}
