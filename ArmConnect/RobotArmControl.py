import time

import serial

from guizero import App, Text, PushButton, Slider, TextBox


connection = serial.Serial('COM4', 9600)  # open serial port

#Stop function to stop the robot from moving
def stop():
    connection.write(bytes("END", 'UTF-8'))
    print("Stop")

#Reset Button to return to the default location
def reset():
    print("Reset")
    connection.write(bytes("reset", 'UTF-8'))


#Creating and sending the Serial message for the motors read in as a string
def SendSerialMesssage():
    motorValues = str(format(float(Motor0Slider.value), ".2f"))
    motorValues += "/"
    motorValues += str(format(float(Motor1Slider.value), ".2f"))
    motorValues += "/"
    motorValues += str(format(float(Motor2Slider.value), ".2f"))
    motorValues += "/"
    motorValues += str(format(float(Motor3Slider.value), ".2f"))
    motorValues += "/"
    motorValues += str(format(float(Motor4Slider.value), ".2f"))
    connection.write(bytes(motorValues, 'UTF-8'))
    time.sleep(3)
    print(motorValues)



#GUI
app = App(title="Robot Controller")

#Motor 1
Motor0Text = Text(app, text="Motor 1")
Motor0Slider = Slider(app, start=-180, end=180)

#Motor 2
Motor1Text = Text(app, text="Motor 2")
Motor1Slider = Slider(app, start=-180, end=180)

#Motor 3
Motor2Text = Text(app, text="Motor 3")
Motor2Slider = Slider(app, start=-180, end=180)

#Motor 4
Motor3Text = Text(app, text="Motor 4")
Motor3Slider = Slider(app, start=-180, end=180)

#Motor 5
Motor4Text = Text(app, text="Motor 5")
Motor4Slider = Slider(app, start=-180, end=180)

#3 buttons
SendCommand = PushButton(app, command=SendSerialMesssage,  text="Send Control")
resetButton = PushButton(app, command=reset, text= "Reset")
StopButton = PushButton(app,command=stop, text="Stop")
app.display()

 
