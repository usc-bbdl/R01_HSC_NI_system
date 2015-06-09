#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, PyQt4
from PyQt4.QtGui import QFileDialog

from PyQt4.QtCore import QTimer,  SIGNAL, SLOT, Qt,  QRect
from MVC_MainGUI import MultiXemScheduler
from C_XemScheduler import SingleXemTester # Controller in MVC
from M_Fpga import SomeFpga # Model in MVC
from V_Display import View, ViewChannel,  CtrlChannel # Viewer in MVC
from Utilities import *
from generate_sin import gen as gen_sin
#from cortex import cortexView
import os

sys.path.append('../')
import platform 
arch = platform.architecture()[0]
if arch == "32bit":
    from opalkelly_32bit import ok
elif arch == "64bit":
    from opalkelly_64bit import ok

if __name__ == "__main__":        
    app = PyQt4.QtGui.QApplication(sys.argv)
    
    LINUX = 0
    WINDOWS = 1
    assert (LINUX + WINDOWS ==1),  "CHOOSE ONE ENVIRONMENT!"
    
    TWO_BOARDS= 0
    THREE_BOARDS=1
    CORTICAL_BOARDS= 0
    assert (TWO_BOARDS+THREE_BOARDS+CORTICAL_BOARDS== 1), "CHOOSE ONE BOARD SETTING!"
    
    if (WINDOWS==1) :
        ROOT_PATH = "C:\\nerf_sangerlab\\projects\\"  # windows setting
    if (LINUX==1):
        ROOT_PATH = "/home/eric/nerf_verilog_eric/projects/"

    #################################################
    if (TWO_BOARDS == 1):
        PROJECT_LIST = ["rack_test", "rack_emg"] 

    if (THREE_BOARDS ==1) :
        PROJECT_LIST = ["rack_test", "rack_CN_simple_S1M1", "rack_emg"]   # rack_CN_simple_general
    
    if (CORTICAL_BOARDS ==1) :
        PROJECT_LIST = ["rack_CN_simple_S1M1", "rack_CN_simple_S1M1"]   # rack_CN_general
        
    PROJECT_PATH = [(ROOT_PATH + p) for p in PROJECT_LIST]
    DEVICE_MODEL = "xem6010"
    
     ### Building M in MVC
    xemList = []
    testrun = ok.FrontPanel()
    numFpga = testrun.GetDeviceCount()
    assert numFpga > 0, "No OpalKelly boards found, is one connected?"
    print "Found ",  numFpga, " OpalKelly devices:"           
    xemSerialList = [testrun.GetDeviceListSerial(i) for i in xrange(numFpga)]
    print xemSerialList
    
    xemSerialList_agonist = ['113700021E', '0000000547', '0000000542']  # BBDL setting
   
        
        
    #sys.path.append(PROJECT_PATH)
    from config_test import NUM_NEURON, SAMPLING_RATE, FPGA_OUTPUT_B1, FPGA_OUTPUT_B2, FPGA_OUTPUT_B3,   USER_INPUT_B1,  USER_INPUT_B2,  USER_INPUT_B3
    FPGA_OUTPUT_B = []
    USER_INPUT_B = []
    
    if (TWO_BOARDS ==1) :
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
        USER_INPUT_B.append(USER_INPUT_B1)
        USER_INPUT_B.append(USER_INPUT_B3)
    
    if (THREE_BOARDS ==1) :
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
        USER_INPUT_B.append(USER_INPUT_B1)
        USER_INPUT_B.append(USER_INPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B3)
    
    if (CORTICAL_BOARDS==1):
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B2)
    
    for idx,  name in enumerate(xemSerialList_agonist):
        print idx,  name
        serX = xemSerialList_agonist[idx]
        xem = SomeFpga(NUM_NEURON, SAMPLING_RATE, serX)
        xemList.append(xem)
    
    """ RESET FPGAs  """
    BUTTON_RESET =0
    BUTTON_INPUT_FROM_TRIGGER = 1
    
    for idx,  name in enumerate(xemSerialList_agonist):
        xemList[idx].SendButton(True, BUTTON_RESET)   # send to FPGA (flexor)
        xemList[idx].SendButton(False, BUTTON_RESET)
        print "reset_global board:",  idx
    ### Building V in MVC
    
    vList = []

    print  xemSerialList_agonist, len(xemList) # 3
    print "temp loc: ",  i, len(xemList)
    
    for i in xrange(len(xemList)):
        dispWin = View(count = i,  projectName = PROJECT_LIST[i] ,  projectPath = PROJECT_PATH[i],  nerfModel = xemList[i],  fpgaOutput = FPGA_OUTPUT_B[i],  userInput = USER_INPUT_B[i])
        vList.append(dispWin)

#    # display VIEW windows for each channel
#    for i in xrange(len(xemList)):
#        vList[i].show()
    
    ### Building C::(M,V)->C in MVC
    cList = []
    for i in xrange(len(xemList)):
        testerGui = SingleXemTester(xemList[i], vList[i], USER_INPUT_B[i],  xem.HalfCountRealTime())
        cList.append(testerGui)

    #testerGui.show()
    
    print vList
    ### global control for MVC
    threeBoard = MultiXemScheduler(xemList = xemList, cList = cList,  vList = vList, halfCountRealTime = xem.HalfCountRealTime() )
#    threeBoard.show()
   
    """burn bit file"""
    bitFileList = []
    for eachV in vList:
        bitFileList.append(str(eachV.listWidget.currentItem().text()))
            
    print bitFileList
    for eachXem, eachBitFile in zip(xemList, bitFileList):
            eachXem.BurnBitFile(eachBitFile)
            
    """ start the simulation"""
    for eachC in cList:
            eachC.startSim()
   
   
    """ closing"""
    for eachC in cList:
            eachC.close()
    
    """ ==============antagonist=========="""
    xemSerialList_antagonist = ['11160001CG', '000000054B', '1137000222']  # BBDL setting
    
    xemList=[]
    #sys.path.append(PROJECT_PATH)
    from config_test import NUM_NEURON, SAMPLING_RATE, FPGA_OUTPUT_B1, FPGA_OUTPUT_B2, FPGA_OUTPUT_B3,   USER_INPUT_B1,  USER_INPUT_B2,  USER_INPUT_B3
    FPGA_OUTPUT_B = []
    USER_INPUT_B = []
    
    if (TWO_BOARDS ==1) :
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
        USER_INPUT_B.append(USER_INPUT_B1)
        USER_INPUT_B.append(USER_INPUT_B3)
    
    if (THREE_BOARDS ==1) :
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
        USER_INPUT_B.append(USER_INPUT_B1)
        USER_INPUT_B.append(USER_INPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B3)
    
    if (CORTICAL_BOARDS==1):
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B2)
        USER_INPUT_B.append(USER_INPUT_B2)
    
    for idx,  name in enumerate(xemSerialList_antagonist):
        print idx,  name
        serX = xemSerialList_antagonist[idx]
        xem = SomeFpga(NUM_NEURON, SAMPLING_RATE, serX)
        xemList.append(xem)
    
    """ RESET FPGAs  """
    BUTTON_RESET =0
    BUTTON_INPUT_FROM_TRIGGER = 1
    
    for idx,  name in enumerate(xemSerialList_antagonist):
        xemList[idx].SendButton(True, BUTTON_RESET)   # send to FPGA (flexor)
        xemList[idx].SendButton(False, BUTTON_RESET)
        print "reset_global board:",  idx
    ### Building V in MVC
    
    vList = []
    
    print  xemSerialList_agonist, len(xemList) # 3
    print "temp loc: ",  i, len(xemList)
 
    for i in xrange(len(xemList)):
        dispWin = View(count = i,  projectName = PROJECT_LIST[i] ,  projectPath = PROJECT_PATH[i],  nerfModel = xemList[i],  fpgaOutput = FPGA_OUTPUT_B[i],  userInput = USER_INPUT_B[i])
        vList.append(dispWin)

#    # display VIEW windows for each channel
#    for i in xrange(len(xemList)):
#        vList[i].show()
    
    ### Building C::(M,V)->C in MVC
    cList = []
    for i in xrange(len(xemList)):
        testerGui = SingleXemTester(xemList[i], vList[i], USER_INPUT_B[i],  xem.HalfCountRealTime())
        cList.append(testerGui)

    #testerGui.show()
    
    print vList
    ### global control for MVC
    threeBoard = MultiXemScheduler(xemList = xemList, cList = cList,  vList = vList, halfCountRealTime = xem.HalfCountRealTime() )
#    threeBoard.show()

    """burn bit file"""
    bitFileList = []
    for eachV in vList:
        bitFileList.append(str(eachV.listWidget.currentItem().text()))
            
    print bitFileList
    for eachXem, eachBitFile in zip(xemList, bitFileList):
            eachXem.BurnBitFile(eachBitFile)

    """ start the simulation """
    for eachC in cList:
            eachC.startSim()
            
       
    """ closing"""
    for eachC in cList:
            eachC.close()
    
    
    
#    
#    """ cortical boards resetting for voluntary command"""
#    TWO_BOARDS= 0
#    THREE_BOARDS=0
#    CORTICAL_BOARDS= 1
#    assert (TWO_BOARDS+THREE_BOARDS+CORTICAL_BOARDS== 1), "CHOOSE ONE BOARD SETTING!"
#    
#    if (CORTICAL_BOARDS ==1) :
#        PROJECT_LIST = ["rack_CN_simple_S1M1", "rack_CN_simple_S1M1"]   # rack_CN_general
#    
#        
#    PROJECT_PATH = [(ROOT_PATH + p) for p in PROJECT_LIST]
#    DEVICE_MODEL = "xem6010"
#    
#    
#    ### Building M in MVC
#    numFpga = 2       
#  
#    xemSerialList_CN = [ '0000000547',  '000000054B']  # CORTICAL BOARDS (BBDL setting)
#    
#    xemList=[]
#    
#    
#    #sys.path.append(PROJECT_PATH)
#    from config_test import NUM_NEURON, SAMPLING_RATE, FPGA_OUTPUT_B1, FPGA_OUTPUT_B2, FPGA_OUTPUT_B3,   USER_INPUT_B1,  USER_INPUT_B2,  USER_INPUT_B3
#    FPGA_OUTPUT_B = []
#    USER_INPUT_B = []
#    
#    if (TWO_BOARDS ==1) :
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
#        USER_INPUT_B.append(USER_INPUT_B1)
#        USER_INPUT_B.append(USER_INPUT_B3)
#    
#    if (THREE_BOARDS ==1) :
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B1)
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B3)
#        USER_INPUT_B.append(USER_INPUT_B1)
#        USER_INPUT_B.append(USER_INPUT_B2)
#        USER_INPUT_B.append(USER_INPUT_B3)
#    
#    if (CORTICAL_BOARDS==1):
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
#        FPGA_OUTPUT_B.append(FPGA_OUTPUT_B2)
#        USER_INPUT_B.append(USER_INPUT_B2)
#        USER_INPUT_B.append(USER_INPUT_B2)
#    
#    
#    print "list CN", xemSerialList_CN
#    for idx,  name in enumerate(xemSerialList_CN):
#        print idx,  name
#        serX = xemSerialList_CN[idx]
#        xem = SomeFpga(NUM_NEURON, SAMPLING_RATE, serX)
#        xemList.append(xem)
#    
#    """ RESET FPGAs  """
#    BUTTON_RESET =0
#    BUTTON_INPUT_FROM_TRIGGER = 1
#    
#    for idx,  name in enumerate(xemSerialList_CN):
#        xemList[idx].SendButton(True, BUTTON_RESET)   # send to FPGA (flexor)
#        xemList[idx].SendButton(False, BUTTON_RESET)
#        print "reset_global board:",  idx
#    ### Building V in MVC
#    
#    vList = []
#    
#    print  xemSerialList_CN, len(xemList) # 3
#    print "temp loc: ",  i, len(xemList)
# 
#    for i in xrange(len(xemList)):
#        dispWin = View(count = i,  projectName = PROJECT_LIST[i] ,  projectPath = PROJECT_PATH[i],  nerfModel = xemList[i],  fpgaOutput = FPGA_OUTPUT_B[i],  userInput = USER_INPUT_B[i])
#        vList.append(dispWin)
#
##    # display VIEW windows for each channel
##    for i in xrange(len(xemList)):
##        vList[i].show()
#    
#    ### Building C::(M,V)->C in MVC
#    cList = []
#    for i in xrange(len(xemList)):
#        testerGui = SingleXemTester(xemList[i], vList[i], USER_INPUT_B[i],  xem.HalfCountRealTime())
#        cList.append(testerGui)
#
#    #testerGui.show()
#    
#    print vList
#    ### global control for MVC
#    threeBoard = MultiXemScheduler(xemList = xemList, cList = cList,  vList = vList, halfCountRealTime = xem.HalfCountRealTime() )
##    threeBoard.show()
#
#    """burn bit file"""
#    bitFileList = []
#    for eachV in vList:
#        bitFileList.append(str(eachV.listWidget.currentItem().text()))
#            
#    print bitFileList
#    for eachXem, eachBitFile in zip(xemList, bitFileList):
#            eachXem.BurnBitFile(eachBitFile)
#
#    """ start the simulation """
#    for eachC in cList:
#            eachC.startSim()
#            
#    
#    """ sine wave checked"""
#    print "waveform sine_bic fed"
#    pipeInData_bic = gen_sin(F = 0.5, AMP = 50000.0,  BIAS = 0.0,  T = 2.0) # was 150000 for CN_general
#    pipeInDataBic=[]
#    for i in xrange(0,  2048):
#        pipeInDataBic.append(max(0.0,  pipeInData_bic[i]))
#
#    print "waveform sine_tri fed"
#    pipeIndata_tri = -gen_sin(F = 0.5,  AMP = 50000.0,  BIAS = 0.0,  T = 2.0)
#    pipeInDataTri=[]
#    for i in xrange(0,  2048):
#        pipeInDataTri.append(max(0.0,  pipeIndata_tri[i]))
#
#    xemList[0].SendPipe(pipeInDataTri)
#    xemList[1].SendPipe(pipeInDataBic)
#
#    
#    
#    
#    sys.exit(app.exec_())
#    
#    
#    
#
#            
