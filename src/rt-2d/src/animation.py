from math import cos, sin, pi
import os
import sys

head_start = 148

for angle in range(head_start, 360, 1):
    # Use angle to take points on a circle with radius 0.1
    rads = angle * pi / 180
    x = cos(rads) * 0.2
    y = sin(rads) * 0.2

    # Open buildSceneSample.c to read
    f = open('buildSceneSample.c', 'r')

    # Open buildScene.c to write (overwrite)
    f2 = open('buildScene.c', 'w')

    # Copy the contents of buildSceneSample.c to buildScene.c, while replacing the strings:
    # ${LIGHT_Y} with x
    # ${LIGHT_Z} with y

    for line in f:
        f2.write(line.replace('${LIGHT_X}', str(x)
                              ).replace('${LIGHT_Y}', str(y)))

    # Close the files
    f.close()
    f2.close()

    # Compile using ./compile.sh synchronously
    os.system('./compile.sh')

    # Run the following: ./light2D 1024 1024 5000000 25
    # Make sure the process finishes before continuing to the next iteration
    # Redirect stdout to /dev/null to suppress output
    os.system('./light2D 1024 1024 2500000 25 > /dev/null')

    # Convert light2D_output.ppm to generated_images/animation_{seq_num}.png
    os.system(f'convert light2D_output.ppm generated_images/animation_{angle}.png')

    print(f'Generated image {angle} of 360')
