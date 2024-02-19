#!/usr/bin/env python3
import time
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket

clients = []
class SimpleTCI(WebSocket):

    currFreq = 14000000
    currTrx = False
    currModulation = 'USB'
    currDrive = 20
    currRITOffset = 0
    currRITEnable = False
    currCWSpeed = 20
    currXITOffset = 0
    currXITEnable = False

    def sendTCIMessage(self, msg):
        print(f"Sending msg: {msg}")
        time.sleep(0.1)
        for client in clients:
           client.sendMessage(msg + ";")

    def VFOCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currFreq = args[2]
        self.sendTCIMessage(f"vfo:0,0,{self.currFreq};")
 
    def TRXCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currTrx = (args[1].lower() == 'true')
        self.sendTCIMessage(f"trx:0,{self.currTrx};")

    def MODULATIONCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currModulation = args[1].upper() 
        self.sendTCIMessage(f"modulation:0,{self.currModulation}")

    def DRIVECommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currDrive = args[1]
        self.sendTCIMessage(f"drive:0,{self.currDrive}")

    def RITOFFSETCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currRITOffset = args[1]
        self.sendTCIMessage(f"rit_offset:0,{self.currRITOffset}")

    def RITENABLECommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currRITEnable = (args[1].lower() == 'true')
        self.sendTCIMessage(f"rit_enable:0,{self.currRITEnable}")

    def XITOFFSETCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currXITOffset = args[1]
        self.sendTCIMessage(f"xit_offset:0,{self.currXITOffset}")

    def XITENABLECommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currXITEnable = (args[1].lower() == 'true')
        self.sendTCIMessage(f"xit_enable:0,{self.currXITEnable}")
 
    def CWMACROSSPEEDCommand(self, args, getNum) :
        if ( len(args) > getNum ) :
            self.currCWSpeed = args[0]
        self.sendTCIMessage(f"cw_macros_speed:0,{self.currCWSpeed}")

    def processCommand(self, argString, getArgsNum, fct):
        stripArgs = argString.replace(';','')
        args = stripArgs.split(",")
        if len(args) < getArgsNum :
            return
        fct(args, getArgsNum); 

    def handleMessage(self):
        commandString = self.data.split(":");
        print(f"Received: {commandString}")
        command = commandString[0].upper()
        if command == "VFO" :
           self.processCommand(commandString[1], 2, self.VFOCommand)
        if command == "TRX" :
           self.processCommand(commandString[1], 1, self.TRXCommand)
        if command == "MODULATION" :
           self.processCommand(commandString[1], 1, self.MODULATIONCommand)
        if command == "DRIVE" :
           self.processCommand(commandString[1], 1, self.DRIVECommand)
        if command == "RIT_OFFSET" :
           self.processCommand(commandString[1], 1, self.RITOFFSETCommand)
        if command == "RIT_ENABLE" :
           self.processCommand(commandString[1], 1, self.RITENABLECommand)
        if command == "XIT_OFFSET" :
           self.processCommand(commandString[1], 1, self.XITOFFSETCommand)
        if command == "XIT_ENABLE" :
           self.processCommand(commandString[1], 1, self.XITENABLECommand)
        if command == "CW_MACROS_SPEED" :
           self.processCommand(commandString[1], 0, self.CWMACROSSPEEDCommand)

       # for client in clients:
       #    if client != self:
       #       client.sendMessage(self.data);

    def handleConnected(self):
        print(self.address, 'connected')
        clients.append(self)
        self.sendTCIMessage("VFO_LIMITS:10000,30000000;TRX_COUNT:2");
        self.sendTCIMessage("DEVICE:SunSDR2DX;MODULATIONS_LIST:AM,SAM,LSB,USB,CW,NFM,WFM;PROTOCOL:ExpertSDR3,1.9");
        self.sendTCIMessage("VFO:0,0,14000000;TRX:0,false;MODULATION:0,USB;RIT_OFFSET:0,-50;RIT_ENABLE:0,false");
        self.sendTCIMessage("READY;");

    def handleClose(self):
        clients.remove(self)
        print(self.address, 'closed')

server = SimpleWebSocketServer('', 8000, SimpleTCI)
server.serveforever()
