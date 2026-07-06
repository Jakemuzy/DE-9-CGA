import sys
import numpy as np
from PIL import Image  # Ensure pillow is installed: pip install pillow

CGA_PALETTE = {
    0x00: (0, 0, 0),       # Black
    0x01: (0, 0, 170),     # Blue
    0x02: (0, 170, 0),     # Green
    0x03: (0, 170, 170),   # Cyan
    0x04: (170, 0, 0),     # Red
    0x05: (170, 0, 170),   # Magenta
    0x06: (170, 85, 0),    # Brown
    0x07: (170, 170, 170), # Light Gray
    0x08: (85, 85, 85),    # Dark Gray
    0x09: (85, 85, 255),   # Light Blue 👈 (Our target circle ring color)
    0x0A: (85, 255, 85),   # Light Green
    0x0B: (85, 255, 255),  # Light Cyan
    0x0C: (255, 85, 85),   # Light Red
    0x0D: (255, 85, 255),  # Light Magenta
    0x0E: (255, 255, 85),  # Yellow
    0x0F: (255, 255, 255), # Bright White
}

WIDTH, HEIGHT = 640, 200

def get_cga_color(pixel_byte):
    cga_token = pixel_byte & 0x0F
    return CGA_PALETTE.get(cga_token, (0, 0, 0))

def listen_and_render():
    print("Reading streaming console data from STDIN... Press Ctrl+C to stop.")
    inside_frame = False
    frame_lines = []

    try:
        for raw_line in sys.stdin:
            print(raw_line, end="") # Keeps your standard ESP_LOG visible
            line = raw_line.strip()
            
            if "---START_FRAME---" in line:
                inside_frame = True
                frame_lines = []
                continue
                
            if "---END_FRAME---" in line:
                inside_frame = False
                if len(frame_lines) >= HEIGHT:
                    process_frame(frame_lines)
                    print("\n[SUCCESS] Image saved to cga_output.png! You can exit via Ctrl+C.")
                continue
                
            if inside_frame and line:
                if all(c in "0123456789ABCDEFabcdef" for c in line[:10]):
                    frame_lines.append(line)
    except KeyboardInterrupt:
        print("\nExiting render engine.")

def process_frame(lines):
    # Create a raw byte array matching our screen bounds (Width * Height * RGB channels)
    img_data = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
    
    for y, line in enumerate(lines[:HEIGHT]):
        pixels = [int(line[i:i+2], 16) for i in range(0, len(line), 2)]
        for x, pixel_byte in enumerate(pixels[:WIDTH]):
            img_data[y, x] = get_cga_color(pixel_byte)

    # Use Pillow to construct a true pixel image and write it to disk
    img = Image.fromarray(img_data, 'RGB')
    img.save('cga_output.png')

if __name__ == "__main__":
    listen_and_render()

