# Program to control camera settings, microscope stepper motors, and LED illumination from keyboard
# Camera: Pi camera v2
# Enable the camera: Preferences --> Raspberry Pi Configuration --> Interfaces --> Camera --> Enable --> Reboot
# Increase GPU memory (Raspberry Pi 4, max. camera resolution): Preferences --> Raspberry Pi Configuration --> Performance --> GPU memory --> Change from 128 to 144 --> Reboot
# Arduino USB communication baud rate: 57600

# The code can also be used to control the camera settings of any Pi camera without having a microscope. 
# In this case, set the variable KeyboardControl = False so that the code does not expect an Arduino board connected to the USB.

## IMPORTANT! For Raspberry Pi zero, and Raspberry Pi 3, reduce the resolution by setting HighResolution = False

HighResolution = True # True: (3280, 2464), False: (1920, 1080)

KeyboardControl = True # set to "False" if the Arduino is not connected to the Raspberry Pi via USB 

import sys
import easygui
import serial
import time
from datetime import datetime
from pynput.keyboard import Key, KeyCode, Listener

### camera

from picamera import PiCamera
camera = PiCamera()
brightness = 50 #start-up brightness
contrast= 0 #start-up contrast
EV = 0 #start-up exposure compensation
saturation = 0 #start-up saturation

### serial communication with Arduino

if KeyboardControl == True:
        ser = serial.Serial('/dev/ttyACM0',57600) # type "ls /dev/tty*" to the terminal to check the serial port 
        
### predefined motor speeds
slow = 10
medium = 100
fast = 500
speed = medium #start-up speed:medium, Ctrl key changes the speed

zoom = 1.0 #start-up digital zoom factor

LEDintensity = 20 #start-up LED intensity
#will be multiplied by 5 to get 0-100% range in 5% steps to make it compatible with
#the potentiometer on the Arduino joystick controller

filename = "" #default filename prefix
path = "/home/pi/Pictures" #default path

arrowkey_mode = False #Alt key toggles this variable to select between X-Y (start-up) and Rotate-Tilt control
recording_mode = False #Tab key toggles between the photo mode (start-up) and the video mode
recording = False #variable to set during recording

def camera_reset():
    global brightness, contrast, EV, saturation
    brightness = 50 
    camera.brightness = brightness
    contrast= 0
    camera.contrast = contrast
    EV = 0
    saturation = 0
    camera.saturation = saturation
    camera.exposure_compensation = 0
    camera.sensor_mode = 2 # full field of view
    if HighResolution == True:
        camera.resolution = (3280, 2464) # Pi camera v2 max resolution, 8MP
    else:
        camera.resolution = (1920, 1080) # reduced resolution, Full HD or (2592 x 1944) for the 5MP camera v1
    camera.rotation=180
    camera.annotate_text_size = 100
    camera.annotate_text = ""
    camera.iso = 0
    camera.shutter_speed = 0
    camera.framerate = 30
    camera.exposure_mode = 'auto'
    camera.awb_mode = 'auto'
    #camera.preview_fullscreen = False # optional
    #camera.preview_window = (0, 50, 1280, 960) #optional
    camera.start_preview()
   

def shortcuts(): # keyboard shortcuts
    camera.preview_alpha=0 
    easygui.msgbox("F1: keyboard shortcuts\
                       \n----------------------------------------------------------\
                       \nTab: toggle between photo and video modes\
                       \nP: start camera preview, p: stop camera preview\
                       \n----------------------------------------------------------\
                       \nB: increase brightness, b: decrease brightness\
                       \nC: increase contrast, c: decrease contrast\
                       \nV: increase EV, v: decrease EV\
                       \nS: increase saturation, s: decrease saturation\
                       \n----------------------------------------------------------\
                       \n+: digital zoom in, -: digital zoom out\
                       \n----------------------------------------------------------\
                       \ni: ISO, e: exposure time, r: framerate\
                       \nE: exposure mode, W: white-balance mode\
                       \n----------------------------------------------------------\
                       \n0: reset camera settings\
                       \n----------------------------------------------------------\
                       \nF: select folder, f: enter filename prefix (optional)\
                       \n----------------------------------------------------------\
                       \nEnter: save image or start\stop video depending on the mode\
                       \n----------------------------------------------------------\
                       \nArrow keys: X-Y or Rotate-Tilt\
                       \nPage up and Page down: Z up and down\
                       \nHome and End: Camera up and down\
                       \n----------------------------------------------------------\
                       \nCtrl: change motor speed (3 predefined speeds)\
                       \nAlt: toggle between X-Y and Rotate-Tilt\
                       \n----------------------------------------------------------\
                       \nEsc: exit","Keyboard shortcuts")
    camera.preview_alpha=255
    

shortcuts() #display the keyboard shortcuts at the beginning
camera_reset() # start the camera preview

def on_press(key):
    global brightness, contrast, EV, saturation, zoom
    global speed, LEDintensity
    global arrowkey_mode, path, filename, recording_mode

### F1: keyboard shortcuts
    
    if key == Key.f1:
        shortcuts()

######## CAMERA
        
### Camera preview modes, video or photo         
   
    if key == Key.tab:
        if recording_mode == False:
            camera.resolution = (1920, 1080)
            camera.annotate_text = "Video mode"
            recording_mode = True
        elif recording_mode == True:
            if HighResolution == True:
                camera.resolution = (3280, 2464) # Pi camera v2 max resolution, 8MP
            else:
                camera.resolution = (1920, 1080) # reduced resolution, Full HD
            camera.annotate_text = "Photo mode"
            recording_mode = False

### Reset camera settings
    if key == KeyCode(char='0'):
        camera_reset()
        
### digital zoom
    
    if key == KeyCode(char='+'):
        if zoom > 0.2:
            zoom = zoom - 0.1
            camera.zoom = (0,0,zoom,zoom)
            annotate_text = "Digital zoom:" + str(round(1/zoom,2)) + "X"
            camera.annotate_text = annotate_text  

    if key == KeyCode(char='-'):
        if zoom < 1.0: 
            zoom = zoom + 0.1
            camera.zoom = (0,0,zoom,zoom)
            annotate_text = "Digital zoom:" + str(round(1/zoom,2)) + "X"
            camera.annotate_text = annotate_text
            
### start and stop camera preview
 
    if key == KeyCode(char='P'):
        camera.start_preview()
        
    if key == KeyCode(char='p'):
        camera.stop_preview()

### brightness
        
    if key == KeyCode(char='B'):
        if brightness !=100:
            brightness += 5
            camera.brightness = brightness
            annotate_text = "Brightness:" + str(brightness) + "%"
            camera.annotate_text = annotate_text
            
    if key == KeyCode(char='b'):
        if brightness !=0:
            brightness -= 5
            camera.brightness = brightness
            annotate_text = "Brightness:" + str(brightness) + "%"
            camera.annotate_text = annotate_text
            
### contrast
            
    if key == KeyCode(char='C'):
        if contrast !=100:
            contrast += 5
            camera.contrast = contrast
            annotate_text = "Contrast:" + str(contrast) + "%"
            camera.annotate_text = annotate_text
    if key == KeyCode(char='c'):
        if contrast !=-100:
            contrast -= 5
            camera.contrast = contrast
            annotate_text = "Contrast:" + str(contrast) + "%"
            camera.annotate_text = annotate_text
            
### EV compensation
            
    if key == KeyCode(char='V'):
        if EV !=25:
            EV += 1
            camera.exposure_compensation = EV
            annotate_text = "EV:" + str(EV)
            camera.annotate_text = annotate_text
    if key == KeyCode(char='v'):
        if EV !=-25:
            EV -= 1
            camera.exposure_compensation = EV
            annotate_text = "EV:" + str(EV)
            camera.annotate_text = annotate_text

### saturation
            
    if key == KeyCode(char='S'):
        if saturation !=100:
            saturation += 5
            camera.saturation = saturation
            annotate_text = "Saturation:" + str(saturation) + "%"
            camera.annotate_text = annotate_text
    if key == KeyCode(char='s'):
        if saturation !=-100:
            saturation -= 5
            camera.saturation = saturation
            annotate_text = "Saturation:" + str(saturation) + "%"
            camera.annotate_text = annotate_text

### ISO
            
    if key == KeyCode(char='i'):
        camera.preview_alpha=0
        ISO_choices = ["0","100","200","320","400","500","640","800"]
        ISO_selected = easygui.choicebox("Select ISO (default: 0 for auto)", "ISO", ISO_choices)
        if ISO_selected is not None:
            camera.iso = int(ISO_selected)
        camera.preview_alpha=255

### white balance
        
    if key == KeyCode(char='W'):
        camera.preview_alpha=0
        WB_choices = ["off","auto","sunlight","cloudy","shade","tungsten","fluorescent","flash","horizon"]
        WB_selected = easygui.choicebox("Select white balance mode (default: auto)", "White Balance", WB_choices)
        if WB_selected is not None:
            camera.awb_mode = WB_selected
        if WB_selected == "off":
            WB_gain = easygui.enterbox("White balance gain - typical values between 0.9 and 1.9", "White balance gain", "1.0")
            if WB_gain is not None:
                camera.awb_gains = float(WB_gain)
        camera.preview_alpha=255

### frame rate
        
    if key == KeyCode(char='r'):
        camera.preview_alpha=0
        Framerate_choices = ["30","20", "15","10","5","2","1", "0.5", "0.1", "manual"]
        Framerate_selected = easygui.choicebox("Select framerate","Framerate", Framerate_choices)
        if Framerate_selected == "manual":
            Framerate_selected = easygui.enterbox("Framerate", "Input", "")
        if Framerate_selected is not None:
            camera.framerate = float(Framerate_selected)
        camera.preview_alpha=255

### exposure time (shutter speed)
        
    if key == KeyCode(char='e'):
        camera.preview_alpha=0
        Exposure_choices = ["0","10","20","50","100","200", "500", "1000", "manual"]
        Exposure_selected = easygui.choicebox("Select exposure time in ms (default: 0, auto)\n\
Maximum exposure time is determined by the framerate","Exposure time (shutter speed)", Exposure_choices)
        if Exposure_selected == "manual":
            Exposure_selected = easygui.enterbox("Exposure time (ms)", "Input", "")
        if Exposure_selected is not None:
            camera.shutter_speed = int(Exposure_selected)*1000
        camera.preview_alpha=255

### exposure modes
            
    if key == KeyCode(char='E'):
        camera.preview_alpha=0
        ExposureMode_choices = ["off","auto","night","nightpreview","backlight","spotlight"]
        ExposureMode_selected = easygui.choicebox("Exposure mode", "Exposure mode", ExposureMode_choices)
        if ExposureMode_selected is not None:
            camera.exposure_mode = ExposureMode_selected
        camera.preview_alpha=255

### select folder and set filename prefix
    
    if key == KeyCode(char='F'):
        camera.preview_alpha=0
        path = easygui.diropenbox()
        camera.preview_alpha=255
    if key == KeyCode(char='f'):
        camera.preview_alpha=0
        filename = easygui.enterbox("Filename prefix", "Input", "")
        camera.preview_alpha=255
        
######## MOTORIZED STAGE

### Ctrl: change motor speed
    
    if key == Key.ctrl_l or key == Key.ctrl_r:
        if speed == slow:
            speed = medium
            camera.annotate_text = "Motor speed = medium"
        elif speed == medium:
            speed = fast
            camera.annotate_text = "Motor speed = fast"
        elif speed == fast:
            speed = slow
            camera.annotate_text = "Motor speed = slow"
            
### keyboard arrow keys, Alt key toggles between X-Y and Rotate-Tilt
### send Arduino a string to control the motors, e.g. "-100X," or "500Y,"
### the integer defines the direction and the speed of the motor, the letter selects the motor
            
    if key == Key.alt_l or key == Key.alt_r:
        if arrowkey_mode == True:
            arrowkey_mode = False
            camera.annotate_text = "X-Y"
        elif arrowkey_mode == False:
            arrowkey_mode = True
            camera.annotate_text = "Rotate-Tilt"
            
    if key == Key.left:
        if arrowkey_mode == False:
            data = str(speed*-1) + "X,"
        else:
            data = str(speed*-1) + "R,"
        ser.write(data.encode())
            
    if key == Key.right:
        if arrowkey_mode == False:
            data = str(speed) + "X,"
        else:
            data = str(speed) + "R,"
        ser.write(data.encode())
      
    if key == Key.down:
        if arrowkey_mode == False:
            data = str(speed*-1) + "Y,"
        else:
            data = str(speed*-1) + "T,"
        ser.write(data.encode())
            
    if key == Key.up:
        if arrowkey_mode == False:
            data = str(speed) + "Y,"
        else:
            data = str(speed) + "T,"
        ser.write(data.encode())
        
### Z up and down, Page up and Page down
            
    if key == Key.page_up: 
        data = str(speed) + "Z,"
        ser.write(data.encode())
        
    if key == Key.page_down:
        data = str(speed*-1) + "Z,"
        ser.write(data.encode())

### Camera up and down, Home and End
        
    if key == Key.home: 
        data = str(speed) + "C,"
        ser.write(data.encode())
        
    if key == Key.end: 
        data = str(speed*-1) + "C,"
        ser.write(data.encode())
        
######## LED

### LED intensity, max. value in the data is set to 20 to make it compatible with the joystick
### PWM of the LED driver works in the opposite direction
### Intensity is controlled in 5% increments
        
    if key == KeyCode(char='L'):
        if LEDintensity !=20:
            LEDintensity += 1
            data = str(20-LEDintensity) + "L,"
            ser.write(data.encode())
            annotate_text = "LED intensity:" + str(LEDintensity*5) + "%"
            camera.annotate_text = annotate_text
    if key == KeyCode(char='l'):
        if LEDintensity !=0:
            LEDintensity -= 1
            data = str(20-LEDintensity) + "L,"
            ser.write(data.encode())
            annotate_text = "LED intensity:" + str(LEDintensity*5) + "%"
            camera.annotate_text = annotate_text

    
def on_release(key):
    global path, filename, recording
  
### Esc: exit
    
    if key == Key.esc:
        sys.exit()

### Enter: save image

    if key == Key.enter:
        camera.annotate_text = ""
        now = datetime.now()
        if recording_mode == False: #photo mode
            now = datetime.now()
            output = path+"/"+ filename+now.strftime("%d-%m-%H%M%S.jpg")   
            time.sleep(1)
            camera.capture(output, quality=100)
            camera.annotate_text = "Photo saved"
        if recording_mode == True: #video mode
            if recording == False:
                output = path+"/"+ filename+now.strftime("%d-%m-%H%M%S.h264")
                camera.annotate_text = "Recording will start in 2 seconds..."
                time.sleep(2)
                camera.annotate_text = ""
                camera.start_recording(output)
                recording = True
            elif recording == True:
                camera.stop_recording()
                camera.annotate_text = "Recording stopped"
                recording == False
        
### Stop motors when key is released

    if key == Key.up or key == Key.down or key == Key.left or key == Key.right\
       or key == Key.page_up or key == Key.page_down or key == Key.home\
       or key == Key.end:
        ser.write(b"0O,") ## send to Arduino "0O," to stop the motors
         
with Listener(on_press=on_press,on_release=on_release) as listener:
    listener.join()
    




