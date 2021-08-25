
人臉辨識-門禁系統
=
相關材料及環境系統
-
* Window10
* python版本 3.6.9
* NodeMCU 
* MG90伺服馬達
* 羅技視訊鏡頭
* Line Notify 權杖 

Python 引用套件(執行前須先安裝)
-
* Face Recognition 
* OpenCV 
* numpy  
* paho-mqtt 
* time 
* requests 


 Main為程式主體，執行後 會開啟鏡頭 並開始偵測人臉，倘若偵測到的人臉有在資料庫內，則會驅動伺服馬達</br>
 以下為需要注意的點:
 
 _讀取鏡頭位置(一般預設為0，若沒有特別更換不用動)_

        video_capture = cv2.VideoCapture(0)

 _建立人臉資料夾(放上自己人臉的正面照)，指定路徑需要正確位置 ((請自行更新))_
        
        chin_image = face_recognition.load_image_file("face_image\\chin.jpg")
        chin_face_encoding = face_recognition.face_encodings(chin_image)[0]
        
_建立已知臉孔代碼以及已知臉孔代碼的對應姓名_

        known_face_encodings = [
           chin_face_encoding, 
        ]
        known_face_names = [
           "Chin"
        ]
        
**上述 使用chin為我自己的姓名，可以更改，但要記得更改後的名稱**        
 
 
