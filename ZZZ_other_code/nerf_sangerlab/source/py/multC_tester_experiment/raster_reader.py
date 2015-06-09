import numpy as np
from matplotlib.pyplot import *

x = np.loadtxt("C:/nerf_sangerlab/source/py/multC_tester_experiment/1125_resimulation/re_expt_rampnhold.gd_0.gs_60.rep_11.dev_fpga_resampled")
pos = x[:,8]
x = x[:,0].astype(int)

bit_mask = 0x0000001000

x = bit_mask & x

print x

subplot(2,1,1)
plot(x)
subplot(2,1,2)
plot(pos)
show()

