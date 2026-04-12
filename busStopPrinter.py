from PIL import Image, ImageDraw, ImageFont
from typing import List
 
  

def display_times(next_predictions: List[dict]):
    # 
    text_lines = [f"{pred['route']} {pred['direction']:<9} {pred['minutes']:>2}m" for pred in next_predictions]
    generate_image(text_lines)



# Given a list of text lines, generate the image
def generate_image(text: List[str]):
    text_height = 8
    display_width = 128
    display_height = 32

    img = Image.new('RGB', (display_width, display_height), color=(0, 0, 0))
    draw = ImageDraw.Draw(img)

    for line_idx in range(0, len(text)):
        line = text[line_idx]
        draw.text((0,line_idx*text_height), line, font=ImageFont.truetype("PressStart2P.ttf", text_height), fill=(255,255,255))


    img.save("test_out.png")