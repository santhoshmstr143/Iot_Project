from ultralytics import YOLO
import cv2
from twilio.rest import Client
import os
from datetime import datetime
import shutil
import matplotlib.pyplot as plt

# 📂 Use local image path from Flask
img_path = "uploads/latest.jpg"

# 🔐 Twilio credentials
account_sid = "AC7760831b433c477857b0186b92cdfe47"
auth_token = "c9b650d285673ae98baba8ce26409c49"
client = Client(account_sid, auth_token)

# 🎯 Load a more accurate YOLO model
model = YOLO("yolov8s.pt")  # ⬅️ switch from 'n' to 's' for better accuracy

# ✅ Load image using OpenCV
img = cv2.imread(img_path)
if img is None:
    print("❌ Failed to load image.")
    exit()

# 🔍 Run detection
results = model(img, verbose=False)[0]  # process OpenCV image directly

# ✅ Confidence threshold
CONFIDENCE_THRESHOLD = 0.4

# Flags
human_detected = False
animal_detected = False

# 🧠 Analyze detections
for box in results.boxes:
    class_id = int(box.cls[0])
    class_name = model.names[class_id]
    conf = float(box.conf[0])

    if conf < CONFIDENCE_THRESHOLD:
        continue

    print(f"🔍 Detected: {class_name} ({conf:.2f})")

    if class_name.lower() == "person":
        human_detected = True
    elif class_name.lower() in [
        "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "deer",
        "monkey", "lion", "tiger", "leopard", "panda",
        "kangaroo", "fox", "wolf", "crocodile"
    ]:
        animal_detected = True

# 🖼️ Show detection results (for debugging only)
annotated = results.plot()
cv2.imshow("YOLOv8 Detection Results", annotated)
cv2.waitKey(2000)  # Wait 2 seconds (2000 milliseconds)
cv2.destroyAllWindows()

# 📢 Final Decision
if human_detected:
    print("🧍 Human Detected!")

    # 🔔 Twilio call
    call = client.calls.create(
        twiml='<Response><Say>Alert! A poacher has been detected in the forest. Please act immediately!</Say></Response>',
        to='+918074455075',
        from_='+19207892513'
    )
    print("📞 Twilio alert sent!")

    # 💾 Save to humans/
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    os.makedirs("humans", exist_ok=True)
    shutil.copy(img_path, f"humans/human_{timestamp}.jpg")
    shutil.copy(img_path, "humans/latesthuman.jpg")
    print(f"📁 Image saved to humans/human_{timestamp}.jpg")

elif animal_detected:
    print("🐅 Animal Detected — no alert needed.")
else:
    print("🌲 Nothing detected — all clear.")

# 📝 Append result to log file
log_file = "detection_log.csv"
log_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

if human_detected:
    status = "Human Detected"
elif animal_detected:
    status = "Animal Detected"
else:
    status = "No detection"

with open(log_file, "a") as f:
    f.write(f"{log_time},{status}\n")
