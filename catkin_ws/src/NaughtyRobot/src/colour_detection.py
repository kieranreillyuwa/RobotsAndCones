#http://wiki.ros.org/cv_bridge/Tutorials/ConvertingBetweenROSImagesAndOpenCVImagesPython

import rospy
import cv2
import numpy as np
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError



class image_converter:


  def __init__(self):
    self.image_pub = rospy.Publisher("out_image",Image)

    self.bridge = CvBridge()
    self.image_sub = rospy.Subscriber("in_image",Image,self.callback)

  def callback(self,data):
    try:
      cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
    except CvBridgeError as e:
      print(e)

    (rows,cols,channels) = cv_image.shape
    if cols > 60 and rows > 60 :
      cv2.circle(cv_image, (50,50), 10, 255)
    #convert the image to hsv format
    hsv = cv2.cvtColor(cv_image, cv2.COLOR_BGR2HSV)
    #define acceptable pixel colours
    lower_red = np.array([0,120,40])
    upper_red = np.array([30,255,230])
    #define pixels with good colour
    mask = cv2.inRange(hsv,lower_red,upper_red)
    #generate histograms
    xhist = np.sum(mask/255,axis=0,dtype=np.uint8)
    xr = np.argmax(xhist)
    yhist = np.sum(mask/255,axis=1,dtype=np.uint8)
    yr = np.argmax(yhist)
    mask = cv2.circle(mask,(xr,yr),50,(0,255,128))
    #display processed image
    cv2.imshow("Filtered Image", mask)
    cv2.imshow("Image window", cv_image)
    cv2.waitKey(3)

    try:
      self.image_pub.publish(self.bridge.cv2_to_imgmsg(cv_image, "bgr8"))
    except CvBridgeError as e:
      print(e)

   	
      
      
ic = image_converter()
rospy.init_node('image_converter', anonymous=True)
try:
	rospy.spin()
except KeyboardInterrupt:
	print("Shutting down")
cv2.destroyAllWindows()
