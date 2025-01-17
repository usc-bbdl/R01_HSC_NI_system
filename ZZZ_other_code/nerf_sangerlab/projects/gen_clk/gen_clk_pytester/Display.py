# -*- coding: utf-8 -*-

"""
Module implementing MainWindow.
"""

from PyQt4.QtGui import QMainWindow
from PyQt4.QtCore import pyqtSignature
from PyQt4.QtCore import QTimer,  SIGNAL, SLOT, Qt,  QRect
from PyQt4.QtGui import QPainter, QRegion, QPen
import sys, random

INIT_X = 200 # pixels offset

from Ui_Display import Ui_Dialog


class View(QMainWindow, Ui_Dialog):
    """
    Class View inherits the GUI generated by QtDesigner, and add customized actions
    """
    def __init__(self, parent = None, \
                 NUM_CHANNEL = 1, DISPLAY_SCALING = [1.0], \
                 VIEWER_REFRESH_RATE = 5, CHANNEL_COLOR = [Qt.blue]):
        """
        Constructor
        """
#        QMainWindow.__init__(self, parent, Qt.FramelessWindowHint)
        QMainWindow.__init__(self, parent)
        self.setupUi(self)
        self.x = 200
        ZERO_DATA = [0.0 for ix in xrange(NUM_CHANNEL)]
        self.data = self.data_1 = ZERO_DATA
        self.pen = QPen()


        self.numPt = INIT_X
        self.isPause = False
        self.NUM_CHANNEL = NUM_CHANNEL
        self.DISPLAY_SCALING = DISPLAY_SCALING
        self.PEN_COLOR = CHANNEL_COLOR



        self.timer = QTimer(self)
        #self.connect(timer, SIGNAL("timeout()"), self, SLOT("update()"))
        #self.connect(timer, SIGNAL("timeout()"), self, SLOT("timerEvent()"))
        self.connect(self.timer, SIGNAL("timeout()"), self.onTimeOut)
        self.connect(self.doubleSpinBox, SIGNAL("editingFinished()"), self.onCh0Gain)
        self.connect(self.doubleSpinBox_2, SIGNAL("editingFinished()"), self.onCh1Gain)
        self.connect(self.doubleSpinBox_3, SIGNAL("editingFinished()"), self.onCh2Gain)
        self.connect(self.doubleSpinBox_4, SIGNAL("editingFinished()"), self.onCh3Gain)
        self.connect(self.doubleSpinBox_5, SIGNAL("editingFinished()"), self.onCh4Gain)
        self.connect(self.doubleSpinBox_6, SIGNAL("editingFinished()"), self.onCh5Gain)
        self.connect(self.doubleSpinBox, SIGNAL("valueChanged(double)"), self.onCh0Gain)
        self.connect(self.doubleSpinBox_2, SIGNAL("valueChanged(double)"), self.onCh1Gain)
        self.connect(self.doubleSpinBox_3, SIGNAL("valueChanged(double)"), self.onCh2Gain)
        self.connect(self.doubleSpinBox_4, SIGNAL("valueChanged(double)"), self.onCh3Gain)
        self.connect(self.doubleSpinBox_5, SIGNAL("valueChanged(double)"), self.onCh4Gain)
        self.connect(self.doubleSpinBox_6, SIGNAL("valueChanged(double)"), self.onCh5Gain)
        
        self.doubleSpinBox.setValue(DISPLAY_SCALING[0])
        self.doubleSpinBox_2.setValue(DISPLAY_SCALING[1])
        self.doubleSpinBox_3.setValue(DISPLAY_SCALING[2])
        self.doubleSpinBox_4.setValue(DISPLAY_SCALING[3])
        self.doubleSpinBox_5.setValue(DISPLAY_SCALING[4])
        self.doubleSpinBox_6.setValue(DISPLAY_SCALING[5])

        self.timer.start(VIEWER_REFRESH_RATE)

    def newData(self, newData):
        self.data_1 = self.data
        self.data = newData

    def onTimeOut(self):

        if (self.isPause):
            return
        size = self.size()
        self.update(QRect(self.x, 0,size.width() - self.x + 1,size.height()))

        if (self.x < size.width()):
            self.x = self.x + 1     
        else:
            self.x = INIT_X
            
    def onCh0Gain(self):
        self.DISPLAY_SCALING[0] = self.doubleSpinBox.value()
            
    def onCh1Gain(self):
        self.DISPLAY_SCALING[1] = self.doubleSpinBox_2.value()
              
    def onCh2Gain(self):
        self.DISPLAY_SCALING[2] = self.doubleSpinBox_3.value()
              
    def onCh3Gain(self):
        self.DISPLAY_SCALING[3] = self.doubleSpinBox_4.value()
              
    def onCh4Gain(self):
        self.DISPLAY_SCALING[4] = self.doubleSpinBox_5.value()
              
    def onCh5Gain(self):
        self.DISPLAY_SCALING[5] = self.doubleSpinBox_6.value()
        

    def paintEvent(self, e):
        """ 
        Overload the standard paintEvent function
        """

        #p = QPainter(self.graphicsView)                         ## our painter
        p = QPainter(self)                         ## our painter
        #r1 = QRegion ( QRect(100,100,200,80), QRegion.Ellipse() )
        r1 = QRegion ( QRect(self.x,10,5,100))
        r2 = QRegion ( QRect(100,120,10,30) ) ## r2 = rectangular region
        r3 = QRegion (r1.intersect( r2 ))    ## r3 = intersection
        #p.setClipRegion( r1 )              ## set clip region
        self.drawPoints(p)          ## paint clipped graphics

#        qp = QPainter()
#        qp.begin(self)
#        self.drawPoints(qp)
#        qp.end()

    def drawPoints(self, qp):
        """ 
        Draw a line between previous and current data points.
        """

#        pen = self.pen


        size = self.size()
        self.yOffset = [size.height()*0.2 + size.height()*0.618/self.NUM_CHANNEL * y for y in xrange(self.NUM_CHANNEL) ]

        for ix in xrange(self.NUM_CHANNEL):
            self.pen.setStyle(Qt.SolidLine)
            self.pen.setWidth(2)
            self.pen.setBrush(self.PEN_COLOR[ix])
            self.pen.setCapStyle(Qt.RoundCap)
            self.pen.setJoinStyle(Qt.RoundJoin)
            qp.setPen(self.pen)

            qp.drawLine(self.x - 2, self.yOffset[ix] - \
                        self.data_1[ix] * self.DISPLAY_SCALING[ix],\
                        self.x , self.yOffset[ix] - \
                        self.data[ix] * self.DISPLAY_SCALING[ix])
#        print "Yes!"
#        for i in range(self.numPt):
#            y = random.randint(1, size.height()-1)
#            #qp.drawPoint(x, y)     
#            qp.drawLine(self.x, y,  self.x + 3,  y)             

    @pyqtSignature("bool")
    def on_pushButton_toggled(self, checked):
        """
        Pausing the plot, FPGA calculation still continues.
        """
        self.isPause = checked
    
    @pyqtSignature("")
    def on_doubleSpinBox_editingFinished(self):
        """
        Slot documentation goes here.
        """
        print self.doubleSpinBox.value()
    

