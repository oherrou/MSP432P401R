#######################################################################################################################
def printError(iLevel,szMessage):
	if(iLevel == 0):
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='DEBUG', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
	elif(iLevel == 1):
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='INFO', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
	elif(iLevel == 2):
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='WARN', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
	elif(iLevel == 3):
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='ERROR', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
	elif(iLevel == 4):
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='FATAL', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
	else:
		print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx='UNKNW', fill='5',sp=' --- ',fill2='5',msg=szMessage,fill3='0'))
		

#######################################################################################################################
def printMessage(iSpace, szMessage):
	try:
		print("{sp: >{fill}}{msg: <{fill2}}".format(sp='', fill=iSpace,msg=szMessage,fill2='0'))
	except:
		printError(2,"Error while calling the function printMessage")


#######################################################################################################################		
def printPrefix(iSpacePfx, szPfx, iSpaceMsg, szMsg):
	print("[{pfx: >{fill}}]{sp: <{fill2}}{msg: >{fill3}}".format(pfx=szPfx, fill=iSpacePfx,sp='',fill2=iSpaceMsg,msg=szMsg,fill3='0'))

