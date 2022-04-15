import cv2
import mediapipe as mp
import time

cap = cv2.VideoCapture(0)  # 調用電腦攝像頭，我這裏參數設置成1的時候出現了不兼容的問題，參數爲鏡頭編號
mpHands = mp.solutions.hands  # 使用手部模型，參考文末函數（1）
hands = mpHands.Hands(max_num_hands=4)  # 調用Hands函數，有五個參數可以設置
mpDraw = mp.solutions.drawing_utils  # 在圖像上畫座標點
handLmsStyle = mpDraw.DrawingSpec(color=(0, 255, 0), thickness=5)
handConStyle = mpDraw.DrawingSpec(color=(255, 0, 0), thickness=4)
pTime = 0
cTime = 0

while True:
    ret, img = cap.read()  # cap.read()按幀讀取視頻，ret是布爾值，img接受返回的每一幀的圖像
    if ret:
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # 需要先將opencv預設的BGR圖片轉換成RGB圖片
        result = hands.process(imgRGB)  # 把轉換結果放到result

        imgHeight = img.shape[0]  # 圖像的縱座標
        imgWidth = img.shape[1]  # 圖像的橫座標

        # 用於圖像上標識fps
        cTime = time.time()
        fps = int(1/(cTime - pTime))
        pTime = cTime

        cv2.putText(img, f"FPS = {fps}", (30, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,0,0), 3)  # 標識fps
        # print(result.multi_hand_landmarks)  # 回傳手上21個點的x y z座標
        if result.multi_hand_landmarks:
            for handlms in result.multi_hand_landmarks:
                mpDraw.draw_landmarks(img, handlms, mpHands.HAND_CONNECTIONS, handLmsStyle, handConStyle)  # 在圖像上畫出每一個點，參數參考文末函數（3）
                for i, lm in enumerate(handlms.landmark):
                    xPos = int(lm.x * imgWidth)  # 如果直接打印的是lm.x則是比例，可以用比例×圖像的橫座標得到圖像點的座標
                    yPos = int(lm.y * imgHeight)
                    cv2.putText(img, str(i), (xPos-5, yPos-5), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 0, 255), 2)  # 在圖像上標識點的信息
                    # if i == 4:
                    #    cv2.circle(img, (xPos, yPos), 20, (0,0,255), cv2.FILLED)  # 可以對特定點設置不同的標識特點，具體看參數
                    print(i, xPos, yPos)  # 打印第i個點的座標
        cv2.imshow('img', img)  # 顯示圖像
    if cv2.waitKey(1) == ord('q'):
        break