from math import cos, sin, pi
import os
import sys

head_start = 0
limit = 360

def sample_point_on_circle(radius, angle):
    rads = angle * pi / 180
    x = cos(rads) * radius
    y = sin(rads) * radius
    return (x, y)

for angle in range(head_start, limit + 1, 1):
    # Use angle to take points on a circle with radius 0.1
    x, y = sample_point_on_circle(0.3, angle)
    print(f'x: {x}, y: {y}')

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
    os.system('./light2D 1024 1024 250000 25 > /dev/null')

    # Convert light2D_output.ppm to generated_images/000.png, 001.png, 002.png, ...
    os.system(f'convert light2D_output.ppm generated_images/{angle:03d}.png')

    print(f'Generated image {angle} of 360')
