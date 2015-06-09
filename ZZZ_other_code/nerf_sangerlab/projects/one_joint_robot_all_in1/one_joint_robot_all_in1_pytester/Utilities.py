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
                (0x22,      'f_fr_I',      1.0,         'float32',      'Qt.red'),  \
                (0x24,      'f_fr_II',      1.0,         'float32',      'Qt.green'),  \
                (0x26,      'i_total_spike_count_sync_MNs',      1.0,         'int32',      'Qt.black'),  \
                (0x28,      'i_EPSC_CN_to_MN',      1.0,         'int32',      'Qt.magenta'),  \
                (0x2A,      'i_MN1_spkcnt',      1.0,         'int32',      'Qt.darkRed'),  \
                (0x2C,      'f_force',      1.0,         'float32',      'Qt.darkGray')
               # (0x2E,      'raster32',      0.0,         'int32',      'Qt.darkGray')
#                (0x36,      'i_emg_mu7',     1.0,         'int32',      'Qt.blue'), \
#                (0x28,      'f_force_mu3',     1.0,         'float32',      'Qt.red')
NUM_CHANNEL = len(CHIN_PARAM) # Number of channels
DATA_OUT_ADDR = list(zip(*CHIN_PARAM)[0])
CH_TYPE = list(zip(*CHIN_PARAM)[3])
                
#            trig_id    name          type          default_value                
CHOUT_PARAM =   (1, 'pps_coef_Ia',  'float32',      30.0), \
                (2, 'tau',  'float32',      0.03), \
                (3, 'gain_CN_to_MN',   'int32',       1), \
                (4, 'gamma_dyn',    'float32',      80.0), \
                (5, 'gamma_sta',    'float32',      80.0), \
                (6, 'gain_SN_to_MN',      'int32',        1),  \
                (7, 'trigger_input',      'int32',        1),  \
                (8, 'i_M1_CN2_drive',      'int32',        1),  \
                (9, 'gain_CN2_to_MN',      'int32',        1),  \
                (10, 'bicep_len_pxi',      'float32',        1.1),  \
                (11, 'i_m1_drive',      'int32',        0),  \
                (12, 'gain_SN_to_CN',      'int32',        1)
                


                
SEND_TYPE = list(zip(*CHOUT_PARAM)[2])   

BIT_FILE = "../one_joint_robot_all_in1_xem6010.bit"
#BIT_FILE = "../one_joint_robot_all_in1_xem6010_before_S_fixed.bit"
#BIT_FILE = "../one_joint_robot_all_in1_xem6010_twitch.bit"
#BIT_FILE2 = "../one_joint_robot_xem6010 (copy).bit"


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

