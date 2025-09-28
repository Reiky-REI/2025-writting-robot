from PIL import Image
import sys

def process_image(input_path, output_path):
    # Open the image and convert it to grayscale
    img = Image.open(input_path).convert("L")
    
    # Process each pixel: set to 0 if brightness is lower than 235
    pixels = img.load()
    for x in range(img.width):
        for y in range(img.height):
            if pixels[x, y] < 242:
                pixels[x, y] = 0

    # Save the processed image
    img.save(output_path)
    print(f"Processed image saved to {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python line.py <input_image> <output_image>")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    process_image(input_file, output_file)