import Jetson.GPIO as GPIO
import nanocamera
import os
import cv2
from time import sleep 

def takepicture(filename):
	if(camera.isReady()):
		frame = camera.read()
		cv2.imwrite(filename,frame)
def update_class1():
	n_class1 = len(os.listdir("./class1"))
	tmp_c1_name = "./class1/class1_"+str(n_class1)+".jpg"
	takepicture(tmp_c1_name)
	sleep(3.0)
def update_class2():
	n_class2 = len(os.listdir("./class2"))
	tmp_c2_name = "./class2/class2_"+str(n_class2)+".jpg"
	takepicture(tmp_c2_name)
	sleep(3.0)


camera = nanocamera.Camera(flip=0,width=1280,height=800,fps=30)
sleep(2.0)
GPIO.setmode(GPIO.BOARD)
GPIO.setup(38,GPIO.IN)
GPIO.setup(37,GPIO.IN)
while(True):
	if(GPIO.input(38)):
		update_class1()
	if(GPIO.input(37)):
		update_class2()





