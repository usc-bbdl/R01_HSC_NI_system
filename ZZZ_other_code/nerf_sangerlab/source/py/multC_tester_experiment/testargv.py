

if (board_scheme.CORTICAL_BOARDS ==1 and board_scheme.LINUX == 1) :   
    print "cortical boards setup"
    board_scheme.xemSerialList = [ '11160001CJ',  '12320003RM']  # CORTICAL BOARDS
#        board_scheme.xemSerialList = [ '000000054P',  '000000053X']  # CORTICAL BOARDS 6 copper top
#        board_scheme.xemSerialList = ['0000000547', '000000054B']  # CORTICAL BOARDS  BBDL setting
elif (board_scheme.TWO_BOARDS ==1 and board_scheme.LINUX == 1):
##        print "2 boards in linux setup"
    board_scheme.xemSerialList = ['124300046A', '1201000216']
#        board_scheme.xemSerialList = ['12320003RN', '12430003T2'] 
#        board_scheme.xemSerialList = ['000000054G', '000000053U'] #6 copper top
#        board_scheme.xemSerialList = ['000000054K', '0000000550'] # copper top

elif (board_scheme.THREE_BOARDS ==1 and board_scheme.LINUX == 1):
    print "3 boards in linux setup"
#        board_scheme.xemSerialList = ['124300046A', '12320003RM', '1201000216'] 
#        board_scheme.xemSerialList = ['12320003RN', '11160001CJ',  '12430003T2']
    board_scheme.xemSerialList = ['000000054G', '000000054P',  '000000053U'] # copper top
#        board_scheme.xemSerialList = ['000000054K', '000000053X',  '0000000550'] # copper top
#        board_scheme.xemSerialList = ['113700021E', '0000000547', '0000000542']  # BBDL setting
#        board_scheme.xemSerialList = ['11160001CG', '000000054B', '1137000222']  # BBDL setting
elif (board_scheme.WINDOWS == 1): # assumes three board setting
    print "windows setup"
    if (board_scheme.CORTICAL_BOARDS ==1) :   
        board_scheme.xemSerialList = [ '0000000547',  '000000054B']  # CORTICAL BOARDS
    else:
        if (board_scheme.TWO_BOARDS ==1) :   
            board_scheme.xemSerialList = ['113700021E', '0000000542'] 
#               board_scheme.xemSerialList = ['11160001CG', '1137000222'] 

        else: 
#                board_scheme.xemSerialList = ['113700021E', '0000000547', '0000000542']  # BBDL setting
            board_scheme.xemSerialList = ['11160001CG', '000000054B', '1137000222']  # BBDL setting
