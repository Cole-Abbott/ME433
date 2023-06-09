from time import sleep
#from machine import UART, Pin
import machine
import network
import socket


# Yes, these could be in another file. But on the Pico! So no more secure. :)
ssid = 'Device-Northwestern'
password = ''

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
    
def connect():
    #Connect to WLAN
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    while wlan.isconnected() == False:
        print('Waiting for connection...')
        sleep(1)
    ip = wlan.ifconfig()[0]
    print(f'Connected on {ip}')
    return ip
    
def open_socket(ip):
    # Open a socket
    address = (ip, 80)
    connection = socket.socket()
    connection.bind(address)
    connection.listen(1)
    return connection

def webpage():
    #Template HTML
    html = f"""
            <!DOCTYPE html>
            <html>
            <head>
            <title>Zumo Robot Control</title>
            </head>
            <center><b>
            <form action="./forward">
            <input type="submit" value="Forward" style="height:120px; width:120px" />
            </form>
            <table><tr>
            <td><form action="./left">
            <input type="submit" value="Left" style="height:120px; width:120px" />
            </form></td>
            <td><form action="./stop">
            <input type="submit" value="Stop" style="height:120px; width:120px" />
            </form></td>
            <td><form action="./right">
            <input type="submit" value="Right" style="height:120px; width:120px" />
            </form></td>
            </tr></table>
            <form action="./back">
            <input type="submit" value="Back" style="height:120px; width:120px" />
            </form>
            </body>
            </html>
            """
    return str(html)

def serve(connection):
    #Start web server
    while True:
        client = connection.accept()[0]
        request = client.recv(1024)
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
        html = webpage()
        client.send(html)
        client.close()

try:
    tx = machine.Pin(4, machine.Pin.OUT)
    rx = machine.Pin(5, machine.Pin.OUT)
    uart = machine.UART(1, 9600)                         # init with given baudrate
    uart.init(9600, bits=8, parity=None, stop=1, tx=tx, rx=rx) # init with given parameters
    
    
    ip = connect()
    connection = open_socket(ip)
    serve(connection)
   
    
 

except KeyboardInterrupt:
    machine.reset()

