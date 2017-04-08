#imports Tkinter for creating the window, serial, time, array, math, and numpy
import Tkinter
from Tkinter import messagebox
from Tkinter import *
import serial
import time
from array import array
import math
import numpy as np
#imports matplotlib and configures the backend
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

#creates arrays for storing scanning functions
deg = []
ir = []
sonar = []

#potential location of the goal
goalX = 3
goalY = 7
#stores current location
currentPositionX = 1
currentPositionY = 1
#stores angle for display
angleVar = 0
#Creates serial connection
ser = serial.Serial('/dev/tty.ElementSerial-ElementSe', 57600, timeout = .05)
#Creates window
top = Tkinter.Tk()
top.configure(background="gray")

#Echos back message (for testing)
def hello():
   messagebox.showinfo("Say Hello", "Hello World")
   
#Gets keyboard input from the user
def getch( ): 
	import sys, tty, termios 
	fd = sys.stdin.fileno( ) 
	old_settings = termios.tcgetattr(fd) 
	try: 
		tty.setraw(fd) 
		ch = sys.stdin.read(1) 
	finally: 
		termios.tcsetattr(fd, termios.TCSADRAIN, old_settings) 
	return ch
	
#Recieves data from the robot and displays it
def scan():
	#default position data
	position = 0
	positionS = 0
	distance = 0
	x = 0
	y = 0
	
	currentPositionX = 1
	currentPositionY = 1
	
	direction = "N"
	#Writes space to the robot to start scan
	ser.write(" ".encode())
	
	while(1):
		#Gets input from the robot
		input = ser.readline().decode().strip('\r\n')
		#Checks to see if the data starts with DD meaning it is the ending degree of an object
		if(input.startswith("DD")):
			#Prints data, stores transmitted number and reads new input
			print(input)
			position = float(input.strip("DD,"))
			input = ser.readline().decode().strip('\r\n')
		#Checks to see if input is the starting degree, prints that data, stores the value and reads data again
		if(input.startswith("Start")):
			print(input)
			positionS = float(input.strip("Start,"))
			position = (position + positionS) / 2
			input = ser.readline().decode().strip('\r\n')
		#Checks to see if input is the distance to the object, prints that data, stores the value and reads data again
		if(input.startswith('DIS')):
			print(input)
			distance = float(input.strip('DIS,'))
			#Converts CM to M
			distance = distance / 100
			#Prints distance and angle to object
			print(position, distance)
			#Converts degrees to radians
			position = math.radians(position)
			#Translates polar to cart
			x = distance * np.cos(position)
			y = distance * np.sin(position)
			#Prints x, y cords
			print(x, y)	
			
			if(direction == 'N'):
				print("North")
				x = x + currentPositionX
				y = y + currentPositionY
			#Plots objects to graph and redraws the graph
			movementA.plot(x, y, 'bo') 	
			movementcanvas.draw()
		#If END is transmitted this loop will end			
		if(input == "END"):
			break
		
#Updates data on the graph	
def update_data():
	#deletes data in arrays
	del deg[:]
	del ir[:]
	del sonar[:]
	
	obj = []
	distance = 0;
	
	while True:
		#Reads data from robot
		data = ser.readline().decode().strip('\r\n')
		
		print(data)
		#filters and stores data for the ui
		if(data.startswith("BL")):
			bumperLeft.set(data.strip('BL,'))
		
		if(data.startswith("BR")):
			bumperRight.set(data.strip('BR,'))
		
		if(data.startswith("CL")):
			cliffLeft.set(data.strip('CL,'))
		
		if(data.startswith("CR")):
			cliffRight.set(data.strip('CR,'))
		
		if(data.startswith("LL")):
			lineLeft.set(data.strip('LL,'))
		
		if(data.startswith("LR")):
			lineRight.set(data.strip('LR,'))
			
		if(data.startswith("S")):
			speedSetting.set(data.strip('S,'))
			
		if(data.startswith("DEG")):
			deg.append(data.strip('DEG,'))
			
		if(data.startswith("IR")):
			ir.append(data.strip('IR,')) #Calibrate Sensor
				
		if(data.startswith("SON")):
			sonar.append(data.strip('SON,'))
			
		if(data.startswith("SD")):
			obj.append(data.strip('SD,'))
			data = ser.readline().decode().strip('\r\n')
			obj.append(data.strip('ED,'))
			data = ser.readline().decode().strip('\r\n')
			distance = data.strip('DIS,')
			
			a.plot(obj, distance)
			
			print(obj)
			
		if(data.startswith("D,")):
			print("stuff")
			
		if(data == "END"):
			break	
	#clears ir graph
	a.cla()
	
	#plots sensor data
	a.plot(deg, sonar)
	a.plot(deg, ir)
	
	a.set_rmax(150)
	a.set_rmax(150)
	#redraws graphs
	ircanvas.draw()
	sonarcanvas.draw()

#Prints data in warnings to button on ui
def check_warning(input):
	print(input.strip('W,'))
	warningText.set(input.strip('W,'))
	
#Sends data to the robot and reads input to update warnings or the angle change
def inputProcessing():
	global cliffLeft
	global angleVar
	cliffLeft.set("1")
	
	print('Input data')
	#Gets keyboard input	
	while True:
		yam = getch()
		if yam == " ":
			break
		#Writes keyboard input to robot
		ser.write(yam.encode())
		input = ser.readline().decode().strip('\r\n')
		print(input)
		#Reads and deciphers recieved data
		if(input.startswith("W")):
			check_warning(input)
			break
			
		if(input.startswith("anglen")):
			print(input)
			tempAngle = (int(input.strip('anglen,')))
			if(tempAngle < 0):
				tempAngle + 256
			print("Temp", tempAngle)
			angleVar = angleVar + tempAngle
			
		if(input.startswith("anglep")):
			print(input)
			tempAngle = (int(input.strip('anglep,')))
			if(tempAngle < 0):
				tempAngle + 256
			print("Temp", tempAngle)
			angleVar = angleVar + tempAngle
		
		if(input.startswith("data")):
			print(input)
			
		angleData.set(angleVar)
		
#Reads test data in from a local file
def readFile():
	i = 0
	del deg[:]
	del ir[:]
	del sonar[:]
	
	sonarX = []
	sonarY = []
	
	with open("deg_ir_sonar.txt") as fileIn:
		for line in fileIn:
			tempdeg, tempir, tempsonar = [int(n) for n in line.split()]
			deg.append(math.radians(tempdeg))
			ir.append(tempir)
			sonar.append(tempsonar)
			i += 1
	for j in range(0,181):
		degrees = math.radians(deg[j])

	k = 0
	
	for x in sonar:
		sonarX.append(x * math.cos(math.radians(deg[k])))
		sonarY.append(x * math.sin(math.radians(deg[k])))
		
		k += 1
		
	[math.radians(x) for x in deg]
	
	a.plot(deg, sonar)
	
	a.plot(deg, ir)
	
	z = 0
	
	a.set_rmax(150)
	a.set_rmax(150)
	
	ircanvas.draw()
	sonarcanvas.draw()
	
#Rotates data 180 degrees for the full scan	
def translateData(degrees):
	#Add degrees to data
	print("Translating")
	
	i = 0
	
	for x in deg:
		deg[i] = x + math.pi
		i += 1
	
	sonarA.cla()
	a.cla()
	
	sonarA.plot(deg, sonar)
	a.plot(deg, ir)
	
	ircanvas.draw()
	sonarcanvas.draw()

#360 ir/sonar scan (not implemented)
def fullscan():
	readFile()
	translateData(180)
	readFile()
  
#Resets warning text on UI  
def resetWarning():
	warning.text="Warning"
	
#Clears graph
def clear():
	movementA.cla()
	movementA.plot([goalX, goalX + 3], [goalY, goalY], 'ro');
	movementA.set_ylim([0, 14])
	movementA.set_xlim([0, 9])
	movementA.grid(True)
	ticksY = np.arange(0, 14, 2)
	ticksX = np.arange(0, 9, 2)
	movementA.set_xticks(ticksX)
	movementA.set_yticks(ticksY)
	movementA.plot(currentPositionX,currentPositionX, 'ro')
	movementcanvas.draw()
	ser.flush()
	
#Creats string variables for UI	
cliffLeft = StringVar()
cliffRight = StringVar()
bumperLeft = StringVar()
bumperRight = StringVar()
lineLeft = StringVar()
lineRight = StringVar()
speedSetting = StringVar()
warningText = StringVar()
angleData = StringVar()

#Creates frame elements for UI
page = Frame(top, bg="Gray")
page.pack( fill=BOTH)

sensors = Frame(top, bg="Gray")
sensors.pack(side=TOP)

irgraph = Frame(top, bg="Blue")
irgraph.pack(side=LEFT)

sonargraph = Frame(top, bg="Red")
sonargraph.pack(side=LEFT)

movementgraph = Frame(top, bg="RED")
movementgraph.pack(side=LEFT)

#Creates label elements for UI
title = Label(sensors, text="Sensors", bg="Gray")
title.grid(row=0, column=0)

bumper = Label(sensors, text="Bumper", bg="Gray")
bumper.grid(row=1, column=0)

cliff = Label(sensors, text="Cliff", bg="Gray")
cliff.grid(row=2, column=0)

line = Label(sensors, text="Line", justify="left", bg="Gray")
line.grid(row=3, column=0)

left = Label(sensors, text="Left", bg="Gray")
left.grid(row=0, column=1)

leftBumper = Label(sensors, textvariable=bumperLeft, bg="Gray")
leftBumper.grid(row=1, column=1)

leftCliff = Label(sensors, textvariable=cliffLeft, bg="Gray")
leftCliff.grid(row=2, column=1)

leftLine = Label(sensors, textvariable=lineLeft, bg="Gray")
leftLine.grid(row=3, column=1)

right = Label(sensors, text="Right", bg="Gray")
right.grid(row=0, column=2)

rightBumper = Label(sensors, textvariable=bumperRight, bg="Gray")
rightBumper.grid(row=1, column=2)

rightCliff = Label(sensors, textvariable=cliffRight, bg="Gray")
rightCliff.grid(row=2, column=2)

rightLine = Label(sensors, textvariable=lineRight, bg="Gray")
rightLine.grid(row=3, column=2)

empty = Label(sensors, text="    ", bg="Gray")
empty.grid(row=0, column=3)

speedLabel = Label(sensors, text="Speed", bg="Gray")
speedLabel.grid(row=0, column=4)

speedOutput = Label(sensors, textvariable=speedSetting, bg="Gray")
speedOutput.grid(row=0, column=5)

angleLabel = Label(sensors, textvariable=angleData)
angleLabel.grid(row=0, column=6)

#Creates three graphs for ir sonar and movement
f = Figure(figsize=(5,5), dpi=100)
a = f.add_subplot(111, polar=True)
a.set_rmax(150)

ircanvas = FigureCanvasTkAgg(f, irgraph)
ircanvas.show()
ircanvas.get_tk_widget().pack(side=BOTTOM, fill=BOTH, expand=True)

sonarF = Figure(figsize=(5,5), dpi=100)
sonarA = sonarF.add_subplot(111, polar=True)
sonarA.set_rmax(150)
sonarA.grid(True)

sonarcanvas = FigureCanvasTkAgg(sonarF, irgraph)
sonarcanvas.show()
sonarcanvas.get_tk_widget().pack(side=BOTTOM, fill=BOTH, expand=True)

movementF = Figure(figsize=(3,5), dpi=200)
movementA = movementF.add_subplot(111)
movementA.plot([goalX, goalX + 3], [goalY, goalY], 'ro');
movementA.set_ylim([0, 14])
movementA.set_xlim([0, 9])
movementA.grid(True)

ticksY = np.arange(0, 14, 2)
ticksX = np.arange(0, 9, 2)
movementA.set_xticks(ticksX)
movementA.set_yticks(ticksY)

movementcanvas = FigureCanvasTkAgg(movementF, movementgraph)

movementcanvas.show()
movementcanvas.get_tk_widget().pack(side=BOTTOM, fill=BOTH, expand=True)

#Creates UI buttons
warning = Tkinter.Button(sensors, textvariable=warningText, command = resetWarning, bg = "Gray")
warning.grid(row=15, column=4)

scan = Tkinter.Button(sensors, text = "Scan", command = scan, bg="Gray")
scan.grid(row=20,column=4)

fullscan = Tkinter.Button(sensors, text = "Full Scan", command = fullscan, bg="Gray")
fullscan.grid(row=20,column=5)

move = Tkinter.Button(sensors, text = "Move Robot", command = inputProcessing, bg="Gray")
move.grid(row=20, column=6)

Findlocation = Tkinter.Button(sensors, text="Find Location", command = findlocation)
Findlocation.grid(row=20, column=7)

B1 = Tkinter.Button(sensors, text = "Nicus", command = top.quit, bg="Gray")
B1.grid(row=20,column=9)

clearbutton = Tkinter.Button(sensors, text = "Clear", command = clear, bg = "Gray")
clearbutton.grid(row=20, column = 8)

#Runs user interface
top.mainloop()
