import cv2
from urllib import request

fire_cascade = cv2.CascadeClassifier('C:/Users/srkms/Desktop/499 Serial/fire_detection_cascade_model.xml')                                                                      

vid = cv2.VideoCapture(0)
runOnce = False
		
while(True):
    Alarm_Status = False
    ret, frame = vid.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    fire = fire_cascade.detectMultiScale(frame, 1.2, 5)

    for (x,y,w,h) in fire:
        cv2.rectangle(frame,(x-20,y-20),(x+w+20,y+h+20),(255,0,0),2)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+w]

        request.urlopen("http://192.168.166.83/"+"/L")
        print("Fire Detected")

    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
