/* Define structs and functions for 2D ray tracing. */
typedef struct RGB {
    double r, g, b;
} RGB;

typedef struct Vector {
    double x, y;
} Vector;

typedef struct Ray {
    Vector origin;
    Vector direction;
    double lambda;
} Ray;

typedef struct Object {
    Vector centre;
    Vector scale;
    double rotation;
    RGB colour;

    double (*findIntersection) (Ray *ray, Object *object); // Returns lambda. If no intersection, returns -1
    Vector (*findNormal) (Ray *ray, Object *object); // Returns normal vector. If no intersection, returns NULL
} Object;

typedef struct ObjectList {
    Object *object;
    struct ObjectList *next;
} ObjectList;

/* Define RT functions */

// Get closest intersection. Takes ray and object list pointers, and sets lambda and object pointers to closest intersection.
// If no intersection, sets lambda to -1 and object to NULL.
void findClosestIntersection(Ray *ray, ObjectList *objectList, double *lambda, Vector *hit_position, Vector *normal, Object **hit_object);

// Trace ray. Takes ray and object list pointers, and sets colour to the colour of the ray.
void rayTrace(Ray *ray, ObjectList *objectList, int depth, RGB *colour);

// Create new circle
Object *newCircle(Vector centre, double radius);
double circleIntersection(Ray *ray, Object *circle);
Vector findCircleNormal(Ray *ray, Object *circle);

// Create new rectangle
Object *newRectangle(Vector centre, double width, double height);
double rectangleIntersection(Ray *ray, Object *rectangle);
Vector findRectangleNormal(Ray *ray, Object *rectangle);

