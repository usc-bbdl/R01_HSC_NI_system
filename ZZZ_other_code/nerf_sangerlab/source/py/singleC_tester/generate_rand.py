from numpy import *
import numpy
import random

def gen(F = 1.0, T =1.0, BIAS = 2000.0, AMP = 2000.0, SAMPLING_RATE = 1024):
    """ f = 1.0 # in Hz, continuous freq
    T = 1.0 # Total time in sec
    BIAS = 1.0
    AMP = 0.2
    """    
    dt = 1.0 / SAMPLING_RATE # Sampling interval in seconds
    #periods = T / (1/F)

    #w = F * 2 * pi * dt
    max_n = int(T * SAMPLING_RATE) - 1
    n = linspace(0 , max_n + 1, max_n + 1)
    
    """random function"""
    #x = [BIAS+random.normalvariate(AMP,  AMP*5) for i in xrange(SAMPLING_RATE*2)]
    #x = [BIAS+numpy.random.poisson(AMP) for i in xrange(SAMPLING_RATE*2)]
    x = [BIAS+AMP*random.random() for i in xrange(SAMPLING_RATE*16)]
    
    return x


if __name__ == '__main__':
    from pylab import *
    import random
    x = gen(BIAS = 3000.0, AMP = 2000.0)
    SAMPLING_RATE = 1024
    intx_list = []
    intx_i1 = 0.0
    for x_i in x:
        intx_i = intx_i1 + x_i * (1.0 /SAMPLING_RATE)
        intx_list.append(intx_i)
        intx_i1 = intx_i
    #plot(intx_list)
    plot(x)
    show()
    print "len_x = %d" % len(x)
