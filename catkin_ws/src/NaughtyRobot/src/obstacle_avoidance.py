import rospy
import sys 
import cv2
import numpy as np
from cv_bridge import CvBridge
from cv_bridge import CvBridgeError
from std_msgs.msg import UInt16MultiArray,Bool
from sensor_msgs.msg import Image


depth_camera_topic = '/rgb_stereo_publisher/stereo/depth'



class DepthImageProcessor:
    def __init__(self):
        self.bridge = CvBridge()



    def ProcessDepthImage(self,cv_image):
        gray_filtered = cv2.inRange(cv_image,30,120)
        mask = cv2.bitwise_not(gray_filtered)
        thresh = cv2.threshold()