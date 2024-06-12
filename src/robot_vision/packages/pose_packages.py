import time
import cv2
import math
import mediapipe as mp


class PoseDetect():
    def __init__(self, mode=False, complex=1, landmarks=True, enableSeg=False, smoothSeg=True, detectionCon=0.5, trackCon=0.5):
        self.mode = mode,
        self.complex = complex,
        self.landmarks = landmarks,
        self.enableSeg = enableSeg,
        self.smoothSeg = smoothSeg,
        self.detectionCon = detectionCon,
        self.trackCon = trackCon
        self.mpPose = mp.solutions.pose
        self.pose = self.mpPose.Pose()
        self.mpDraw = mp.solutions.drawing_utils

    def findPose(self, img, draw=True):
        w, h = img.shape[:2]
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        self.results = self.pose.process(imgRGB)
        # print(self.results.pose_landmarks)
        if self.results.pose_landmarks:
            # for poseLms in self.results.pose_landmarks:
            if draw:
                self.mpDraw.draw_landmarks(img, self.results.pose_landmarks, self.mpPose.POSE_CONNECTIONS)
        return img

    def findPostion(self, img, Posenum=0, draw=True):
        self.lmlist = []
        if self.results.pose_landmarks:
            # myPose = self.results.pose_landmarks[Posenum]
            for id, lm in enumerate(self.results.pose_landmarks.landmark):
                h, w, c = img.shape
                # print(id, lm)
                cx, cy = int(lm.x * w), int(lm.y * h)
                self.lmlist.append([id, cx, cy])
                if draw:
                    # if id == Posenum:
                    cv2.circle(img, (cx, cy), 4, (0, 255, 0), cv2.FILLED)
        return self.lmlist

    def findAngle(self, img, p1, p2, p3, draw=True):
        # _, x1, y1 = self.lmlist[p1]
        x1, y1 = self.lmlist[p1][1:]
        x2, y2 = self.lmlist[p2][1:]
        x3, y3 = self.lmlist[p3][1:]
        angel = math.degrees(math.atan2(y1 - y2, x1 - x2) - math.atan2(y3 - y2, x3 - x2))
        if angel <= 0:
            angel += 360

        # print(angel)
        if draw:
            cv2.line(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.line(img, (x2, y2), (x3, y3), (0, 255, 0), 2)
            cv2.circle(img, (x1, y1), 5, (255, 0, 255), cv2.FILLED)
            cv2.circle(img, (x2, y2), 5, (255, 0, 255), cv2.FILLED)
            cv2.circle(img, (x3, y3), 5, (255, 0, 255), cv2.FILLED)
            cv2.circle(img, (x1, y1), 15, (255, 0, 255), 3)
            cv2.circle(img, (x2, y2), 15, (255, 0, 255), 3)
            cv2.circle(img, (x3, y3), 15, (255, 0, 255), 3)
            cv2.putText(img, str(int(angel)), (x2-20, y2+70), cv2.FONT_HERSHEY_PLAIN, 2, (0, 0, 255), 2)

        return angel
