#! /usr/bin/env python
#  -*-coding:utf8 -*-

import rospy
import cv2
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image

class image_converter:
    def __init__(self):
        self.image_pub = rospy.Publisher("cv_bridge_image", Image, queue_size=1)
        self.bridge = CvBridge()
        self.image_sub = rospy.Subscriber("/camera/color/image_raw", Image, self.callback)
        
    def callback(self, imgmsg):
        cv_image = self.bridge.imgmsg_to_cv2(imgmsg, "bgr8")
        
        cv_image_gray = cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
        
        cv2.imshow("image", cv_image_gray)
        cv2.waitKey(1)
        
        self.image_pub.publish(self.bridge.cv2_to_imgmsg(cv_image_gray))
        
if __name__ == "__main__":
    try:
        rospy.init_node("cv_to_bridge")
        rospy.loginfo("Starting cv_bridge node ...")
        image_converter()
        rospy.spin()
    except KeyboardInterrupt:
        rospy.loginfo("Shutdown cv_bridge node !!!")
        cv2.destroyAllWindows()
