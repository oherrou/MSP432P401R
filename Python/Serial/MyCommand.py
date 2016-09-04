from MyPrint import printMessage
from MyPrint import printError
from MyPrint import printPrefix
from MyTrame import sendPacket
from MyTrame import createPacket

OPCODE_RGB = 0x48
OPCODE_BRIGTH = 0x49
lCommand =	[("help",	"CommandHelp",		"show help"),
			("rgb",		"CommandRGB",		"set RGB color"),
			("bright",	"CommandBright",	"set brightness")]




#######################################################################################################################
def UserSendCommand(ser,key):
	for i, val in enumerate(lCommand): #Is it a known command
		if(key == val[0]):
			function = eval(val[1]) # eval is "evil" care
			break
		else:
			function = "N/A"
	if(function == "N/A"):
		printError(1,"write 'help' to print the available command or 'quit' to exit")
	else:
		function(ser)


def CommandRGB(ser):
	szInput = input("{:15}".format("> [R][G][B]"))
	try:
		listInput = list(map(int, szInput.strip().split(' ')))
		if(len(listInput) != 3):
			printError(2,"You must enter 3 int value for each component")				
		else:
			tmp = True
			for i, val in enumerate(listInput):
				if(0 > val or val > 255):
					printError(2,"Enter value between 0-255")
					tmp = False
					break
			if (tmp):
				sendPacket(ser,createPacket(OPCODE_RGB,listInput))	
	except ValueError:
		printError(2,"Enter int value 0-255")


def CommandHelp(ser):
	printMessage(0, "\n|----------|-------------------------------|")
	printMessage(0, "|{:10}| {:30}|".format("COMMAND","DESCRIPTION"))
	printMessage(0, "|----------|-------------------------------|")
	for i, val in enumerate(lCommand):
		printMessage(0, "|{:10}| {:30}|".format(val[0],val[2]))
		printMessage(0, "|----------|-------------------------------|")


def CommandBright(ser):
	szInput = input("{:15}".format("> Brightness"))
	try:
		listInput = list(map(int, szInput.strip().split(' ')))
		if(len(listInput) != 1):
			printError(2,"Enter only 1 value for the percentage of brightness")				
		else:
			tmp = True
			for i, val in enumerate(listInput):
				if(0 > val or val > 100):
					printError(2,"Enter percentage between 0-100")
					tmp = False
					break
			if (tmp):
				sendPacket(ser,createPacket(OPCODE_BRIGTH,listInput))	
	except ValueError:
		printError(2,"Enter percentage value 0-100")