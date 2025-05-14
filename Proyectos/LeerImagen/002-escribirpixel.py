from PIL import Image
import time

# Start timer
start = time.time()

# Load the image
image = Image.open("Proyectos/LeerImagen/image.jpg").convert("RGB")
width, height = image.size
pixels = list(image.getdata())

# Convert to grayscale using median
def median_gray(rgb):
    return sorted(rgb)[1]  # median of R, G, B

gray_pixels = [median_gray(pixel) for pixel in pixels]

# Create a new grayscale image
gray_image = Image.new("L", (width, height))
gray_image.putdata(gray_pixels)
gray_image.save("imagenfinal.jpg", "JPEG", quality=100)

# End timer
elapsed = time.time() - start
print(f"Grayscale image saved as 'imagenfinal.jpg'")
print(f"Elapsed time: {elapsed:.4f} seconds")
