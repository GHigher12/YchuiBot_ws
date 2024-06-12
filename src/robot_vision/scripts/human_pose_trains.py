#! /usr/bin/env python
#  -*-coding:utf8 -*-

import sys
sys.path.append("/home/autobot/workspace/Ychuibot/src/robot_vision/packages")

import time
import rospy
import cv2
import numpy as np
from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
from pose_packages import PoseDetect


class Ros_Pose_Detect():
    def __init__(self):
        self.cTime = 0
        self.pTime = 0
        
        self.pose_detector = PoseDetect()
        self.count = 0
        self.dir = 0
        self.bar = 350

        self.image_pub = rospy.Publisher("ros_pose_trains", Image, queue_size=1)
        self.bridge = CvBridge()
        self.image_sub = rospy.Subscriber("/camera/color/image_raw", Image, self.callback)
        
        
    def callback(self, imgmsg):
        cv_image = self.bridge.imgmsg_to_cv2(imgmsg, "bgr8")
        
        cv_image = self.pose_detector.findPose(cv_image, draw=False)
        lmlist = self.pose_detector.findPostion(cv_image, draw=False)
        if len(lmlist) != 0:
            # print(lmlist)
            # right
            angel = self.pose_detector.findAngle(cv_image, 12, 14, 16)
            per = np.interp(angel, (40, 130), (0, 100))
            self.bar = np.interp(angel, (40, 130), (150, 350))
            # print(angel, per)
            if per == 100:
                if self.dir == 0:
                    self.count += 0.5
                    self.dir = 1
            if per == 0:
                if self.dir == 1:
                    self.count += 0.5
                    self.dir = 0
            cv2.rectangle(cv_image, (5, 150), (30, 350), (0, 255, 0), 2)
            cv2.rectangle(cv_image, (5, int(self.bar)), (30, 350), (0, 255, 0), cv2.FILLED)
            cv2.putText(cv_image, str(int(100 - per))+'%', (0, 380), cv2.FONT_HERSHEY_PLAIN, 2, (255, 0, 0), 2)

            cv2.rectangle(cv_image, (0, 400), (80, 500), (0, 255, 0), cv2.FILLED)
            cv2.putText(cv_image, str(int(self.count)), (1, 470), cv2.FONT_HERSHEY_PLAIN, 4, (255, 0, 0), 2)
            # left
            # detector.findAngle(cv_image, 11, 13, 15)

        self.cTime = time.time()
        fps = 1 / (self.cTime - self.pTime)
        self.pTime = self.cTime
        cv2.putText(cv_image, str(int(fps)), (10, 50), cv2.FONT_HERSHEY_PLAIN, 3, (0, 255, 0), 3)

        cv2.imshow("pose_image", cv_image)
        cv2.waitKey(1)
        
        self.image_pub.publish(self.bridge.cv2_to_imgmsg(cv_image,  "bgr8"))


if __name__ == "__main__":
    try:
        rospy.init_node("human_pose_detect")
        rospy.loginfo("Starting pose detect node ...")
        Ros_Pose_Detect()
        rospy.spin()
    except KeyboardInterrupt:
        rospy.loginfo("Shutdown pose detect node !!!")
        cv2.destroyAllWindows()
