#!/usr/bin/env python3
from pygments import highlight
import rospy
import sys 
import cv2
import numpy as np
from cv_bridge import CvBridge
from cv_bridge import CvBridgeError
from std_msgs.msg import UInt16MultiArray,Bool
from sensor_msgs.msg import Image

# THE MESSAGE TO MAIN WILL BE THE FOLLOWING: uint16_t arr[] = {identifier, xPosition, yPosition, width, height}

imageInTopic  = "rgb_stereo_publisher/color/image"
imageOutTopic = "vision"
imageStatusTopic = "image_status"
getImageTopic = "get_highlights"
ackImageTopic = "ack_highlights"

coneIdentifier = 0x00
bucketIdentifier = 0x01


class ImageProcessor:

    def __init__(self):
        self.imageOutPub = rospy.Publisher(imageOutTopic,Image, queue_size=5)
        self.mainMessagePub = rospy.Publisher(imageStatusTopic,UInt16MultiArray,queue_size=5)
        self.imageSub = rospy.Subscriber(imageInTopic,Image,self.ImageCB,queue_size=1)
        self.getImage = rospy.Subscriber(getImageTopic,Bool,self.GetHighlightCB, queue_size=1)
        self.ackPub = rospy.Publisher(ackImageTopic,Bool,queue_size=1)
        self.bridge = CvBridge()
        self.counter = 0
        self.hb_count = 0
        self.saveImage = False
        self.saveImageCounter = 0


    def ImageCB(self,data):
        self.counter+=1
        if self.counter%5==0:
            print("heartbeat " + str(self.hb_count) + "\n")
            cv_image = self.bridge.imgmsg_to_cv2(data, "bgr8")
            self.hb_count+=1
            self.ProcessRosImage(cv_image)

    # print("here in ImageCB")
    def GetHighlightCB(self ,data):
        if(data.data==1):
            print("need to save image...\n")
            self.saveImage=True          



    def ProcessRosImage(self,image):
        # try:
        #     image = CvBridge.imgmsg_to_cv2(rosImageIn,"bgr8")
        # except CvBridgeError as e:
        #     print(e)
        if(self.saveImage==True):
            print("Saving image....\n")
            self.saveImage= False
            cv2.imwrite('/home/netipc/RobotsAndCones/screenshots/Image_' + str(self.saveImageCounter)+ '.jpeg',image)
            self.saveImageCounter+=1
            self.ackPub.publish(True)
        
        #obtain hue,thicc,race from image
        hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

        #red, green, blue colour detection tresholds
        # lower_red1 = np.array([0,100,50])
        # upper_red1 = np.array([100,255,255])

        #lg = np.array([50,120,30])
        #ug = np.array([90,255,150])

        #lb = np.array([98,120,30])
        #ub = np.array([139,255,255])

        #create the image with filtered colours
        # mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
        mask1 = cv2.inRange(hsv, (0,150 ,80), (20,255,255))
        mask2 = cv2.inRange(hsv, (175,100,80), (185,255,255))


        #mask2 = cv2.inRange(hsv, lb, ub)
        #mask3 = cv2.inRange(hsv, lg, ug)
        #mask = cv2.bitwise_or(mask1,mask2)
        #mask = cv2.bitwise_or(mask,mask3)
        # mask = cv2.bitwise_not(mask1)
        mask = cv2.bitwise_or(mask1,mask2)
        # thresh = cv2.threshold(mask, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)[1]

        # rosImageOut = self.bridge.cv2_to_imgmsg(mask,"mono8")
        # self.imageOutPub.publish(rosImageOut)

        #determine objects and draw boundaries
        #\/ obtains boundaries
        # cnts = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE) 
        cnts = cv2.findContours(mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        cnts = cnts[0] if len(cnts) == 2 else cnts[1]
        w_tol=50 #set for boundary
        h_tol = 80

        for c in cnts:
            #get boundary info
            x,y,w,h = cv2.boundingRect(c)
            if w>w_tol and h>h_tol: #if boundary is large, investigate potential cone/bucket
                #determine how many pixels are of the rgb colour in boundary box
                shade=0
                for i in range(h):
                    # shade+=sum(thresh[y+i][x:x+w])
                    shade+=sum(mask[y+i][x:x+w])

                shade=shade//255
                #what percentage of pixelse in box are a specific colour
                ratio = shade*100//h//w
                # print("Ratio is "+str(ratio)+"\n")
                if ratio<=43:
                    print("Red Cone Detected")
                    cv2.rectangle(image, (x, y), (x + w, y + h), (255,0,255), 2)
                    cv2.putText(image, 'Cone', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (36,12,255), 2)
                    conepos_x=x+w//2
                    conepos_y = y+h//2
                    msgToSend = UInt16MultiArray()
                    msgToSend.data = [0,0,0,0,0,0]
                    msgToSend.data[0] = coneIdentifier
                    msgToSend.data[1] = conepos_x
                    msgToSend.data[2] = conepos_y
                    msgToSend.data[3] = w
                    msgToSend.data[4] = h
                    msgToSend.data[5] = self.hb_count

                    # print("x = " + str(conepos_x) + "\n")
                    # print("y = " + str(conepos_y) + "\n")
                    # print("w = " + str(w) + "\n")
                    # print("h = " + str(h) + "\n")
                    print("ratio: " + str(ratio) + "\n")

                    self.mainMessagePub.publish(msgToSend)



                    #tell ros/main a cone has been detected
                else:
                    if w < 100: continue
                    print("Bucket Detected")
                    cv2.rectangle(image, (x, y), (x + w, y + h), (255,255,0), 2)
                    cv2.putText(image, 'Bucket', (x, y-10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (36,255,12), 2)

                    buckpos_x=x+w//2
                    buckpos_y = y+h//2 

                    msgToSend = UInt16MultiArray()
                    msgToSend.data = [0,0,0,0,0,0]
                    msgToSend.data[0] = bucketIdentifier
                    msgToSend.data[1] = buckpos_x
                    msgToSend.data[2] = buckpos_y
                    msgToSend.data[3] = w
                    msgToSend.data[4] = h
                    msgToSend.data[5] = self.hb_count

                    self.mainMessagePub.publish(msgToSend)

                    # print("x = " + str(buckpos_x) + "\n")
                    # print("y = " + str(buckpos_y) + "\n")
                    # print("w = " + str(w) + "\n")
                    # print("h = " + str(h) + "\n")
                    print("ratio: " + str(ratio) + "\n")
                    
                    #tell ros/main a bucket has been detected
        rosImageOut = self.bridge.cv2_to_imgmsg(image,"bgr8")
        self.imageOutPub.publish(rosImageOut)





def main(args):
    rospy.init_node('img_proc_node',anonymous=True)
    rate = rospy.Rate(20)
    imgProc = ImageProcessor()
    while not rospy.is_shutdown():
        rospy.spin()
        rate.sleep()
    cv2.destroyAllWindows()
     


    



if __name__=='__main__':
    main(sys.argv)
    

