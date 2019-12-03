import pyautogui
import serial
import argparse
import time
import logging
import functions

state = False

class MyControllerMap:
    def __init__(self):
        self.button = {'0': 'L', '1': 'F', '2': 'G'} # Fast forward (10 seg) pro Youtube

class SerialControllerInterface:
    # Protocolo
    # byte 1 -> Botão 1 (estado - Apertado 1 ou não 0)
    # byte 2 -> EOP - End of Packet -> valor reservado 'X'

    def __init__(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate=baudrate)
        self.mapping = MyControllerMap()
        self.incoming = '0'
        pyautogui.PAUSE = 0  ## remove delay
    
    def update(self):
        ## Sync protocol
        while self.incoming != b'X':
            self.incoming = self.ser.read()
            logging.debug("Received INCOMING: {}".format(self.incoming))

        id = self.ser.read()
        status = self.ser.read()
        logging.debug("Received DATA: {} {}".format(id,status))

        if id == b'0':
            #logging.info("KEYDOWN {}".format(id.decode("utf-8") ))
            #pyautogui.keyDown(self.mapping.button[id.decode("utf-8") ])
            if(status ==  b'1'):
                functions.previous()
        elif id == b'1':
            #logging.info("KEYUP {}".format(id.decode("utf-8") ))
            #pyautogui.keyUp(self.mapping.button[id.decode("utf-8") ])
            if(status ==  b'1'):
                #functions.mute()
                functions.pause()
                #print("saida do get window\n")
                #print(functions.getwindow())
        elif id == b'2':
            if (status == b'1'):
                functions.next()
        
        elif id == b'3':
            if (status == b'1'):
                functions.SpotifyStart()

        elif id == b'4':
            if (status == b'1'):
                functions.mute()
                
        elif id == b'A':
            if (status == b'0'):
                functions.volu(10)
            elif (status == b'1'):
                functions.volu(20)
            elif (status == b'2'):
                functions.volu(30)
            elif (status == b'3'):
                functions.volu(40)
            elif (status == b'4'):
                functions.volu(50)
            elif (status == b'5'):
                functions.volu(60)
            elif (status == b'6'):
                functions.volu(70)
            elif (status == b'7'):
                functions.volu(80)
            elif (status == b'8'):
                functions.volu(90)
            elif (status == b'9'):
                functions.volu(100)

        self.incoming = self.ser.read()

class DummyControllerInterface:
    def __init__(self):
        self.mapping = MyControllerMap()

    def update(self):
        pyautogui.keyDown(self.mapping.button['A'])
        time.sleep(0.1)
        pyautogui.keyUp(self.mapping.button['A'])
        logging.info("[Dummy] Pressed A button")
        time.sleep(1)


if __name__ == '__main__':
    interfaces = ['dummy', 'serial']
    argparse = argparse.ArgumentParser()
    argparse.add_argument('serial_port', type=str, default='COM13')
    argparse.add_argument('-b', '--baudrate', type=int, default=9600)
    argparse.add_argument('-c', '--controller_interface', type=str, default='serial', choices=interfaces)
    argparse.add_argument('-d', '--debug', default=False, action='store_true')
    args = argparse.parse_args()
    
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)

    print("Connection to {} using {} interface ({})".format(args.serial_port, args.controller_interface, args.baudrate))
    if args.controller_interface == 'dummy':
        controller = DummyControllerInterface()
    else:
        controller = SerialControllerInterface(port=args.serial_port, baudrate=args.baudrate)

    while True:
        controller.update()
