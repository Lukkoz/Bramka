import picamera
import PIL
from PIL import Image
from PIL import ImageChops
import time
from time import sleep
import numpy
import os
import subprocess
import signal
import math
import RPi.GPIO as GPIO

def saveImageFormTable(input_table,output_file_name):
    input_table = numpy.array(numpy.round(input_table),dtype=numpy.uint8)
    tmp_img = Image.fromarray(input_table,mode="RGB")
    tmp_img.save(output_file_name)
def channelFromId(id):
    if(id == 0):return([1,0,0])
    if(id == 1):return([0,1,0])
    if(id == 2):return([0,0,1])
    return([1,0,0])

def prepare_background_mask_photo(photo_name,background_photo_name,output_file,channel,cutoff_level = 30):
    target = Image.open(photo_name)
    mask = numpy.zeros((target.size[1],target.size[0],3),float)
    mask_tmp = numpy.zeros((target.size[1],target.size[0],3),float)
    target_array = numpy.array(Image.open(photo_name),dtype=numpy.float)
    background_array = numpy.array(Image.open(background_photo_name),dtype=numpy.float)
    target_array_tmp = abs(target_array-background_array)
    print("subtracting background...")
    for kk in range(target.size[1]):
        if(kk%200 == 0):
            print(kk/target.size[1])
        for hh in range(target.size[0]):
            if(abs(target_array_tmp[kk][hh][channel]) > cutoff_level):
                mask_tmp[kk][hh][channel] = 255
                mask[kk][hh][channel] = 1
                
    saveImageFormTable(mask_tmp,output_file+".jpg")
    saveImageFormTable(mask,output_file+".bmp")

def check_photo_difference(photo_name,reference_photo,mask,channel = [1,1,1]):
    target_array = numpy.array(Image.open(photo_name),dtype=numpy.float)
    ref_array = numpy.array(Image.open(reference_photo),dtype=numpy.float)
    
    mask = mask//channel #filtering the channel

    target_array_tmp = target_array-ref_array
    target_array = numpy.multiply(numpy.absolute(target_array_tmp),mask)
    
    #saveImageFormTable(target_array_tmp,"compare_"+photo_name)
    #saveImageFormTable(target_array,"processed_"+str(channel)+photo_name)

    return(numpy.sum(target_array))

def prepare_mask_from_photo(input_file):
    target = Image.open(input_file)
    mask_tmp = numpy.zeros((target.size[1],target.size[0],3),dtype=numpy.uint8)
    mask_debug = numpy.zeros((target.size[1],target.size[0],3),dtype=numpy.uint8)
    target_array = numpy.array(Image.open(input_file),dtype=numpy.uint8)
    color_config = numpy.array([[255,0,0],[0,0,255],[0,255,0],[255,255,255],[0,255,255],[255,0,255],[255,255,0]])
    output_masks = numpy.zeros((7,target.size[1],target.size[0],3),dtype=numpy.uint8)
    nonZeroMasks = 0
    for kk in range(target.size[1]):
        for hh in range(target.size[0]):
            for jj in range(color_config.shape[0]):
                if(all(target_array[kk][hh] == color_config[jj])):
                    output_masks[jj][kk][hh] = [255,255,255]
    for kk in range(color_config.shape[0]):
        tmp = output_masks[kk]
        tmp_sum = numpy.sum(tmp)
        if(tmp_sum > 0):
            saveImageFormTable(tmp,"mask_"+str(nonZeroMasks)+".bmp")
            nonZeroMasks = nonZeroMasks+1
    return(nonZeroMasks)

def generate_mask_table(nmasks):
    target = Image.open("mask_0.bmp")
    masks = numpy.zeros((nmasks,target.size[1],target.size[0],3),dtype=numpy.uint8)
    for hh in range(nmasks):
        masks[hh] = numpy.array(Image.open("mask_"+str(hh)+".bmp"),dtype=numpy.uint8)
        masks[hh] = masks[hh]//255 #scaling from debug view with colors
        #masks[hh] = masks[hh]//channelFromId(channels[hh])
    return(masks)

def generate_difference_mask_table(nmasks,channels):
    target = Image.open("mask_0.bmp")
    masks = numpy.zeros((nmasks,target.size[1],target.size[0],3),dtype=numpy.uint8)
    for hh in range(nmasks):
        masks[hh] = numpy.array(Image.open("mask_"+str(hh)+".bmp"),dtype=numpy.uint8)
        masks[hh] = masks[hh]//255 #scaling from debug view with colors
        masks[hh] = masks[hh]//channelFromId(channels[hh])
        if(hh != 0):masks[hh] = masks[hh-1]+masks[hh]
    return(masks)

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

def calculate_treshhold_levels(mask_table,n_background_samples):
    file = open("calibration_parameters.txt","w+")
    for ll in range(mask_table.shape[0]):
        signal_R = 0.0
        signal_G = 0.0
        signal_B = 0.0
        for kk in range(n_background_samples):
            sig_r = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[1,0,0])
            sig_g = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[0,1,0])
            sig_b = check_photo_difference("noise_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[0,0,1])
            signal_R = signal_R+sig_r
            signal_G = signal_G+sig_g
            signal_B = signal_B+sig_b
        signal_R_n = signal_R/n_background_samples
        signal_G_n = signal_G/n_background_samples
        signal_B_n = signal_B/n_background_samples
        signal_R = 0.0
        signal_B = 0.0
        signal_G = 0.0
        for kk in range(n_background_samples):
            sig_r = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[1,0,0])
            sig_g = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[0,1,0])
            sig_b = check_photo_difference("empty_"+str(kk)+".jpg","reference.jpg",mask_table[ll],[0,0,1])
            signal_R = signal_R+sig_r
            signal_G = signal_G+sig_g
            signal_B = signal_B+sig_b
        signal_R_e = signal_R/n_background_samples
        signal_G_e = signal_G/n_background_samples
        signal_B_e = signal_B/n_background_samples
        channel_r_signal = signal_R_e-signal_R_n
        channel_g_signal = signal_G_e-signal_G_n
        channel_b_signal = signal_B_e-signal_B_n
        print("Mask "+str(ll)+" signals:")
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
        print("Mask "+str(ll)+":")
        print("Treshold:")
        print(_signal_treshold)
        print("selected channel:")
        print(_prefered_channel)
        file.write(str(ll)+"\t"+str(int(_signal_treshold))+"\t"+_prefered_channel+"\n")
    file.close()

def load_treshold_levels_and_prefered_channels(input_file):
    file = open(input_file,"r+")
    lines = file.readlines()
    output_parametrs = numpy.zeros((len(lines),2),dtype=numpy.uint32)
    for ll in range(len(lines)):
        tmp_line = lines[ll]
        line_data = tmp_line.split("\t")
        output_parametrs[ll][0] = int(line_data[1])
        if(line_data[2].startswith('R')):    
            output_parametrs[ll][1] = 0
        if(line_data[2].startswith('G')):    
            output_parametrs[ll][1] = 1
        if(line_data[2].startswith('B')):    
            output_parametrs[ll][1] = 2
    file.close();
    print(output_parametrs)
    return(output_parametrs)
def calculate_multimask_difference(input_photo,reference_photo,masks,signal_tresholds):
    target_array = numpy.array(Image.open(input_photo),dtype=numpy.float)
    ref_array = numpy.array(Image.open(reference_photo),dtype=numpy.float)
    output_table = numpy.absolute(target_array-ref_array)
    saveImageFormTable(output_table,"raw_diff.jpg")
    outcome = numpy.zeros((masks.shape[0],1),dtype=numpy.bool_)
    output_table = numpy.multiply(output_table,masks[masks.shape[0]-1])
    #saveImageFormTable(output_table,"tmp_00.jpg")
    tmp_sum_1 = numpy.sum(output_table)
    for ll in range(masks.shape[0]):
        if(ll != masks.shape[0] - 1):
            output_table = numpy.multiply(output_table,masks[masks.shape[0]-2-ll])
            tmp_sum = numpy.sum(output_table)
            outcome[ll] = (tmp_sum_1-tmp_sum) < signal_tresholds[masks.shape[0]-2-ll]
            print(tmp_sum_1-tmp_sum)
            tmp_sum_1 = tmp_sum
        else:
            output_table = output_table*0
            outcome[ll] = tmp_sum_1 < signal_tresholds[masks.shape[0]-1-ll]
            print(tmp_sum_1)
        #saveImageFormTable(output_table,"tmp_"+str(ll)+".jpg")
    print(outcome)
    return(outcome)

def display_info_picture(measurement_output,masks):
    tmp_mask_1 = masks[0]//255
    for ii in range(masks.shape[0]):
        tmp_mask = masks[masks.shape[0]-1-ii]
        if(measurement_output[ii]):
            tmp_mask = masks[masks.shape[0]-1-ii]*[0,255,0]
        else:
            tmp_mask = masks[masks.shape[0]-1-ii]*[255,0,0]
        tmp_mask_1 = tmp_mask_1+tmp_mask
    saveImageFormTable(tmp_mask_1,"output.jpg")
    proc = subprocess.Popen(["feh", "--hide-pointer","-S", "filename", "-x", "-q", "-B", "black" , "-g", "800x480","output.jpg"])
    sleep(3)
    proc.kill()
with picamera.PiCamera() as camera:
    camera.resolution = (800,480)
    sleep(2)
    print(" 1-set background photo.\n 2 -set reference photo.\n 3 - Prepare mask images automaticaly.\n 4 measure target fluctuations.\n 5 Load manual mask image.\n 6. Run background noise estimation\n 7. Start autonomic watchdog.\n  0 - exit program.\n") 
    ref_photo_exist = True
    background_photo_exists = True
    mask_exist = True
    Ntests = 0
    n_background_samples = 2
    nmasks = 0;
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
            tmp = Image.open("background.jpg")
            tmp.save("for_manual_mask.bmp")
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
            if(nmasks == 0):
                print("Specify number of slots:")
                nmasks = int(input())
            params = load_treshold_levels_and_prefered_channels("calibration_parameters.txt")
            mask_table = generate_difference_mask_table(nmasks,params[:,1])
            debug_mask_table = generate_mask_table(nmasks)
            start = time.time()
            camera.capture("tmp.jpg",format="jpeg",use_video_port="True")
            output = calculate_multimask_difference("tmp.jpg","reference.jpg",mask_table,params[:,0])
            process_time = time.time()-start
            print("Process ended. Analysis time:")
            print(process_time)
            display_info_picture(output,debug_mask_table)
        elif(command == '5'):
            nmasks = prepare_mask_from_photo("for_manual_mask.bmp")
        elif(command == '6'):
            if(nmasks == 0):
                print("Specify number of slots:")
                nmasks = int(input())
            prepare_noise_calibration_photos(n_background_samples)
            mask_table = generate_mask_table(nmasks)
            calculate_treshhold_levels(mask_table,n_background_samples)
        elif(command == '7'):
            state = True
            params = load_treshold_levels_and_prefered_channels("calibration_parameters.txt")
            mask_table = generate_difference_mask_table(nmasks)
            debug_mask_table = generate_mask_table(nmasks)
            while(True):
                if(GPIO.input(27) == False and state == False):
                    GPIO.output(23,0)
                    GPIO.output(24,0)
                    print("MEASUREMENT!")
                    camera.capture("tmp.jpg",format="jpeg",use_video_port="True")
                    output = calculate_multimask_difference("tmp.jpg","reference.jpg",mask_table,params[:,1])
                    if(all(output)):
                        GPIO.output(23,0)
                        GPIO.output(24,1)
                    else:
                        GPIO.output(23,1)
                        GPIO.output(24,0)
                    display_info_picture(output,debug_mask_table)
                    state = True
                else:
                    if(GPIO.input(27) == True):
                        state = False
        elif(command == '0'):
            exit()
        print("ok " + str(command)+"\n")  