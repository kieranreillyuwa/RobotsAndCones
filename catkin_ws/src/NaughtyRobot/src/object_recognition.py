#!/usr/bin/env python3
from pygments import highlight
import rospy
import sys 
import cv2
import numpy as np
from cv_bridge import CvBridge
from std_msgs.msg import UInt16MultiArray,Bool
from sensor_msgs.msg import Image

# THE MESSAGE TO MAIN WILL BE THE FOLLOWING: uint16_t arr[] = {identifier, xPosition, yPosition, width, height}

imageInTopic  = "**INSERT**"
imageOutTopic = "vision"
imageStatusTopic = "image_status"
mainStatusTopic = "main_status"

coneIdentifier = 0x00
bucketIdentifier = 0x01



class ImageProcessor:

    def __init__(self):
        self.process = False
        self.imageOutPub = rospy.Publisher(imageOutTopic,Image)
        self.mainMessagePub = rospy.Publisher(imageStatusTopic,UInt16MultiArray)
        self.mainStatusSub = rospy.Subscriber(mainStatusTopic,Bool,self.StatCB)
        self.imageSub = rospy.Subscriber(imageInTopic,Image,self.ImageCB)

    def StatCB(self,data):
        if(data.data == 1):
            self.process = True
        else:
            self.process = False

    def ImageCB(self,data):
        if self.process:
            self.ProcessRosImage(self,data)




    def ProcessRosImage(self,rosImageIn):
        image = CvBridge.imgmsg_to_cv2(rosImageIn,"bgr8")
        #obtain hue,thicc,race from image
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        #red, green, blue colour detection tresholds
        lower_red1 = np.array([0,120,30])
        upper_red1 = np.array([20,255,255])

        lg = np.array([50,120,30])
        ug = np.array([90,255,150])

        lb = np.array([98,120,30])
        ub = np.array([139,255,255])

        #create the image with filtered colours
        mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
        mask2 = cv2.inRange(hsv, lb, ub)
        mask3 = cv2.inRange(hsv, lg, ug)
        mask = cv2.bitwise_or(mask1,mask2)
        mask = cv2.bitwise_or(mask,mask3)
        mask = cv2.bitwise_not(mask)
        thresh = cv2.threshold(mask, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)[1]

        #determine objects and draw boundaries
        #\/ obtains boundaries
        cnts = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
        cnts = cnts[0] if len(cnts) == 2 else cnts[1]
        tol=140 #set for boundary

        for c in cnts:
            #get boundary info
            x,y,w,h = cv2.boundingRect(c)
            if w>tol and h>tol: #if boundary is large, investigate potential cone/bucket
                #determine how many pixels are of the rgb colour in boundary box
                shade=0
                for i in range(h):
                    shade+=sum(thresh[y+i][x:x+w])
                shade=shade//255
                #what percentage of pixelse in box are a specific colour
                ratio = shade*100//h//w
                if ratio>40 and ratio<=60:
                    print("Red Cone Detected")
                    cv2.rectangle(image, (x, y), (x + w, y + h), (255,0,255), 2)
                    cv2.putText(image, 'Cone', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (36,12,255), 2)
                    conepos_x=x+w//2
                    conepos_y = y+h//2
                    msgToSend = UInt16MultiArray()
                    msgToSend.data[0] = coneIdentifier
                    msgToSend.data[1] = conepos_x
                    msgToSend.data[2] = conepos_y
                    msgToSend.data[3] = w
                    msgToSend.data[4] = h

                    print("x = " + conepos_x + "\n")
                    print("y = " + conepos_y + "\n")
                    print("w = " + w + "\n")
                    print("h = " + h + "\n")

                    self.mainMessagePub.publish(msgToSend)



                    #tell ros/main a cone has been detected
                else:
                    print("Bucket Detected")
                    cv2.rectangle(image, (x, y), (x + w, y + h), (255,255,0), 2)
                    cv2.putText(image, 'Bucket', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (36,255,12), 2)

                    buckpos_x=x+w//2
                    buckpos_y = y+h//2 

                    msgToSend = UInt16MultiArray()
                    msgToSend.data[0] = coneIdentifier
                    msgToSend.data[1] = buckpos_x
                    msgToSend.data[2] = buckpos_y
                    msgToSend.data[3] = w
                    msgToSend.data[4] = h

                    self.mainMessagePub.publish(msgToSend)

                    print("x = " + buckpos_x + "\n")
                    print("y = " + buckpos_y + "\n")
                    print("w = " + w + "\n")
                    print("h = " + h + "\n")
                    #tell ros/main a bucket has been detected
        rosImageOut = CvBridge.cv2_to_imgmsg(image,"bgr8")
        self.imageOutPub.publish(rosImageOut)





def main(args):
    rospy.init_node('Image Processing',anonymous=True)
    rate = rospy.Rate(5)
    imgProc = ImageProcessor()
    while not rospy.is_shutdown():
        rospy.spin()
        rate.sleep()
    cv2.destroyAllWindows()
     


    



if __name__=='__main__':
    main(sys.argv)
    

