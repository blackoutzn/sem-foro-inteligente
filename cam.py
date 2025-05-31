import cv2
import serial
from ultralytics import YOLO
import time

try:
    arduino = serial.Serial('COM4', 9600)
    print("Conectado ao Arduino na porta COM4")
except:
    print("Erro ao conectar ao Arduino")

time.sleep(2)

model = YOLO("yolov8n.pt")

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Erro ao conectar à câmera")
    exit()

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

frame_width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
frame_height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
print(f"Resolução da câmera: {frame_width}x{frame_height}")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Erro ao capturar o frame!")
        break
    
    
  

    frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    frame_gray = cv2.convertScaleAbs(frame_gray, alpha=2.0, beta=50)
    frame = cv2.cvtColor(frame_gray, cv2.COLOR_GRAY2BGR)


    results = model(frame)

    carros_detectados = 0

    if results:
        boxes = results[0].boxes.xyxy.cpu().numpy()
        confidences = results[0].boxes.conf.cpu().numpy()
        class_ids = results[0].boxes.cls.cpu().numpy()

        for box, confidence, class_id in zip(boxes, confidences, class_ids):
            if int(class_id) == 2:
                carros_detectados += 1
                x1, y1, x2, y2 = box
                label = f"{model.names[int(class_id)]} ({confidence:.2f})"
                cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 3)
                cv2.putText(frame, label, (int(x1), int(y1) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

    cv2.imshow("Detecção do Carro", frame)

    if arduino:
        arduino.write(f"{carros_detectados}\n".encode())

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
arduino.close()
cv2.destroyAllWindows()