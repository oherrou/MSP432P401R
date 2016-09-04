import serial
import struct
from MyPrint import printMessage
from MyPrint import printError
from MyPrint import printPrefix

#######################################################################################################################
def ComConnect(comPort,timeout):
	printPrefix(7,"SERIAL", 4, "Connection to port COM{} ...".format(comPort))
	try:
		ser = serial.Serial()
		ser.port = 'COM'+str(comPort)
		ser.timeout = timeout
		ser.open()
	except:
		try:
			ser.close()
		except:
			pass
		printError(2,"Could not open port COM{}".format(comPort))
		sys.exit(1)
	return ser


#######################################################################################################################
def ComDisconnect(ser,comPort,timeout):
	printPrefix(7,"SERIAL", 4, "Closing COM{} ...".format(comPort))
	try:
		ser.close()
	except:
		printError(2,"Could not close port COM{}".format(comPort))
		return 1
	printPrefix(7,"SERIAL", 4, "COM{} is closed".format(comPort))
	return 0
