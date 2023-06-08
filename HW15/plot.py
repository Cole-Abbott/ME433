# requires adafruit_ov7670.mpy and adafruit_st7735r.mpy in the lib folder
import time
from displayio import (
    Bitmap,
    Group,
    TileGrid,
    FourWire,
    release_displays,
    ColorConverter,
    Colorspace,
    Palette,
)
from adafruit_st7735r import ST7735R
import board
import busio
import digitalio
from adafruit_ov7670 import (
    OV7670,
    OV7670_SIZE_DIV1,
    OV7670_SIZE_DIV8,
    OV7670_SIZE_DIV16,
)
from ulab import numpy as np

release_displays()
spi = busio.SPI(clock=board.GP2, MOSI=board.GP3)
display_bus = FourWire(spi, command=board.GP0, chip_select=board.GP1, reset=None)
display = ST7735R(display_bus, width=160, height=128, rotation=90) 

# Ensure the camera is shut down, so that it releases the SDA/SCL lines,
# then create the configuration I2C bus

with digitalio.DigitalInOut(board.GP10) as reset:
    reset.switch_to_output(False)
    time.sleep(0.001)
    bus = busio.I2C(board.GP9, board.GP8) #GP9 is SCL, GP8 is SDA

# Set up the camera (you must customize this for your board!)
cam = OV7670(
    bus,
    data_pins=[
        board.GP12,
        board.GP13,
        board.GP14,
        board.GP15,
        board.GP16,
        board.GP17,
        board.GP18,
        board.GP19,
    ],  # [16]     [org] etc
    clock=board.GP11,  # [15]     [blk]
    vsync=board.GP7,  # [10]     [brn]
    href=board.GP21,  # [27/o14] [red]
    mclk=board.GP20,  # [16/o15]
    shutdown=None,
    reset=board.GP10,
)  # [14]

width = display.width
height = display.height

bitmap = None
# Select the biggest size for which we can allocate a bitmap successfully, and
# which is not bigger than the display
for size in range(OV7670_SIZE_DIV1, OV7670_SIZE_DIV16 + 1):
    #cam.size = size # for 4Hz
    #cam.size = OV7670_SIZE_DIV16 # for 30x40, 9Hz
    cam.size = OV7670_SIZE_DIV8 # for 60x80, 9Hz
    if cam.width > width:
        continue
    if cam.height > height:
        continue
    try:
        bitmap = Bitmap(cam.width, cam.height, 65536)
        break
    except MemoryError:
        continue

print(width, height, cam.width, cam.height)
if bitmap is None:
    raise SystemExit("Could not allocate a bitmap")
time.sleep(4)
g = Group(scale=1, x=(width - cam.width) // 2, y=(height - cam.height) // 2)
tg = TileGrid(
    bitmap, pixel_shader=ColorConverter(input_colorspace=Colorspace.BGR565_SWAPPED)
)
g.append(tg)
display.show(g)

t0 = time.monotonic_ns()
display.auto_refresh = False

# arrays to store the color data
reds = np.zeros(60,dtype=np.uint16)
greens = np.zeros(60,dtype=np.uint16)
blues = np.zeros(60,dtype=np.uint16)
bright = np.zeros(60)

filteredReds = np.zeros(50,dtype=np.uint16)
filteredGreens = np.zeros(50,dtype=np.uint16)
filteredBlues = np.zeros(50,dtype=np.uint16)
filteredBright = np.zeros(50)

# moving average filter
def MAF(data, n):
    filteredData = []
    n = int (n/2)
    for i in range(n,len(data)-n): 
        # take the mean of the window
        filteredData.append(np.mean(data[i-n:i+n]))
    return filteredData


while True:
    # wait for a newline from the computer
    input()

    cam.capture(bitmap)
    #bitmap[10,10] = 0 # set a pixel to black
    #bitmap[10,20] = 0 # [Y,X], [0,0] is bottom left

    
    row = 40 # which row to send to the computer
    # draw a red dot above the row, in the middle
    bitmap[row+1,30] = 0x3F<<5

    for i in range(0,60):
        print(str(i)+" "+str(bitmap[row,i]))

    # calculate the colors
    for i in range(0,60):
        reds[i] = ((bitmap[row,i]>>5)&0x3F)/0x3F*100
        greens[i] = ((bitmap[row,i])&0x1F)/0x1F*100
        blues[i] = (bitmap[row,i]>>11)/0x1F*100
        bright[i] = reds[i]+greens[i]+blues[i]


    # lowpass filter the data
    n = 8
    filteredReds = MAF(reds,n)
    filteredGreens = MAF(greens,n)
    filteredBlues = MAF(blues,n)
    filteredBright = MAF(bright,n)

    # threshold to try to find the line
    com = 0
    mass = 1
    for i in range(0,len(filteredReds)):
        if (filteredReds[i]>70 and filteredBright[i]>70):
            bitmap[row,i+5] = 0xFFFF
        if (filteredBlues[i]>70):
            bitmap[row,i+5] = 0xFFFF
        else:
            bitmap[row,i+5] = 0x0000
        
        com += i*bitmap[row,i+5]
        mass += bitmap[row,i+5]
    
    
    # draw a dot at the center of mass
    line = round(com/mass) + n
     #write the midpoint to pic over uart

    #print(line)
    for i in range(-1,2):
        for j in range(-1,2):
            if (line+j>=0 and line+j<60):
                bitmap[row+i,line+j] = 0xF800



    bitmap.dirty() # updae the image on the screen
    display.refresh(minimum_frames_per_second=0)
    t1 = time.monotonic_ns()
    #print("fps", 1e9 / (t1 - t0))
    t0 = t1