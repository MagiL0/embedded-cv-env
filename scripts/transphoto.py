'''
Author: FYB
Description: GitHub: https://github.com/magil0
'''
import cv2
import sys
import numpy as np

print(__name__)
if __name__ == "__main__":
    try:
        photo_path = sys.argv[1]
    except:
        print("error: no input args")
        exit(0)
    
    name = photo_path[:-4]
    # print(name, photo_path)
    
    image = cv2.imread(photo_path, 1)
    print(">> source image data:")
    print(">>   width: ", image.shape[1])
    print(">>   height: ", image.shape[0])
    print(">>   dtype: ", image.dtype)
    image = cv2.pyrDown(image)
    image = cv2.pyrDown(image)
    image = cv2.pyrDown(image)
    image = cv2.pyrDown(image)
    image = cv2.resize(image, (192, 120))
    image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    - 
    print(">>   width: ", image.shape[1])
    print(">>   height: ", image.shape[0])
    print(">>   dtype: ", image.dtype)
    # cv2.imshow("raw image", image)
    # cv2.waitKey(0)
    np.savetxt(name + ".txt", image, fmt= '%4d', delimiter='')
    
