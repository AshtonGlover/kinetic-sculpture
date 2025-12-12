import cv2
import serial
import time

ser = serial.Serial('/dev/cu.usbmodemB081849E65602', 9600, timeout=1)
time.sleep(2) 

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Cannot open camera")
    exit()

prev_x = None
speed_scale = 5 

print("Tracking hand. Press 'q' to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1) 

    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_skin = (0, 30, 60)
    upper_skin = (20, 150, 255)
    mask = cv2.inRange(hsv, lower_skin, upper_skin)
    mask = cv2.GaussianBlur(mask, (5, 5), 0)

    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        hand = max(contours, key=cv2.contourArea)
        x, y, w, h = cv2.boundingRect(hand)
        cx = x + w // 2

        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
        cv2.circle(frame, (cx, y + h // 2), 5, (0, 0, 255), -1)

        if prev_x is not None:
            dx = cx - prev_x
            motor_speed = int(dx * speed_scale)
            motor_speed = max(-255, min(255, motor_speed))

            ser.write(f"{motor_speed}\n".encode('utf-8'))

            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8').strip()
                print("Arduino echoed:", line, "Original speed:", motor_speed)

        prev_x = cx

    cv2.imshow('Hand Tracking', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
ser.close()
