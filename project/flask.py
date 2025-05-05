from flask import Flask, request, render_template, jsonify
from datetime import datetime
import os
import shutil
import subprocess

app = Flask(__name__)

# Create folders to store uploads and human detections
UPLOAD_FOLDER = 'uploads'
HUMAN_FOLDER = 'humans'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)
os.makedirs(HUMAN_FOLDER, exist_ok=True)

# Route to serve the index.html page
@app.route('/')
def index():
    return render_template('index.html')

# Route to handle image uploads
@app.route('/upload', methods=['POST'])
def upload_file():
    # Save image to uploads/latest.jpg
    filename = "latest.jpg"
    filepath = os.path.join(UPLOAD_FOLDER, filename)

    with open(filepath, 'wb') as f:
        f.write(request.data)

    print(f"[INFO] Image saved to {filepath}")

    # Run h3.py detection script
    result = subprocess.run(['python3', 'humandetection.py'], capture_output=True, text=True)
    output = result.stdout
    print(output)

    # Check for human detection in output
    if "Human Detected" in output:
        timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
        human_path = os.path.join(HUMAN_FOLDER, f"human_{timestamp}.jpg")
        shutil.copy(filepath, human_path)
        print(f"[INFO] Human image saved to {human_path}")

    return "Image received and processed", 200

# Route to return detection status
@app.route('/status', methods=['GET'])
def get_status():
    # Check if human was detected
    if os.path.exists('humans/latest.jpg'):
        status_message = "Human Detected"
    else:
        status_message = "No detection"

    return jsonify({"message": status_message}), 200

# Main entry point for the Flask app
if __name__ == '__main__':
    print("✅ Flask server is starting...")
    app.run(host='0.0.0.0', port=5000)

# 
# 
# 