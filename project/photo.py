from flask import Flask, render_template_string, send_from_directory
import os

app = Flask(__name__)

IMAGE_FOLDER = 'humans'  # Updated folder name

@app.route('/')
def gallery():
    image_extensions = ('.png', '.jpg', '.jpeg', '.gif', '.webp')
    images = [img for img in os.listdir(IMAGE_FOLDER) if img.lower().endswith(image_extensions)]

    with open('gallery.html', 'r') as file:
        html = file.read()
    
    return render_template_string(html, images=images)

# Serve images from the humans folder
@app.route('/humans/<filename>')
def serve_image(filename):
    return send_from_directory(IMAGE_FOLDER, filename)

if __name__ == '__main__':
    app.run(debug=True, port=5001)  # Change port here (e.g., 5001)
