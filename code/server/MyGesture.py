import cv2
import mediapipe as mp
import time
from math import sqrt
import json

def getLength(v):
    return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2])

def getDotProduct(v1, v2):
    return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]

def getCrossProduct(v1, v2):
    return [v1[1]*v2[2]-v1[2]*v2[1], v1[2]*v2[0]-v1[0]*v2[2], v1[0]*v2[1]-v1[1]*v2[0]]

def checkOpen(p1, p2, p3, p4):
    v1 = [p1[0]-p2[0], p1[1]-p2[1], p1[2]-p2[2]]
    v2 = [p2[0]-p3[0], p2[1]-p3[1], p2[2]-p3[2]]
    v3 = [p3[0]-p4[0], p3[1]-p4[1], p3[2]-p4[2]]
    v1Length = getLength(v1)
    v2Length = getLength(v2)
    v3Length = getLength(v3)
    cosAngle1 = -getDotProduct(v1, v2)/(v1Length*v2Length)
    cosAngle2 = -getDotProduct(v2, v3)/(v2Length*v3Length)
    if cosAngle1<-0.5 and cosAngle2<-0.5:
        return True
    else:
        return False

def checkDirection(p1, p2, p3, p4):
    v1 = [p1[0]-p2[0], p1[1]-p2[1], p1[2]-p2[2]]
    v2 = [p2[0]-p3[0], p2[1]-p3[1], p2[2]-p3[2]]
    v3 = [p3[0]-p4[0], p3[1]-p4[1], p3[2]-p4[2]]
    d1 = [1, 0, 0]  # Left, Right
    d2 = [0, 1, 0]  # Up, Down
    d3 = [0, 0, 1]  # Front, Back
    v1Length = getLength(v1)
    v2Length = getLength(v2)
    v3Length = getLength(v3)
    # Check Left, Right
    cosAngle11 = getDotProduct(v1, d1)/v1Length
    cosAngle12 = getDotProduct(v2, d1)/v2Length
    cosAngle13 = getDotProduct(v3, d1)/v3Length
    # Check Up, Down
    cosAngle21 = getDotProduct(v1, d2)/v1Length
    cosAngle22 = getDotProduct(v2, d2)/v2Length
    cosAngle23 = getDotProduct(v3, d2)/v3Length
    # Check Front, Back
    cosAngle31 = getDotProduct(v1, d3)/v1Length
    cosAngle32 = getDotProduct(v2, d3)/v2Length
    cosAngle33 = getDotProduct(v3, d3)/v3Length

    if cosAngle11>0.5 and cosAngle12>0.5 and cosAngle13>0.5:
        return "Left"
    elif cosAngle11<-0.5 and cosAngle12<-0.5 and cosAngle13<-0.5:
        return "Right"
    elif cosAngle21>0.5 and cosAngle22>0.5 and cosAngle23>0.5:
        return "Down"
    elif cosAngle21<-0.5 and cosAngle22<-0.5 and cosAngle23<-0.5:
        return "Up"
    elif cosAngle31>0.5 and cosAngle32>0.5 and cosAngle33>0.5:
        return "Back"
    elif cosAngle31<-0.5 and cosAngle32<-0.5 and cosAngle33<-0.5:
        return "Front"
    else:
        return "Unknown"

def checkPalmDirection(p0, p1, p2, type):
    v1 = [p1[0]-p0[0], p1[1]-p0[1], p1[2]-p0[2]]
    v2 = [p2[0]-p0[0], p2[1]-p0[1], p2[2]-p0[2]]
    if type == "Left":
        vNorm = getCrossProduct(v2, v1)
    else:
        vNorm = getCrossProduct(v1, v2)
    d1Norm = [0, 1, 0]  # Check Up, Down
    d2Norm = [0, 0, 1]  # Check Front, Back
    cosAngle1 = getDotProduct(vNorm, d1Norm)/getLength(vNorm)
    cosAngle2 = getDotProduct(vNorm, d2Norm)/getLength(vNorm)
    if cosAngle1 > 0.5:
        return "Up"
    elif cosAngle1 < -0.5:
        return "Down"
    elif cosAngle2 > 0.5:
        return "Front"
    else:
        return "None"

class MyGesture:
    def __init__(self, s, dest):
        # self.cap = cv2.VideoCapture(0)
        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5)
        self.mpDraw = mp.solutions.drawing_utils
        self.handLmsStyle = self.mpDraw.DrawingSpec(color=(0, 0, 255), thickness=3)
        self.handConStyle = self.mpDraw.DrawingSpec(color=(0, 255, 0), thickness=5)
        self.pTime = 0
        self.cTime = 0
        self.getLeftGesture = "None"
        self.getLeftDirection = "Unknown"
        self.getRightGesture = "None"
        self.getRightDirection = "Unknown"
        self.s = s
        self.dest = dest
        self.name = "MyGesture"
        self.reset = True
        # self.dataGUI = dataGUI

    def __getGesture__(self, landmark, type):
        # ✌️(G): Go, ✋(S): Stop
        # 👈(L): Left, 👉(R): Right, 👆(F): Front, 👇(B): Back
        # 🫴(U): SpeedUp, 🫳(D): SlowDown

        # finger states
        # thumbOpen = checkOpen(landmark[4], landmark[3], landmark[2], landmark[1])
        indexOpen = checkOpen(landmark[8], landmark[7], landmark[6], landmark[5])
        middleOpen = checkOpen(landmark[12], landmark[11], landmark[10], landmark[9])
        ringOpen = checkOpen(landmark[16], landmark[15], landmark[14], landmark[13])
        pinkyOpen = checkOpen(landmark[20], landmark[19], landmark[18], landmark[17])

        # direction = None
        # gesture = None

        # if type == "Left":
        #     direction = self.getLeftDirection
        #     gesture = self.getLeftGesture
        # else:
        #     direction = self.getRightDirection
        #     gesture = self.getRightGesture

        if type == "Left":
            if indexOpen:
                self.getLeftDirection = checkDirection(landmark[8], landmark[7], landmark[6], landmark[5])
            else:
                self.getLeftDirection = "Unknown"

            if indexOpen and middleOpen and not(ringOpen) and not(pinkyOpen):
                self.getLeftGesture = "Go"
            elif indexOpen and not(middleOpen) and not(ringOpen) and not(pinkyOpen):
                if self.getLeftDirection == "Left":
                    self.getLeftGesture = "Left"
                elif self.getLeftDirection == "Right":
                    self.getLeftGesture = "Right"
                elif self.getLeftDirection == "Up":
                    self.getLeftGesture = "Front"
                elif self.getLeftDirection == "Down":
                    self.getLeftGesture = "Back"
                else:
                    self.getLeftGesture = "None"
            elif indexOpen and middleOpen and ringOpen and pinkyOpen:
                palmDirection = checkPalmDirection(landmark[0], landmark[5], landmark[17], type)
                if self.getLeftDirection == "Left" or self.getLeftDirection == "Right":
                    if palmDirection == "Up":
                        self.getLeftGesture = "SpeedUp"
                    elif palmDirection == "Down":
                        self.getLeftGesture = "SlowDown"
                    else:
                        self.getLeftGesture = "None"
                elif self.getLeftDirection == "Up":
                    if palmDirection == "Front":
                        self.getLeftGesture = "Stop"
                    else:
                        self.getLeftGesture = "None"
                else:
                    self.getLeftGesture = "None"
            else:
                self.getLeftGesture = "None"
        else:
            if indexOpen:
                self.getRightDirection = checkDirection(landmark[8], landmark[7], landmark[6], landmark[5])
            else:
                self.getRightDirection = "Unknown"

            if indexOpen and middleOpen and not(ringOpen) and not(pinkyOpen):
                self.getRightGesture = "Go"
            elif indexOpen and not(middleOpen) and not(ringOpen) and not(pinkyOpen):
                if self.getRightDirection == "Left":
                    self.getRightGesture = "Left"
                elif self.getRightDirection == "Right":
                    self.getRightGesture = "Right"
                elif self.getRightDirection == "Up":
                    self.getRightGesture = "Front"
                elif self.getRightDirection == "Down":
                    self.getRightGesture = "Back"
                else:
                    self.getRightGesture = "None"
            elif indexOpen and middleOpen and ringOpen and pinkyOpen:
                palmDirection = checkPalmDirection(landmark[0], landmark[5], landmark[17], type)
                if self.getRightDirection == "Left" or self.getRightDirection == "Right":
                    if palmDirection == "Up":
                        self.getRightGesture = "SpeedUp"
                    elif palmDirection == "Down":
                        self.getRightGesture = "SlowDown"
                    else:
                        self.getRightGesture = "None"
                elif self.getRightDirection == "Up":
                    if palmDirection == "Front":
                        self.getRightGesture = "Stop"
                    else:
                        self.getRightGesture = "None"
                else:
                    self.getRightGesture = "None"
            else:
                self.getRightGesture = "None"

        # if indexOpen:
        #     direction = checkDirection(landmark[8], landmark[7], landmark[6], landmark[5])
        # else:
        #     direction = "Unknown"

        # if indexOpen and not(middleOpen) and not(ringOpen) and not(pinkyOpen):
        #     if direction == "Left":
        #         gesture = "Left"
        #     elif direction == "Right":
        #         gesture = "Right"
        #     elif direction == "Up":
        #         gesture = "Up"
        #     elif direction == "Down":
        #         gesture = "Down"
        #     else:
        #         gesture = "None"
        # elif indexOpen and middleOpen and ringOpen and pinkyOpen:
        #     palmDirection = checkPalmDirection(landmark[0], landmark[5], landmark[17], type)
        #     if direction == "Left" or direction == "Right":
        #         if palmDirection == "Up":
        #             gesture = "SpeedUp"
        #         elif palmDirection == "Down":
        #             gesture = "SlowDown"
        #         else:
        #             gesture = "None"
        #     elif direction == "Up":
        #         if palmDirection == "Front":
        #             gesture = "Stop"
        #         else:
        #             gesture = "None"
        #     else:
        #         gesture = "None"
        # else:
        #     gesture = "None"

    def start(self, isJetson=False):
        # self.dataGUI.getData()
        if isJetson:
            self.cap = cv2.VideoCapture('nvarguscamerasrc ! nvvidconv ! video/x-raw, format=BGRx ! videoconvert ! video/x-raw, format=BGR ! appsink', cv2.CAP_GSTREAMER)
        else:
            self.cap = cv2.VideoCapture(0)
        while True:
            ret, img = self.cap.read()
            if ret:
                imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                result = self.hands.process(imgRGB)
                imgHeight = img.shape[0]
                imgWidth = img.shape[1]
                handTypeList = []
                landmarkList = []
                if result.multi_hand_landmarks:
                    for hand in result.multi_handedness:
                        handType=hand.classification[0].label
                        handTypeList.append(handType)
                    for handLms in result.multi_hand_landmarks:
                        self.mpDraw.draw_landmarks(img, handLms, self.mpHands.HAND_CONNECTIONS, self.handLmsStyle, self.handConStyle)
                        landmark = []
                        for i, lm in enumerate(handLms.landmark):
                            landmark.append([lm.x, lm.y, lm.z])
                        landmarkList.append(landmark)
                    isLeft = False
                    isRight = False
                    for i in range(len(handTypeList)):
                        self.__getGesture__(landmarkList[i], handTypeList[i])
                        if handTypeList[i]=="Left":
                            isLeft = True
                        else:
                            isRight = True
                    if not(isLeft):
                        self.getLeftDirection = "Unknown"
                        self.getLeftGesture = "None"
                    if not(isRight):
                        self.getRightDirection = "Unknown"
                        self.getRightGesture = "None"

                self.cTime = time.time()
                fps = 1/(self.cTime-self.pTime)
                self.pTime = self.cTime
                cv2.putText(img, f"FPS : {int(fps)}", (30, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 3)
                cv2.putText(img, "Left", (imgWidth-180, imgHeight-150), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(img, self.getRightGesture, (imgWidth-180, imgHeight-50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(img, self.getRightDirection, (imgWidth-180, imgHeight-100), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(img, "Right", (30, imgHeight-150), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(img, self.getLeftGesture, (30, imgHeight-50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.putText(img, self.getLeftDirection, (30, imgHeight-100), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 3)
                cv2.imshow(self.name, img)
                if (self.getLeftGesture!="None" or self.getRightGesture!="None"):
                    if self.reset:
                        data = {
                            "type": "RPI_GES",
                            "left": self.getRightGesture,
                            "right": self.getLeftGesture,
                            "dest": self.dest
                        }
                        self.s.send(bytes(json.dumps(data),"utf-8"))
                        self.reset = False
                else:
                    self.reset = True
            if cv2.waitKey(1) == ord('q'):
                cv2.destroyWindow(self.name)
                self.cap.release()
                break
            # elif cv2.waitKey(1) == ord('s'):
            #     self.dataGUI.__start__()
            # elif cv2.waitKey(1) == ord('t'):
            #     self.dataGUI.__stop__()
                

if __name__ == "__main__":
    myGesture = MyGesture()
    myGesture.start()