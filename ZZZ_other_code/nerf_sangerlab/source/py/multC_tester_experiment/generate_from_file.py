import numpy as np
import os.path as path
from pylab import *

def gen():
    # input_path = "C:/nerf_sangerlab/source/py/multC_tester_experiment/1125_resampled/"
    # conditions = np.loadtxt('conditions.txt')
    # gamma_dyn = int(conditions[0])
    # gamma_sta = int(conditions[1])
    # rep = int(conditions[2])
    # found = False
    # for i in range(gamma_dyn, 220, 20):
        # for j in range(gamma_sta, 220, 20):
            # for k in range(rep,20):
                # file_name = "expt_rampnhold.gd_" + str(i) + ".gs_" + str(j) + ".rep_" + str(k) + ".dev_fpga_resampled.txt"
                # if path.exists(input_path + file_name):
                    # x = np.loadtxt(input_path + file_name)
                    # found = True
                    # file = open('conditions.txt', 'w')
                    # if k < 19:
                        # file.write(str(i) + '\n' + str(j) + '\n' + str(k + 1) + '\n')
                    # elif j < 200:
                        # file.write(str(i) + '\n' + str(j + 20) + '\n' + str(0) + '\n')
                    # else:
                        # file.write(str(i + 20) + '\n' + str(0) + '\n' + str(0) + '\n')
                    # file.close()
                    # print file_name
                # if found:
                    # break
            # rep = 0
            # if found:
                # break
        # rep = 0
        # gamma_sta = 0
        # if found:
            # break
                    #print input_path + file_name
    x = np.ones(4096);
    i = 0
    j = 0
    k = 0
    # print len(x)
    return x, i, j, k
	
if __name__ == '__main__':	
	gen()
