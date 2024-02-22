/*
  CSC D18 - Assignment 1 - 2D light propagation

  This is the place where you will be doing most of your work for solving this
  assignment. Before you start working here, you shold have become familiar
  with the data structures and functions available to you from light2D.h, and
  with the way a scene is built in buildScene.c

  Go over each of the functions below, and implement the different components
  of the solution in the sections marked

  /************************
  / TO DO:
  ************************ /

  Do not add or modify code outside these sections.

  Details about what needs to be implemented are described in the comments, as
  well as in the assignment handout. You must read both carefully.

  Starter by: F.J. Estrada, Aug. 2017
*/

/****************************************************************************
 * Uncomment the #define below to enable debug code, add whatever you need
 * to help you debug your program between #ifdef - #endif blocks
 * ************************************************************************/
#define __DEBUG_MODE

/**
 * @brief Dot product
 * @param p1 Point 1
 * @param p2 Point 2
 * @return double Dot product
 */
double dotProduct(struct point2D *p1, struct point2D *p2)
{
  return p1->px * p2->px + p1->py * p2->py;
}

/**
 * @brief Subtracts two vectors
 * @param a Vector 1
 * @param b Vector 2
 * @return struct point2D Result
 */
struct point2D vectorSubtract(struct point2D *a, struct point2D *b)
{
  struct point2D result;
  result.px = a->px - b->px;
  result.py = a->py - b->py;
  return result;
}

/**
 * @brief Rotates a vector by a given angle
 * @param v Vector
 * @param angle Angle
 * @param point2D* Result
 */
void vectorRotate(struct point2D *v, double angle, struct point2D *result)
{
  result->px = v->px * cos(angle) - v->py * sin(angle);
  result->py = v->px * sin(angle) + v->py * cos(angle);
}

/**
 * @brief Scales a vector by a given factor
 * @param v Vector
 * @param factor Factor
 * @param point2D* Result
 */
void vectorScale(struct point2D *v, double factor, struct point2D *result)
{
  result->px = v->px * factor;
  result->py = v->py * factor;
}

/**
 * @brief Computes point on a ray.
 *
 * @param ray Ray object
 * @param lambda Lambda value
 * @param result Point result
 */
void computePointOnLine(struct ray2D *ray, double lambda, struct point2D *result)
{
  result->px = ray->p.px + ray->d.px * lambda;
  result->py = ray->p.py + ray->d.py * lambda;
}

/**
 * @brief Solves a quadratic equation ax^2 + bx + c = 0's real numbered solutions,
 * providing in soln_count the number of real-valued solutions, and in soln the
 * value of the solutions.
 *
 * @param a Constant prefixing x^2
 * @param b Constant prefixing x
 * @param c Constant value
 * @param soln Pointer to double array of size 2 to store solutions
 * @param soln_count Pointer to integer to store solution count
 */
void solveQuadratic(double a, double b, double c, double *soln, int *soln_count)
{
  double discriminant = b * b - 4.0 * a * c;
  if (discriminant < 0)
  {
    *soln_count = 0;
    return;
  };

  double discriminant_sqrt = sqrt(discriminant);

  soln[0] = (-b + discriminant_sqrt) / (2.0 * a);
  *soln_count = 1;
  if (fabs(discriminant) < 10e-8)
  {
    return;
  }

  soln[1] = (-b - discriminant_sqrt) / (2.0 * a);
  *soln_count = 2;
}

/**
 * @brief Uses Cramer's rule to figure out the solution to a 2 by 2 matrix (equation of the form Ax=b,
 * where A is a R^{2,2} matrix, x is a R^2 vector and x is an unknown R^2 vector (solution).
 *
 * @param A
 * @param b
 * @param x
 */
void solve2By2Matrix(double *A, double *b, double *x)
{
  double det = A[0] * A[3] - A[1] * A[2];
  x[0] = (b[0] * A[3] - b[1] * A[1]) / det;
  x[1] = (b[1] * A[0] - b[0] * A[2]) / det;
}

/**
 * @brief Calculates Euclidean distance for a pair of 2D points.
 *
 * @param p1 Point 1
 * @param p2 Point 2
 * @return double Distance
 */
double dist2D(struct point2D *p1, struct point2D *p2)
{
  double x_component = p1->px - p2->px;
  double y_component = p1->py - p2->py;
  return sqrt(x_component * x_component + y_component * y_component);
}

/**
 * @brief Attempts to find the closest wall intn. The function is guaranteed to return
 * a valid result, and will set the POI, the normal of the intersected wall, the lambda
 * value, the wall's ID and the material type. Uses distance to determine the closest
 * wall.
 *
 * @param ray Ray to intersect against wall
 * @param p Point of intersection. In cases of error, can be (inf, inf).
 * @param n  Normal vector.
 * @param lambda Lambda value of wall ray.
 * @param wall_id Wall ID relative to walls array.
 */
void intersectRayWall(struct ray2D *ray, struct point2D *p, struct point2D *n, double *lambda, int *wall_id, int *material)
{
  struct point2D best_intersection;
  double best_lambda = INFINITY;
  int best_wall_id = -1;
  int best_material = -1;

  for (u_int32_t i = 0; i < 4; i++)
  {
    struct ray2D wall = walls[i].w;
    struct point2D wall_p = wall.p;
    struct point2D wall_d = wall.d;

    double A[4] = {ray->d.px, -wall_d.px, ray->d.py, -wall_d.py};
    double b[2] = {wall_p.px - ray->p.px, wall_p.py - ray->p.py};

    double x[2];
    solve2By2Matrix(A, b, x);

    if (x[0] < TOL || x[0] > 1 - TOL || x[1] < TOL || x[1] > 1 - TOL)
    {
      continue;
    }

    double lambda = x[1];
    struct point2D poi;
    computePointOnLine(&wall, lambda, &poi);

    if (!(poi.px > W_LEFT - TOL && poi.px < W_RIGHT + TOL && poi.py > W_TOP - TOL && poi.py < W_BOTTOM + TOL))
    {
      continue;
    }

    double dist = dist2D(&poi, &ray->p);

    if (dist < best_lambda)
    {
      best_intersection = poi;
      best_lambda = dist;
      best_wall_id = i;
      best_material = walls[i].material_type;
    }
  }

  *p = best_intersection;
  *lambda = best_lambda;
  *wall_id = best_wall_id;
  *material = best_material;

  // Compute normal
  struct ray2D wall = walls[best_wall_id].w;
  struct point2D wall_d = wall.d;
  n->px = -wall_d.py;
  n->py = wall_d.px;
  normalize(n);
}

struct ray2D makeLightSourceRay(void)
{
  /*
    This function should return a light ray that has its origin at the light
    source, and whose direction depends on the type of light source.

    For point light sources (which emit light in all directions) the direction
     has to be chosen randomly and uniformly over a unit circle (i.e. any
     direction is equally likely)

    For a laser type light source, the direction of the ray is exactly the same
     as the direction of the lightsource.

    Set the colour of the ray to the same colour as the light source, and
     set the inside_outside flag to 0 (we assume light sources are
     outside objects)

    In either case, the direction vector *must be unit length*.
 */

  struct ray2D ray;

  ray.p.px = lightsource.l.p.px; // Ray's origin
  ray.p.py = lightsource.l.p.py;

  double rayDirX, rayDirY = 0.0;

  switch (lightsource.light_type)
  {
  case 0: // point light source
    rayDirX = rand() / (RAND_MAX / 2.0) - 1.0;
    rayDirY = rand() / (RAND_MAX / 2.0) - 1.0;
    break;
  case 1: // laser light source
    rayDirX = lightsource.l.d.px;
    rayDirY = lightsource.l.d.py;
    break;
  }

  ray.d.px = rayDirX; // Ray's direction
  ray.d.py = rayDirY;
  ray.inside_out = 0;    // Initially 0 since the ray starts outside an object
  ray.monochromatic = 0; // Initially 0 since the ray is white (from lightsource)
  ray.R = lightsource.R; // Ray colour in RGB must be the same as the lightsource
  ray.G = lightsource.G;
  ray.B = lightsource.B;

  return (ray);
}

void propagateRay(struct ray2D *ray, int depth)
{
  if (depth >= max_depth)
    return;

  // Step 1 - Find *closest* intersection with the 4 walls (the written part of A1
  //          should help you figure out how to do that.
  struct point2D
      best_intersection,
      best_normal;
  double best_lambda = 1e6; // Initialize to a large value
  int material_type;
  int wall_id;
  intersectRayWall(ray, &best_intersection, &best_normal, &best_lambda, &wall_id, &material_type);

  // Step 2 - Check for intersection against objects in the object array - you must
  //          complete the intersectRay() function, call it, and obtain the closest
  //          intersection (in the forward ray direction) with objects in the scene.
  //          Note that you must provide variables for intersectRay() to return
  //          the point of intersection, normal at intersection, lambda, material type,
  //          and refraction index for the closest object hit by the ray.
  struct point2D intersect_p;
  struct point2D intersect_n;
  double intersect_lambda = INFINITY;
  int intersect_mat_type;
  double intersect_r_idx;
  intersectRay(ray, &intersect_p, &intersect_n, &intersect_lambda, &intersect_mat_type, &intersect_r_idx);

  // Step 3 - Check whether the closest intersection with objects is closer than the
  //          closest intersection with a wall. Choose whichever is closer.
  double dist_wall = dist2D(&best_intersection, &ray->p);
  double dist_obj = dist2D(&intersect_p, &ray->p);

  if (depth == 11)
  {
    // print intersection points
    printf("Wall intersection point: (%lf, %lf)\n", best_intersection.px, best_intersection.py);
    printf("Object intersection point: (%lf, %lf)\n", intersect_p.px, intersect_p.py);
  }

  // Ensure intersect_lambda is within [0, 1] and that it is closer than the wall
  if (intersect_lambda > 0 && dist_obj < dist_wall)
  {
    best_intersection = intersect_p;
    best_normal = intersect_n;
    best_lambda = intersect_lambda;
    material_type = intersect_mat_type;
  }

// Print the closest intersection point
#ifdef __DEBUG_MODE
  printf("Closest intersection point: (%lf, %lf)\n", best_intersection.px, best_intersection.py);
  printf("Wall intersection point: (%lf, %lf)\n", best_intersection.px, best_intersection.py);
  printf("Object intersection point: (%lf, %lf)\n", intersect_p.px, intersect_p.py);
  printf("Distance to wall: %lf, Distance to object: %lf\n", dist_wall, dist_obj);
  printf("Normal: (%lf, %lf)\n", best_normal.px, best_normal.py);
  printf("Ray direction: (%lf, %lf)\n", ray->d.px, ray->d.py);
  printf("Case : %d\n", material_type);

  printf("====================================\n");
#endif

  // Step 4 - Render the ray onto the image. Use renderRay(). Provide renderRay() with
  //          the origin of the ray, and the intersection point (it will then draw a
  //          ray from the origin to the intersection). You also need to provide the
  //          ray's colour.

  renderRay(&ray->p, &best_intersection, ray->R, ray->G, ray->B);

  // Step 5 - Decide how to handle the ray's bounce at the intersection. You will have
  //          to provide code for 3 cases:
  //          If material type = 0, you have a mirror-reflecting object.
  //                                Create a ray in the mirror reflection direction,
  //                                with the same colour as the incoming ray, and
  //                                with origin at the intersection point.
  //                                Then call propagateRay() recursively to trace it.
  //          if material type = 1, you have a scattering surface.
  //                                Choose a random direction within +- 90 degrees
  //                                from the normal at the intersection. Create a
  //                                ray in this direction, with the same colour as
  //                                the incoming ray, and origin at the intersection,
  //                                then call propagateRay() recursively to trace it.
  //          if material type = 2, you have a refracting (transparent) material.
  // 				   Here you need to process two rays:
  //                                * First, determine how much of the incoming light is
  //                                  reflected and how much is transmitted, using
  //				     Schlick's approximation:
  // 					 R0 = ((n1-n2)/(n1+n2))^2
  // 					 R(theta)=R0+((1-R0)*(1-cos(theta))^5)
  //				     If the ray is travelling from air to the inside
  //                                  of an object, n1=1, n2=object's index of refraction.
  //                                  If the ray is travelling from inside an object
  //                                  back onto air, n1=object's index of refraction, n2=1
  //				     And 'theta' is the angle between the normal and the
  // 				     ray direction.
  //				     R(theta) gives the amount Rs of reflected light,
  //				     1.0-R(theta) gives the amount Rt of transmitted light.
  //                                * Now, make a ray in the mirror-reflection direction
  //				     (same as for material type 0), with the same colour
  //				     as the incoming ray, but with intensity modulated
  //				     by Rs. (e.g. if the incoming's colour is R,G,B,
  //                                  the reflected ray's colour will be R*Rs, G*Rs, B*Rs)
  //				     trace this ray.
  //				   * Make a ray in the refracted-ray direction. The
  //				     angle for the transmitted ray is given by Snell's law
  //				     n1*sin(theta1) = n2*sin(theta2). The colour of the
  //				     transmitted ray is the same as the incoming ray but
  //			             modulated by Rt. Trace this ray.
  //	That's it! you're done!

  // Handle reflections

  if (material_type == 0)
  {
    struct point2D reflected_ray_dir;
    reflected_ray_dir.px = ray->d.px - 2 * dotProduct(&ray->d, &best_normal) * best_normal.px;
    reflected_ray_dir.py = ray->d.py - 2 * dotProduct(&ray->d, &best_normal) * best_normal.py;

    struct ray2D *reflected_ray = (struct ray2D *)malloc(sizeof(struct ray2D));
    reflected_ray->p = best_intersection;
    reflected_ray->d = reflected_ray_dir;
    reflected_ray->inside_out = ray->inside_out;
    reflected_ray->monochromatic = ray->monochromatic;
    reflected_ray->R = ray->R;
    reflected_ray->G = ray->G;
    reflected_ray->B = ray->B;

    // Normalize
    normalize(&reflected_ray->d);

    // Print reflected ray
    printf("Reflected ray: (%lf, %lf)\n", reflected_ray->d.px, reflected_ray->d.py);

    propagateRay(reflected_ray, depth + 1);
  }
  else if (material_type == 1)
  {
    // // Handle scattering
    // double angle = (rand() / (RAND_MAX / 180.0)) - 90.0;
    // struct point2D scattered_ray_dir;
    // vectorRotate(&best_normal, angle, &scattered_ray_dir);

    // struct ray2D scattered_ray;
    // scattered_ray.p = best_intersection;
    // scattered_ray.d = scattered_ray_dir;
    // scattered_ray.inside_out = ray->inside_out;
    // scattered_ray.monochromatic = ray->monochromatic;
    // scattered_ray.R = ray->R;
    // scattered_ray.G = ray->G;
    // scattered_ray.B = ray->B;

    // propagateRay(&scattered_ray, depth + 1);

    // Handle scattering
    double angle = (rand() / (RAND_MAX / 180.0)) - 90.0;
    struct point2D scattered_ray_dir;
    vectorRotate(&best_normal, angle, &scattered_ray_dir);

    struct ray2D *scattered_ray = (struct ray2D *)malloc(sizeof(struct ray2D));
    scattered_ray->p = best_intersection;
    scattered_ray->d = scattered_ray_dir;
    scattered_ray->inside_out = ray->inside_out;
    scattered_ray->monochromatic = ray->monochromatic;
    scattered_ray->R = ray->R;
    scattered_ray->G = ray->G;
    scattered_ray->B = ray->B;

    // Normalize
    normalize(&scattered_ray->d);

    propagateRay(scattered_ray, depth + 1);
  }
}

void intersectRay(struct ray2D *ray, struct point2D *p, struct point2D *n, double *lambda, int *type, double *r_idx)
{
  /*
   This function checks for intersection between the ray and any objects in the objects
   array. The objects are circles, so we are in fact solving for the intersection
   between a ray and a circle.

   For a unit circle centered at the origin, we would have the equation

   x^2 + y^2 = 1

   Using vector notation, with C=[x y]', we get

   ||C||^2 = 1

   A point on the ray is given by p + lambda*d

   Substituting in the equation for the circle we have

   (p + lambda*d)(p + lambda*d) - 1 = 0

   If we expand the product above (here the product of two vectors is a DOT product),
   we can form a quadratic equation

   A*lambda^2 + B*lambda + C = 0

   Which as you know, has a very simple solution.

   Your task is to
   * Figure out A, B, and C, considering that your circles don't necessarily have r=1
   * Figure out how to deal with the fact that circles in the scene are likely
     *not* centered at the origin

   Then implement the code that will find the value(s) of lambda at the intersection(s).

   Note that you can have no intersections, 1 intersection, or 2 intersections

   This function *must* find the closest intersection (if any) and update the value
   of lambda, the intersection point p, the normal n at the intersection,
   the corresponding object's material type (needed outside here to figure out how
   to handle the light's bouncing off this object), and the index of refraction for
   the object (needed if this is a transparent object).

   You must make sure that n is a unit-length vector.
  */

  struct circ2D closest_object;
  double closest_object_distance = INFINITY;
  struct point2D closest_object_poi;
  double ray_lambda = 0.0;
  int has_found_point = 0;

  // For each object
  for (u_int32_t i = 0; i < MAX_OBJECTS; i++)
  {
    struct circ2D current = objects[i];

    // Check if object is active
    if (current.r < TOL)
    {
      continue;
    }

    // Get A, B and C to solve eqn
    double A = dotProduct(&ray->d, &ray->d);
    struct point2D p_minus_c = vectorSubtract(&ray->p, &current.c);
    double B = 2 * dotProduct(&ray->d, &p_minus_c);
    double C = dotProduct(&ray->p, &ray->p) - 2 * dotProduct(&ray->p, &current.c) + dotProduct(&current.c, &current.c) - current.r * current.r;

    double solution[2];
    int solution_count;

    solveQuadratic(A, B, C, solution, &solution_count);

#ifdef __DEBUG_MODE
    // printf("Solution count: %d\n", solution_count);
    // printf("Solution 1: %lf\n", solution[0]);
    // printf("Solution 2: %lf\n", solution[1]);
#endif

    struct point2D poi;
    for (u_int32_t j = 0; j < solution_count; j++)
    {
      if (solution[j] < TOL)
      {
        continue;
      }

      computePointOnLine(ray, solution[j], &poi);

      // Check if point is within constraints of the window
      if (!(poi.px > W_LEFT - TOL && poi.px < W_RIGHT + TOL && poi.py > W_TOP - TOL && poi.py < W_BOTTOM + TOL))
      {
        continue;
      }

      double dist = dist2D(&poi, &ray->p);

      if (dist < closest_object_distance)
      {
        // We found a closer object
        closest_object = current;
        closest_object_poi = poi;
        closest_object_distance = dist;
        ray_lambda = solution[j];
        has_found_point = 1;
      }
    }
  }

  if (has_found_point == 1)
  { // We found some object
    // Calculate the normal
    n->px = closest_object_poi.px - closest_object.c.px;
    n->py = closest_object_poi.py - closest_object.c.py;
    // Check if we need to flip the normal

    // printf("Normal: (%lf, %lf)\n", n->px, n->py);

    normalize(n);

    *lambda = ray_lambda;
    *type = closest_object.material_type;
    *r_idx = closest_object.r_idx;
    *p = closest_object_poi;
  }
  else
  { // Otherwise, set the POI to (inf, inf) to indicate no point was found
    *lambda = -1;
    p->px = INFINITY;
    p->py = INFINITY;
  }
}