#-*-coding:utf-8-*-
'''
Author: FYB
Description: GitHub: https://github.com/magil0
'''
from encodings import utf_8
import cv2
import sys
import numpy as np

if __name__ == "__main__":
    try:
        photo_path = sys.argv[1]
    except:
        print("error: no input args")
        exit(0)
    
    print(">> trying open ", photo_path)
    image = np.loadtxt(photo_path, dtype= np.uint8)
    raw_image = np.loadtxt(photo_path[:-5] + ".txt", dtype= np.uint8)

    print(">> input image data:")
    print(">>   width: ", image.shape[1])
    print(">>   height: ", image.shape[0])
    print(">>   dtype: ", image.dtype)

    cv2.imshow('raw image', raw_image)
    cv2.imshow('image', image)
    cv2.waitKey(0)