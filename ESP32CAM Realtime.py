#!/usr/bin/env python
# coding: utf-8

# In[1]:


import numpy as np
import argparse
import pickle
import cv2
import os
import time
from keras.models import load_model
from collections import deque
import requests
import urllib

def print_results(video, limit=None):
    if not os.path.exists('output'):
        os.mkdir('output')

    print("Loading model ...")
    model = load_model('E:/499/499.h5', compile=False)

    Q = deque(maxlen=128)
    consistent_frames = 0  # Number of consistent frames
    consistent_threshold = 10  # Threshold for consistent frames
    
    # Open the video stream from IP camera
    stream = requests.get(video, stream=True)
    bytes = b''  # Initialize bytes as an empty byte array
    
    # Set up the video writer
    writer = None
    (W, H) = (None, None)
    count = 0
    
    for chunk in stream.iter_content(chunk_size=1024):
        bytes += chunk
        a = bytes.find(b'\xff\xd8')
        b = bytes.find(b'\xff\xd9')
        
        if a != -1 and b != -1:
            jpg = bytes[a:b+2]
            bytes = bytes[b+2:]
            
            try:
                # Decode the frame
                frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
                
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
                
                text_color = (0, 255, 0)
                
                if label:
                    consistent_frames += 1
                    if consistent_frames >= consistent_threshold:
                        text = "Violence: True"
                        text_color = (0, 0, 255)  # Set label color to red
                else:
                    consistent_frames = 0
                    text = "Violence: False"
                    
                FONT = cv2.FONT_HERSHEY_SIMPLEX
                cv2.putText(output, text, (35, 50), FONT, 1.25, text_color, 3)

                if writer is None:
                    fourcc = cv2.VideoWriter_fourcc(*"MJPG")
                    writer = cv2.VideoWriter("output/v_output.avi", fourcc, 30, (W, H), True)

                writer.write(output)
                cv2.imshow('Real_feed', output)

                key = cv2.waitKey(1) & 0xFF

                if key == ord("q"):
                    break
                    
            except Exception as e:
                print(f"Error decoding frame: {e}")
                
    print("[INFO] cleaning up...")
    writer.release()
    cv2.destroyAllWindows()

print_results('http://192.168.0.107:81/stream')


# In[ ]:




