from struct import pack, unpack
from PyQt4.QtCore import Qt

VIEWER_REFRESH_RATE = 10 # in ms, This the T for calculating digital freq
PIPE_IN_ADDR = 0x80
BUTTON_RESET = 0
BUTTON_RESET_SIM = 1
BUTTON_ENABLE_SIM = 2

DATA_EVT_CLKRATE = 0
#            address         name   visual_gain         type            color
CHIN_PARAM =    (0x20,      'f_len',      50,         'float32',      'Qt.blue'),  \
                (0x22,      'i',      1.0,         'float32',      'Qt.red'),  \
                (0x24,      'i_CN_spkcnt',      1.0,         'int32',      'Qt.green'),  \
                (0x26,      'i_emg_bic',      1.0,         'int32',      'Qt.black'),  \
                (0x28,      'f_lce_bic',      1.0,         'Uint32',      'Qt.magenta'),  \
                (0x30,      'f_force_bic',      1.0,         'float32',      'Qt.darkRed'),  \
                (0x32,      'non',      1.0,         'float32',      'Qt.darkGray'),   \
#                (0x36,      'i_emg_mu7',     1.0,         'int32',      'Qt.blue'), \
#                (0x28,      'f_force_mu3',     1.0,         'float32',      'Qt.red')
NUM_CHANNEL = len(CHIN_PARAM) # Number of channels
DATA_OUT_ADDR = list(zip(*CHIN_PARAM)[0])
CH_TYPE = list(zip(*CHIN_PARAM)[3])
                
#            trig_id    name          type          default_value                
CHOUT_PARAM =   (1, 'pps_coef_Ia',  'float32',      30.0), \
                (2, 'tau',  'float32',      0.01), \
                (3, 'close_loop',   'int32',        0), \
                (4, 'gamma_dyn',    'float32',      80.0), \
                (5, 'gamma_sta',    'float32',      80.0), \
                (6, 'gain_big_MN',      'int32',        4),  \
                (7, 'trigger_input',      'int32',        1),  \
                (8, 'bicep_len_pxi',      'float32',        1.1)
                


                
SEND_TYPE = list(zip(*CHOUT_PARAM)[2])   

BIT_FILE = "../rng_osc_xem6010.bit"


SAMPLING_RATE = 1024
#NUM_NEURON = 512
NUM_NEURON = 128


def ConvertType(val, fromType, toType):
    return unpack(toType, pack(fromType, val))[0]
    

import sys, re
def interp(string):
  locals  = sys._getframe(1).f_locals
  globals = sys._getframe(1).f_globals
  for item in re.findall(r'#\{([^}]*)\}', string):
    string = string.replace('#{%s}' % item,
                            str(eval(item, globals, locals)))
  return string

