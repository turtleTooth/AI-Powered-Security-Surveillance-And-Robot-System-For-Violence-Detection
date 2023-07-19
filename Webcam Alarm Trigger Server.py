#!/usr/bin/env python
# coding: utf-8

# In[ ]:


import numpy as np
import argparse
import pickle
import cv2
import os
import time 
import requests
from keras.models import load_model
from collections import deque

ESP32_IP = "192.168.0.108"  # Replace with the IP address of your ESP32

def trigger_alarm():
    # Send a GET request to the ESP32 to trigger the alarm
    url = f"http://{ESP32_IP}/trigger-alarm"
    try:
        response = requests.get(url)
        if response.status_code == 200:
            print("Alarm triggered successfully")
        else:
            print("Failed to trigger the alarm")
    except requests.exceptions.RequestException as e:
        print("Failed to connect to the ESP32:", str(e))

def print_results(video, limit=None):
    if not os.path.exists('output'):
        os.mkdir('output')

    print("Loading model ...")
    
    model = load_model('E:/499/499.h5', compile=False)

    Q = deque(maxlen=128)
    vs = cv2.VideoCapture(video)
    
    writer = None
    (W, H) = (None, None)
    count = 0
    consecutive_count = 0  # Counter for consecutive frames with violence
    threshold = 1  # Number of consecutive frames required to trigger red label
    violence_detected = False  # Flag indicating if violence is detected
    
    while True:
        (grabbed, frame) = vs.read()
        if not grabbed:
            break
        if W is None or H is None:
            (H, W) = frame.shape[:2]

        output = frame.copy()

        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frame = cv2.resize(frame, (128, 128)).astype("float32")
        frame = frame.reshape(128, 128, 3) / 255

        preds = model.predict(np.expand_dims(frame, axis=0))[0]

        Q.append(preds)

        results = np.array(Q).mean(axis=0)
        i = (preds > 0.50)[0]
        label = i

        if label:
            consecutive_count += 1
        else:
            consecutive_count = 0

        if consecutive_count >= threshold:
            violence_detected = True
        else:
            violence_detected = False

        if violence_detected:
            trigger_alarm()
            text = "Violence: True"
            text_color = (0, 0, 255)  # Red color for text
        else:
            text = "Violence: False"
            text_color = (0, 255, 0)  # Green color for text
        
        FONT = cv2.FONT_HERSHEY_SIMPLEX 
        cv2.putText(output, text, (35, 50), FONT, 1.25, text_color, 3)

        if writer is None:
            fourcc = cv2.VideoWriter_fourcc(*"MJPG")
            writer = cv2.VideoWriter("output/v_output.avi", fourcc, 30, (W, H), True)

        writer.write(output)
        cv2.imshow('Webcam', output)

        key = cv2.waitKey(1) & 0xFF

        if key == ord("q"):
            break

    print("[INFO] Cleaning up...")
    writer.release()
    vs.release()
    cv2.destroyAllWindows()

print_results(0)

