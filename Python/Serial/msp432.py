# TODO
# 
# 


import argparse
import sys
import struct
from MySerial import ComConnect
from MySerial import ComDisconnect
from MyPrint import printMessage
from MyPrint import printError
from MyPrint import printPrefix
from MyCommand import UserSendCommand


#######################################################################################################################
def Parser():
	global args
	global ser

	parser = argparse.ArgumentParser(description="UART COMMUNICATION MSP432")
	parser.add_argument("-v", "--verbose", action="store_true", help="Increase output verbosity")
	parser.add_argument("comPort", type=int, help="COM port number")
	parser.add_argument("-timeout", type=int, help="Timeout for serial communication", default="20")
	args = parser.parse_args()



#######################################################################################################################
def UserLoopCommand(ser):
	key = "N/A"
	
	printMessage(15, "\n{}".format("Write 'quit' to stop the script\n\n"))
	while(key != "quit"):
		command 	= "N/A"
		key 		= input("{:15} ".format("\n>>> Command"))
		
		if(key == "quit"):
			printMessage(15, "Exiting the script")
			break			
		else:
			UserSendCommand(ser,key)
	return 0



def main():	
	global comPort
	global timeout	
	
	Parser()	
	comPort = args.comPort
	timeout = args.timeout
	
	#Connect
	ser = ComConnect(comPort,timeout)
	
	#User Loop=
	UserLoopCommand(ser)
	
	#Disconnect
	ComDisconnect(ser,comPort,timeout)
	sys.exit(0)	







	
main()