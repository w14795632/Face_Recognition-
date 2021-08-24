import face_recognition
import cv2
import numpy as np
import paho.mqtt.publish as publish
import time
import requests

### LINE 通知設定
headers = {
    "Authorization": "Bearer " + "cilGuMLmQpO6gCcKAtfefpDdrwHdCqKMUjiUj2N****",
    "Content-Type": "application/x-www-form-urlencoded"
}


###  讀取電腦上的鏡頭位置
video_capture = cv2.VideoCapture(0)

### 建立人臉資料夾，指定路徑需要正確位置 ((請自行更新))

chin_image = face_recognition.load_image_file("face_image\\chin.jpg")
chin_face_encoding = face_recognition.face_encodings(chin_image)[0]


###  建立已知臉孔代碼
known_face_encodings = [
    chin_face_encoding, 
]

### 已知臉孔代碼 對應姓名
known_face_names = [
    "Chin"
]


## 反覆迴圈讀取影像內容
while True:
    # 取一個畫面
    ret, frame = video_capture.read()

    # 將該畫面轉換為灰階模式 
    rgb_frame = frame[:, :, ::-1]

    # 讀取該畫面的臉孔資料  
    face_locations = face_recognition.face_locations(rgb_frame)
    face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)

    # 比對是否符合 已知臉孔資料庫內是否有相同的臉孔代碼
    for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
        # See if the face is a match for the known face(s)
        matches = face_recognition.compare_faces(known_face_encodings, face_encoding,tolerance=0.5)

        name = "Unknown"



        # Or instead, use the known face with the smallest distance to the new face
        face_distances = face_recognition.face_distance(known_face_encodings, face_encoding)
        best_match_index = np.argmin(face_distances)
        

        ##如果符合人臉資料庫內的圖案，則透過MQTT傳送指令給 NodeMCU
        if matches[best_match_index]:
            name = known_face_names[best_match_index]
            msg = '1'
            publish.single("inTopicJIN", msg, hostname="broker.mqtt-dashboard.com")

            params = {"message": name +"回來囉"}
 
            r = requests.post("https://notify-api.line.me/api/notify",
                headers=headers, params=params)

             
            time.sleep(10)

            msg = '0'

            publish.single("inTopicJIN", msg, hostname="broker.mqtt-dashboard.com")

        else :
            
            name = "Unknown"

            params = {"message": "注意囉! 有不認識的人在你家門口~"}
 
            r = requests.post("https://notify-api.line.me/api/notify",
                headers=headers, params=params)

            time.sleep(10)

        # 畫出臉孔框框
        cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

        # 畫出 姓名框框
        cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
        font = cv2.FONT_HERSHEY_DUPLEX
        cv2.putText(frame, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)

    # Display the resulting image
    cv2.imshow('Video', frame)

    # 輸入" q "  結束畫面 (大小寫需一至)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release handle to the webcam
video_capture.release()
cv2.destroyAllWindows()
