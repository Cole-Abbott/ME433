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
while True:
    cam.capture(bitmap)
   
    # colors:
    #0x1F ->   0b0000000000011111 # all green
    #0x7E0 ->  0b0000011111100000 # all red
    #0xF800 -> 0b1111100000000000 # all blue
    
    row = 40 # which row to send to the computer
    # draw a red dot above the row, in the middle
    bitmap[row+1,30] = 0x3F<<5
   
    # calculate the colors
    for i in range(0,60):
        reds[i] = ((bitmap[row,i]>>5)&0x3F)/0x3F*100 + ((bitmap[row+1,i]>>5)&0x3F)/0x3F*100 + ((bitmap[row-1,i]>>5)&0x3F)/0x3F*100
        greens[i] = ((bitmap[row,i])&0x1F)/0x1F*100 + ((bitmap[row+1,i])&0x1F)/0x1F*100 + ((bitmap[row-1,i])&0x1F)/0x1F*100
        blues[i] = (bitmap[row,i]>>11)/0x1F*100 + (bitmap[row+1,i]>>11)/0x1F*100 + (bitmap[row-1,i]>>11)/0x1F*100
        bright[i] = reds[i]+greens[i]+blues[i]

    com = 0
    mass = 0
    # threshold to try to find the line
    for i in range(0,60):
        if (reds[i]<35 and greens[i]<30):
            bitmap[row,i] = 0xFFFF
            print("1", end=" ")
        #elif ( reds[i]>90):
        #    bitmap[row,i] = 0xFFFF
        #    print("2", end=" ")
        elif ( reds[i]>70 and greens[i]< 20):
            bitmap[row,i] = 0xFFFF
            print("3", end=" ")
        elif ( reds[i]>70 and greens[i]> 70 and blues[i] < 50):
            bitmap[row,i] = 0xFFFF
            print("4", end=" ")
        else:
            bitmap[row,i] = 0x0000
        
        #compute COM
        com += i*bitmap[row, i]
        mass += bitmap[row, i]

    midPoint = int(com/mass)

    print(midPoint, end=" ")
    for i in range(-1,2):
        for j in range(-1,2):
            if (midPoint+j>=0 and midPoint+j<60):
                bitmap[row+i,midPoint+j] = 0xF800
    
    bitmap.dirty() # updae the image on the screen
    display.refresh(minimum_frames_per_second=0)
    t1 = time.monotonic_ns()
    print("fps", 1e9 / (t1 - t0))
    t0 = t1