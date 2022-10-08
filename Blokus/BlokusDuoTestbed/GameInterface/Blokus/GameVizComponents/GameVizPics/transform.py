
import Image, os

def transform(inputFile, outputFile, srcColor, destColor):
    img = Image.open(inputFile)
    for x in range(img.size[0]):
        for y in range(img.size[1]):
            cColor = img.getpixel((x,y))
            if abs(srcColor[0] - cColor[0]) <=20 and \
               abs(srcColor[1] - cColor[1]) <=20 and \
               abs(srcColor[2] - cColor[2]) <=20:
                img.putpixel((x,y), destColor)
    img.save(outputFile)

if __name__ == '__main__':
    srcColor = (45, 45, 126, 255)
    #srcColor = (53, 59, 158, 255)
    dstColor = (50, 0, 0, 255)
    #srcColor = (53, 59, 158, 255)
    #dstColor = (53, 59, 158, 255)
    #dstColor = (163, 0,   0, 255)
    for f in [x for x in os.listdir('.') if x.startswith('red')]:
        #outputFile = f.replace('blue', 'red')
        print ('process', f)
        transform(f, f, srcColor, dstColor)
    

    #srcColor = (36, 36, 106)
    
