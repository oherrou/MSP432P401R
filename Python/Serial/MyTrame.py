import struct
import MySerial
from MyPrint import printMessage
from MyPrint import printError
from MyPrint import printPrefix


UART_HEADER = struct.pack("B",1)
UART_FOOTER = struct.pack("B",2)
OPCODE_RGB = 0x48

#######################################################################################################################
def sendPacket(ser, data):
	ser.flushInput()
	s_data =  struct.pack("B", len(data))
	printMessage(0, "{:15}{}".format("> Payload",data))
	#print(str(bytes(data)).replace("\\x"," 0x").replace("'","")[1:])
	printMessage(0, "> Sending...")
	ser.write(UART_HEADER)
	printMessage(0, "{:15}{}".format("> Received",ser.readline().decode(encoding='UTF-8')).strip())	
	ser.write(s_data) #length
	printMessage(0, "{:15}{}".format("> Received",str(ser.readline())[2:-3])) #need to remove one 'b and \n'
	for i in range(len(data)):
		ser.write(struct.pack("B", data[i]))
	#TODO CRC
	ser.write(UART_FOOTER)
	printMessage(0, "{:15}{}".format("> Received",ser.readline().decode(encoding='UTF-8')).strip())	
	printMessage(0, "{:15}{}".format("> Response",ser.readline().decode(encoding='UTF-8')).strip())
# Il faut rajouter un truc qui acknoledge lorsqu'on a recu la trame histoire qu'on puisse se resynchroniser ?


#######################################################################################################################
def createPacket(opcode, dataIn):
	dataOut = []
	dataOut.append(opcode)
	dataOut.extend(dataIn)
	return dataOut