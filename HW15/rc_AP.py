try:
 import usocket as socket        #importing socket
except:
 import socket
import network            #importing network
import gc
import machine

gc.collect()
ssid = 'RPI_PICO_AP'                  #Set access point name 
password = '12345678'      #Set your access point password

tx = machine.Pin(4, machine.Pin.OUT)
rx = machine.Pin(5, machine.Pin.OUT)
uart = machine.UART(1, 9600)                         # init with given baudrate
uart.init(9600, bits=8, parity=None, stop=1, tx=tx, rx=rx) # init with given parameters
    

def move_forward():
    print ("Forward")
    uart.write('1\n')
    
def move_backward():
    print ("Backward")
    uart.write('2\n')
    
def move_stop():
    print ("Stop")
    uart.write('3\n')
    
def move_left():
    print ("Left")
    uart.write('4\n')
    
def move_right():
    print ("Right")
    uart.write('5\n')
    


ap = network.WLAN(network.AP_IF)
ap.config(essid=ssid, password=password)
ap.active(True)            #activating

while ap.active() == False:
  pass
print('Connection is successful')
print(ap.ifconfig())

def webpage():
    page = open('index.html')
    html = page.read()
    page.close()
    return str(html)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   #creating socket object
s.bind(('', 80))
s.listen(5)
while True:
    conn, addr = s.accept()
    print('Got a connection from %s' % str(addr))
    request = conn.recv(1024)

    request = str(request)
    try:
        request = request.split()[1]
    except IndexError:
        pass
    if request == '/forward?':
        move_forward()
    elif request =='/left?':
        move_left()
    elif request =='/stop?':
        move_stop()
    elif request =='/right?':
        move_right()
    elif request =='/back?':
        move_backward()
    
    print('Content = %s' % str(request))
    response = webpage()
    conn.send(response)
    conn.close()

