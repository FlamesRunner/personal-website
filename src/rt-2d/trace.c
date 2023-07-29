#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "trace.h"

ObjectList *ol;

double circleIntersection(Ray *ray, Object *circle) {
    

}

Vector findCircleNormal(Ray *ray, Object *circle) {

}

Object *newCircle(Vector centre, double radius) {
    // We create a new object
    Object *object = malloc(sizeof(Object));

    // We set the object intersection function to circleIntersection and
    // the normal function to findCircleNormal
    object->findIntersection = &circleIntersection;
    object->findNormal = &findCircleNormal;

    // Set the object data
    object->centre = centre;

    // Define scale vector
    Vector scale = {radius, radius};
    object->scale = scale;

    // Return the object
    return object;
}

double rectangleIntersection(Ray *ray, Object *rectangle) {

}

Vector findRectangleNormal(Ray *ray, Object *rectangle) {

}

// Create new rectangle
Object *newRectangle(Vector centre, double width, double height) {
    // We create a new object
    Object *object = malloc(sizeof(Object));

    // We set the object intersection function to rectangleIntersection and
    // the normal function to findRectangleNormal
    object->findIntersection = &rectangleIntersection;
    object->findNormal = &findRectangleNormal;

    // Set the object data
    object->centre = centre;

    // Define scale vector
    Vector scale = {width, height};
    object->scale = scale;

    // Return the object
    return object;
}



void findClosestIntersection(Ray *ray, ObjectList *objectList, double *lambda, Vector *hit_position, Vector *normal, Object **hit_object) {

}

void rayTrace(Ray *ray, ObjectList *objectList, int depth, RGB *colour) {

}

void initScene() {
    // Initialize the object list

}

int main(int argc, char *argv[]) {


    return 0;
}


