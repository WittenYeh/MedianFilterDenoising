import argparse
import numpy as np
from PIL import Image
import random

def add_salt_and_pepper_noise(image, prob, noise_size):
    """
    Add salt and pepper noise to an image with a specified noise block size
    :param image: Input image (PIL.Image)
    :param prob: Noise probability (between 0 and 1)
    :param noise_size: Side length of each noise block
    :return: Image with added noise (PIL.Image)
    """
    # Convert the image to a NumPy array (RGB image)
    image_np = np.array(image)
    output = np.copy(image_np)
    
    # Get image width and height
    h, w = image_np.shape[:2]

    # Total number of pixels to be modified (based on probability)
    total_pixels = h * w
    num_salt = int(total_pixels * prob / 2)
    num_pepper = int(total_pixels * prob / 2)

    # Add salt noise (white pixels, value [255, 255, 255] for RGB)
    for _ in range(num_salt):
        x = random.randint(0, h - noise_size)
        y = random.randint(0, w - noise_size)
        output[x:x+noise_size, y:y+noise_size] = [255, 255, 255]  # Apply noise block

    # Add pepper noise (black pixels, value [0, 0, 0] for RGB)
    for _ in range(num_pepper):
        x = random.randint(0, h - noise_size)
        y = random.randint(0, w - noise_size)
        output[x:x+noise_size, y:y+noise_size] = [0, 0, 0]  # Apply noise block

    return Image.fromarray(output)

def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Add salt and pepper noise to an image with customizable noise block size")
    parser.add_argument("input_image", type=str, help="Input image file path")
    parser.add_argument("output_image", type=str, help="Output image file path")
    parser.add_argument("--prob", type=float, default=0.1, help="Salt and pepper noise probability (default: 0.05)")
    parser.add_argument("--size", type=int, default=1, help="Size of the noise block (default: 1 pixel)")

    args = parser.parse_args()

    # Open the image (without converting to grayscale)
    image = Image.open(args.input_image)

    # Add salt and pepper noise
    noisy_image = add_salt_and_pepper_noise(image, prob=args.prob, noise_size=args.size)

    # Save the output image
    noisy_image.save(args.output_image)
    print(f"Image with salt and pepper noise saved to {args.output_image}")

if __name__ == "__main__":
    main()
