import picamera
import PIL
from PIL import Image
from PIL import ImageChops
from time import sleep
import numpy
import os
import math
import RPi.GPIO as GPIO

def prepare_background_mask_photo(photo_name,background_photo_name,output_file,channel,cutoff_level = 30):
    target = Image.open(photo_name)
    mask = numpy.zeros((target.size[1],target.size[0],3),float)
    mask_tmp = numpy.zeros((target.size[1],target.size[0],3),float)
    target_array = numpy.array(Image.open(photo_name),dtype=numpy.float)
    background_array = numpy.array(Image.open(background_photo_name),dtype=numpy.float)
    target_array_tmp = target_array-background_array
    print("subtracting background...")
    for kk in range(target.size[1]):
        if(kk%200 == 0):
            print(kk/target.size[1])
        for hh in range(target.size[0]):
            if(abs(target_array_tmp[kk][hh][channel]) > cutoff_level):
                mask_tmp[kk][hh][channel] = 255
                mask[kk][hh][channel] = 1
                
    mask_tmp = numpy.array(numpy.round(mask_tmp),dtype=numpy.uint8)
    mask = numpy.array(numpy.round(mask),dtype=numpy.uint8)
    print("Mask photo chechsum:")
    print(numpy.sum(mask))
    mask_tmp_img = Image.fromarray(mask_tmp,mode="RGB")
    mask_tmp_img.save(output_file+".jpg")
    mask_bmp_img = Image.fromarray(mask,mode="RGB")
    mask_bmp_img.save(output_file+".bmp")

def check_photo_difference(photo_name,reference_photo,mask,channel):
    target_array = numpy.array(Image.open(photo_name),dtype=numpy.float)
    mask_array = numpy.array(Image.open(mask+".bmp"),dtype=numpy.float)

    ref_array = numpy.array(Image.open(reference_photo),dtype=numpy.float)

    target_array_tmp = target_array-ref_array
    target_array = numpy.multiply(numpy.absolute(target_array_tmp),mask_array)
    ref_comp = Image.fromarray(target_array_tmp,mode="RGB")
    ref_comp.save("compare_"+photo_name)

    target_array = numpy.array(numpy.round(target_array),dtype=numpy.uint8)
    tmp_photo = Image.fromarray(target_array,mode="RGB")
    tmp_photo.save("processed_"+str(channel)+photo_name)
    return(numpy.sum(target_array))

def prepare_mask_from_photo(input_file,output_file,channel,color_code_R,color_code_G,color_code_B):
    target = Image.open(input_file)
    mask_tmp = numpy.zeros((target.size[1],target.size[0],3),float)
    mask_debug = numpy.zeros((target.size[1],target.size[0],3),float)
    target_array = numpy.array(Image.open(input_file),dtype=numpy.float)
    for kk in range(target.size[1]):
        for hh in range(target.size[0]):
            if(target_array[kk][hh][0] == color_code_R and target_array[kk][hh][1] == color_code_G and target_array[kk][hh][2] == color_code_B):
                mask_tmp[kk][hh][channel] = 1
                mask_debug[kk][hh][channel] = 255

    mask_tmp = numpy.array(numpy.round(mask_tmp),dtype=numpy.uint8)
    mask_debug = numpy.array(numpy.round(mask_debug),dtype=numpy.uint8)
    mask_debug_img = Image.fromarray(mask_debug,mode="RGB")
    mask_photo = Image.fromarray(mask_tmp,mode="RGB")
    mask_photo.save(output_file+".bmp");
    mask_debug_img.save(output_file+".jpg")
def prepare_noise_calibration_photos(nsamples):
    print("Prepare background setup, confirm with enter")
    tmp = input()
    for kk in range(n_background_samples):
        camera.capture("empty_"+str(kk)+".jpg",format="jpeg",use_video_port="True")
        sleep(1)
    for ll in range(n_background_samples):
        print("Prepare "+str(ll)+" full setup and confirm with enter.")
        tmp=input()
        camera.capture("noise_"+str(ll)+".jpg",format="jpeg",use_video_port="True")
        sleep(1)
def calculate_treshhold_level(input_mask,nsamples):
    signal_R = 0;
    signal_B = 0;
    signal_G = 0;
    for kk in range(n_background_samples):
        sig_r = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",input_mask+"_R","R")
        sig_g = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",input_mask+"_G","G")
        sig_b = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",input_mask+"_B","B")
        signal_R = signal_R+sig_r
        signal_G = signal_G+sig_g
        signal_B = signal_B+sig_b
    signal_R_n = signal_R/n_background_samples
    signal_G_n = signal_G/n_background_samples
    signal_B_n = signal_B/n_background_samples
    signal_R = 0;
    signal_B = 0;
    signal_G = 0;
    for kk in range(n_background_samples):
        sig_r = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",input_mask+"_R","R")
        sig_g = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",input_mask+"_G","G")
        sig_b = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",input_mask+"_B","B")
        signal_R = signal_R+sig_r
        signal_G = signal_G+sig_g
        signal_B = signal_B+sig_b
    signal_R_e = signal_R/n_background_samples
    signal_G_e = signal_G/n_background_samples
    signal_B_e = signal_B/n_background_samples
    channel_r_signal = signal_R_e-signal_R_n
    channel_g_signal = signal_G_e-signal_G_n
    channel_b_signal = signal_B_e-signal_B_n
    print("r signal:")
    print(channel_r_signal)
    print("g signal:")
    print(channel_g_signal)
    print("b signal:")
    print(channel_b_signal)
    _signal_treshold = 0
    _prefered_channel = "R"
    if(channel_r_signal > channel_g_signal and channel_r_signal > channel_b_signal):
        _prefered_channel = "R"
        _signal_treshold = signal_R_n + 0.3*channel_r_signal
    if(channel_g_signal > channel_r_signal and channel_g_signal > channel_b_signal):
        _prefered_channel = "G"
        _signal_treshold = signal_G_n + 0.3*channel_g_signal
    if(channel_b_signal > channel_r_signal and channel_b_signal > channel_r_signal):
        _prefered_channel = "B"
        _signal_treshold = signal_B_n + 0.3*channel_b_signal
    print("Treshold:")
    print(_signal_treshold)
    print("selected channel:")
    print(_prefered_channel)
    return(_signal_treshold,_prefered_channel)
 
with picamera.PiCamera() as camera:
    camera.resolution = (1292,944)
    sleep(2)
    print(" 1-set background photo.\n 2 -set reference photo.\n 3 - Prepare mask images automaticaly.\n 4 measure target fluctuations.\n 5 Load manual mask image.\n 6. Run background noise estimation\n 7. Start autonomic watchdog.\n  0 - exit program.\n") 
    ref_photo_exist = True
    background_photo_exists = True
    mask_exist = True
    Ntests = 0
    prefered_channel_1 = "B"
    prefered_channel_2 = "B"
    prefered_channel_3 = "B"
    signal_treshold_1 = 0
    signal_treshold_2 = 0
    signal_treshold_3 = 0
    n_background_samples = 2
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(27, GPIO.IN)
    GPIO.setup(23, GPIO.OUT)
    GPIO.setup(24, GPIO.OUT)
    GPIO.output(23,1)
    GPIO.output(24,1)
    while(True):
        command = input()
        if(command == '1'):
            camera.capture("background.jpg",format="jpeg",use_video_port="True")
            background_photo_exists = True
            camera.capture("for_manual_mask.bmp",format="bmp")
        elif(command == '2'):
            camera.capture("reference.jpg",format="jpeg",use_video_port="True")
            ref_photo_exist = True
        elif(command == '3'):
            if(ref_photo_exist and background_photo_exists):
                prepare_background_mask_photo("reference.jpg","background.jpg","mask_R",0,25)
                prepare_background_mask_photo("reference.jpg","background.jpg","mask_G",1,25)
                prepare_background_mask_photo("reference.jpg","background.jpg","mask_B",2,25)
                mask_exist = True;
            else:
                print("No reference or background photo detected set it first with \"1\"/\"2\"commands")
        elif(command == '4'):
            if(ref_photo_exist and background_photo_exists and mask_exist):
                camera.capture("tmp.jpg",format="jpeg",use_video_port="True")
                signal_1 = check_photo_difference("tmp.jpg","reference.jpg","mask_1_"+prefered_channel_1,prefered_channel_1)
                signal_2 = check_photo_difference("tmp.jpg","reference.jpg","mask_2_"+prefered_channel_2,prefered_channel_2)
                signal_3 = check_photo_difference("tmp.jpg","reference.jpg","mask_3_"+prefered_channel_3,prefered_channel_3)
                if(signal_1 > signal_treshold_1):
                    print("ERROR DETECTED!")
                    print("SOURCE 1")
                else:
                    print("1 OK")
                if(signal_2 > signal_treshold_2):
                    print("ERROR DETECTED!")
                    print("SOURCE 2")
                else:
                    print("2 OK")
                if(signal_3 > signal_treshold_3):
                    print("ERROR DETECTED!")
                    print("SOURCE 3")
                else:
                    print("3 OK")
            else:
                print("No reference, mask or background photo detected set it first with \"1\"/\"2\"/\"3\"commands")
        elif(command == '5'):
            prepare_mask_from_photo("for_manual_mask.bmp","mask_1_R",0,255,0,0)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_1_G",1,255,0,0)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_1_B",2,255,0,0)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_2_R",0,0,0,255)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_2_G",1,0,0,255)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_2_B",2,0,0,255)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_3_R",0,0,255,0)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_3_G",1,0,255,0)
            prepare_mask_from_photo("for_manual_mask.bmp","mask_3_B",2,0,255,0)
        elif(command == '6'):
            prepare_noise_calibration_photos(3)
            tmp1 = calculate_treshhold_level("mask_1",3)
            tmp2 = calculate_treshhold_level("mask_2",3)
            tmp3 = calculate_treshhold_level("mask_3",3)
            signal_treshold_1 = tmp1[0]
            prefered_channel_1 = tmp1[1]
            signal_treshold_2 = tmp2[0]
            prefered_channel_2 = tmp2[1]
            signal_treshold_3 = tmp3[0]
            prefered_channel_3 = tmp3[1]
        elif(command == '7'):
            state = True
            while(True):
                if(GPIO.input(27) == False and state == False):
                    GPIO.output(23,0)
                    GPIO.output(24,0)
                    print("MEASUREMENT!")
                    camera.capture("tmp.jpg",format="jpeg",use_video_port="True")
                    signal_1 = check_photo_difference("tmp.jpg","reference.jpg","mask_1_"+prefered_channel_1,prefered_channel_1)
                    signal_2 = check_photo_difference("tmp.jpg","reference.jpg","mask_2_"+prefered_channel_2,prefered_channel_2)
                    signal_3 = check_photo_difference("tmp.jpg","reference.jpg","mask_3_"+prefered_channel_3,prefered_channel_3)
                    no_error = True
                    if(signal_1 > signal_treshold_1):
                        print("ERROR DETECTED!")
                        print("SOURCE 1")
                        no_error = False
                    else:
                        print("1 OK")
                    if(signal_2 > signal_treshold_2):
                        print("ERROR DETECTED!")
                        print("SOURCE 2")
                        no_error = False
                    else:
                        print("2 OK")
                    if(signal_3 > signal_treshold_3):
                        print("ERROR DETECTED!")
                        print("SOURCE 3")
                        no_error = False
                    else:
                        print("3 OK")
                    if(no_error):
                        GPIO.output(23,0)
                        GPIO.output(24,1)
                    else:
                        GPIO.output(23,1)
                        GPIO.output(24,0)
                    state = True
                else:
                    if(GPIO.input(27) == True):
                        state = False
        elif(command == '0'):
            exit()
        print("ok " + str(command)+"\n")
    
    