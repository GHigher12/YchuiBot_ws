#! /usr/bin/env python
#  -*-coding:utf8 -*-

import sys
sys.path.append("/home/autobot/workspace/Ychuibot/src/robot_vision/packages")

import time
import rospy
import cv2
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
from pose_packages import PoseDetect


class Ros_Pose_Detect():
    def __init__(self):
        self.cTime = 0
        self.pTime = 0
        
        self.pose_detector = PoseDetect()

        self.image_pub = rospy.Publisher("ros_pose_detect", Image, queue_size=1)
        self.bridge = CvBridge()
        self.image_sub = rospy.Subscriber("/camera/color/image_raw", Image, self.callback)
        
        
    def callback(self, imgmsg):
        cv_image = self.bridge.imgmsg_to_cv2(imgmsg, "bgr8")
        
        cv_image = self.pose_detector.findPose(cv_image, draw=True)

        self.cTime = time.time()
        fps = 1 / (self.cTime - self.pTime)
        self.pTime = self.cTime
        cv2.putText(cv_image, str(int(fps)), (10, 50), cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 3)

        cv2.imshow("pose_image", cv_image)
        cv2.waitKey(1)
        
        self.image_pub.publish(self.bridge.cv2_to_imgmsg(cv_image,  "bgr8"))


if __name__ == "__main__":
    try:
        rospy.init_node("pose_detector")
        rospy.loginfo("Starting pose detect node ...")
        Ros_Pose_Detect()
        rospy.spin()
    except KeyboardInterrupt:
        rospy.loginfo("Shutdown pose detect node !!!")
        cv2.destroyAllWindows()
