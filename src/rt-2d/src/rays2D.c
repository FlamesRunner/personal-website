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
// #define __DEBUG_MODE
#include "light2D.h"

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
 * @brief Rotates a 2D point by some number of radians.
 *
 * @param point 2D point
 * @param radians Radians to rotate by
 */
void rotate2DPoint(struct point2D *point, double radians)
{
  double new_px = point->px * cos(radians) - point->py * sin(radians);
  double new_py = point->py * cos(radians) + point->px * sin(radians);
  point->px = new_px;
  point->py = new_py;
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
 * @brief Get the angle between the two rays, assuming
 * that the two lines originate from the same point and
 * are not parallel.
 *
 * @param ln1 First line
 * @param ln2 Second line
 * @return Angle
 */
double getAngleBetweenLines(struct ray2D *ln1, struct ray2D *ln2)
{
  double dot_product = ln1->d.px * ln2->d.px + ln1->d.py * ln2->d.py;
  double ln1_magnitude = ln1->d.px * ln1->d.px + ln1->d.py * ln1->d.py;
  double ln2_magnitude = ln2->d.px * ln2->d.px + ln2->d.py * ln2->d.py;
  double angle = acos(dot_product * (ln1_magnitude * ln2_magnitude));
  return angle;
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

struct point2D getWallNormal(int wall_idx)
{
  struct point2D n1;
  switch (wall_idx)
  {
  case 0:
    n1.px = 0.0;
    n1.py = 1.0;
    break;
  case 1:
    n1.px = -1.0;
    n1.py = 0.0;
    break;
  case 2:
    n1.px = 0.0;
    n1.py = -1.0;
    break;
  case 3:
    n1.px = 1.0;
    n1.py = 0.0;
    break;
  }

  return n1;
}

/**
 * @brief Attempts to find the closest wall intn. The function is guaranteed to return
 * a valid result, and will set the POI, the normal of the intersected wall, the lambda
 * value, the wall's ID and the material type. Uses distance to determine the closest
 * wall.
 *
 * If the function detects a corner intersection, it will return the average of
 * the two normals given by the intersecting walls. It will also choose a wall ID
 * and material type at random.
 *
 * @param ray Ray to intersect against wall
 * @param p Point of intersection. In cases of error, can be (inf, inf).
 * @param n  Normal vector.
 * @param lambda Lambda value of wall ray.
 * @param wall_id Wall ID relative to walls array.
 * @param material_type Material type of wall.
 */
void intersectRayWall(struct ray2D *ray, struct point2D *p, struct point2D *n, double *lambda, int *wall_id, int *material_type)
{
  struct point2D closest_poi;
  double closest_dist = INFINITY;
  double wall_lambda = 0.0;
  int has_found_point = 0;

  // Set default
  closest_poi.px = INFINITY;
  closest_poi.py = INFINITY;

  for (int wall_idx = 0; wall_idx < 4; wall_idx++)
  {
    struct ray2D wall_ray = walls[wall_idx].w;
    double coeff[4] = {ray->d.px, -1.0 * wall_ray.d.px, ray->d.py, -1.0 * wall_ray.d.py};
    double b[2] = {wall_ray.p.px - ray->p.px, wall_ray.p.py - ray->p.py};
    double solutions[2];
    solve2By2Matrix(coeff, b, solutions);

    // Ensure lambda from ray is positive
    // solutions[0] is the lambda for the ray, solutions[1] is the lambda for the wall
    if (solutions[0] < TOL || solutions[1] > 1 - TOL)
    {
      continue;
    }

    struct point2D poi;
    computePointOnLine(ray, solutions[0], &poi);

    double current_dist = dist2D(&poi, &ray->p);

    if (current_dist < closest_dist)
    {
      closest_dist = current_dist;
      closest_poi = poi;
      wall_lambda = solutions[1];
      *wall_id = wall_idx;
      has_found_point = 1;
      *material_type = walls[wall_idx].material_type;
    }

    if (current_dist < TOL)
    {
      // Potential corner intersection
      struct point2D current_wall_p = wall_ray.p;
      struct point2D next_wall_p = walls[(wall_idx + 1) % 4].w.p;
      struct point2D prev_wall_p = walls[(wall_idx + 3) % 4].w.p;

      if (dist2D(&current_wall_p, &closest_poi) < TOL && dist2D(&next_wall_p, &closest_poi) < TOL)
      {
        // Force normal to be average of the two normals
        struct point2D n1 = getWallNormal(wall_idx);
        struct point2D n2 = getWallNormal((wall_idx + 1) % 4);
        n->px = (n1.px + n2.px) / 2.0;
        n->py = (n1.py + n2.py) / 2.0;
        *lambda = wall_lambda;
        *p = closest_poi;

        // Randomly choose wall ID of the two walls
        int wall_choices[2] = {wall_idx, (wall_idx + 1) % 4};
        *wall_id = wall_choices[rand() % 2];
        *material_type = walls[*wall_id].material_type;

        // Perturb the point slightly
        p->px += (rand() / (RAND_MAX / 0.01)) - 0.005;
        p->py += (rand() / (RAND_MAX / 0.01)) - 0.005;

        // Perturb the lambda slightly
        *lambda += (rand() / (RAND_MAX / 0.01)) - 0.005;
        return;
      }
      else if (dist2D(&current_wall_p, &closest_poi) < TOL && dist2D(&prev_wall_p, &closest_poi) < TOL)
      {
        // Force normal to be average of the two normals
        struct point2D n1 = getWallNormal(wall_idx);
        struct point2D n2 = getWallNormal((wall_idx + 3) % 4);
        n->px = (n1.px + n2.px) / 2.0;
        n->py = (n1.py + n2.py) / 2.0;
        *lambda = wall_lambda;
        *p = closest_poi;

        // Randomly choose wall ID of the two walls
        int wall_choices[2] = {wall_idx, (wall_idx + 3) % 4};
        *wall_id = wall_choices[rand() % 2];
        *material_type = walls[*wall_id].material_type;

        // Perturb the point slightly
        p->px += (rand() / (RAND_MAX / 0.01)) - 0.005;
        p->py += (rand() / (RAND_MAX / 0.01)) - 0.005;

        // Perturb the lambda slightly
        *lambda += (rand() / (RAND_MAX / 0.01)) - 0.005;
        return;
      }
    }
  }

  // If a point was found, use the convention [TOP, RIGHT, BOTTOM, LEFT] to get normal
  if (has_found_point == 1)
  {
    *n = getWallNormal(*wall_id);
  }

  *lambda = wall_lambda;
  *p = closest_poi;
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
  double ptAngle = (rand() / (RAND_MAX / 360.0)) * PI / 180.0;
  switch (lightsource.light_type)
  {
  case 0: // point light source; get random direction

    rayDirX = cos(ptAngle);
    rayDirY = sin(ptAngle);
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
  double R = 1;
  double G = 1;
  double B = 1;
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
  double intersect_R, intersect_G, intersect_B;
  intersectRay(ray, &intersect_p, &intersect_n, &intersect_lambda, &intersect_mat_type, &intersect_r_idx, &intersect_R, &intersect_G, &intersect_B);

  // Step 3 - Check whether the closest intersection with objects is closer than the
  //          closest intersection with a wall. Choose whichever is closer.
  double dist_wall = dist2D(&best_intersection, &ray->p);
  double dist_obj = dist2D(&intersect_p, &ray->p);

  // Ensure intersect_lambda is within [0, 1] and that it is closer than the wall
  if (intersect_lambda > 0 && dist_obj < dist_wall)
  {
    best_intersection = intersect_p;
    best_normal = intersect_n;
    best_lambda = intersect_lambda;
    material_type = intersect_mat_type;
    R = intersect_R;
    G = intersect_G;
    B = intersect_B;
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

  if (fabs(ray->p.px) + fabs(ray->p.py) > fabs(W_LEFT) + fabs(W_TOP) - CORNER_TOL)
  {
    // We ignore the ray, as we are in a corner
    return;
  }

  if (fabs(best_intersection.px) + fabs(best_intersection.py) > fabs(W_LEFT) + fabs(W_TOP) - CORNER_TOL)
  {
    // We ignore the ray, as the intersection is invalid (corner case likely)
    return;
  }

  renderRay(&ray->p, &best_intersection, ray->R, ray->G, ray->B);

  if (ray->inside_out == 1)
  {
    best_normal.px *= -1.0;
    best_normal.py *= -1.0;
  }

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
    reflected_ray->R = ray->R * R;
    reflected_ray->G = ray->G * G;
    reflected_ray->B = ray->B * B;

    // Normalize
    normalize(&reflected_ray->d);

    propagateRay(reflected_ray, depth + 1);

    // Free memory
    free(reflected_ray);
  }
  else if (material_type == 1)
  {
    // Handle scattering. Ensure that this does not
    // enter an object

    struct point2D scattered_ray_dir;
    double angle = (rand() / (RAND_MAX / 180.0)) - 90.0;
    angle = angle * PI / 180.0;
    vectorRotate(&best_normal, angle, &scattered_ray_dir);

    struct ray2D *scattered_ray = (struct ray2D *)malloc(sizeof(struct ray2D));
    scattered_ray->p = best_intersection;
    scattered_ray->d = scattered_ray_dir;
    scattered_ray->inside_out = ray->inside_out;
    scattered_ray->monochromatic = ray->monochromatic;
    scattered_ray->R = ray->R * R;
    scattered_ray->G = ray->G * G;
    scattered_ray->B = ray->B * B;

    // Normalize
    normalize(&scattered_ray->d);

    propagateRay(scattered_ray, depth + 1);

    // Free memory
    free(scattered_ray);
  }
  else if (material_type == 2)
  {
    // Case 2 can only occur with objects (refracting)
    struct ray2D *reflected_ray = (struct ray2D *)malloc(sizeof(struct ray2D));
    double n1, n2, r0, r_theta, r_transmitted;

    // Figure out angle
    struct ray2D normal_ray;
    normal_ray.p = best_intersection;
    normal_ray.d = best_normal;

    struct ray2D reversed_ray;
    reversed_ray.p = best_intersection;
    reversed_ray.d.px = ray->d.px * -1.0;
    reversed_ray.d.py = ray->d.py * -1.0;
    normalize(&reversed_ray.d);

    double angle_between_normal = getAngleBetweenLines(&normal_ray, &reversed_ray);
    if (ray->inside_out == 1)
    { // In object
      n1 = intersect_r_idx;
      n2 = 1.0;
      normal_ray.d.px *= -1.0;
      normal_ray.d.py *= -1.0;
    }
    else
    { // In air
      n1 = 1.0;
      n2 = intersect_r_idx;
    }

    r0 = pow((n1 - n2) / (n1 + n2), 2);
    r_theta = r0 + (1 - r0) * pow((1.0 - fabs(cos(angle_between_normal))), 5);
    r_transmitted = 1.0 - r_theta;
    double refracted_angle = asin((n1 * sin(angle_between_normal)) / n2);

    // Create refracted ray
    struct ray2D *refracted_ray = (struct ray2D *)calloc(1, sizeof(struct ray2D));
    refracted_ray->p = best_intersection;
    refracted_ray->d.px = best_normal.px * -1.0;
    refracted_ray->d.py = best_normal.py * -1.0;
    refracted_ray->inside_out = (ray->inside_out == 1) ? 0 : 1;

    rotate2DPoint(&refracted_ray->d, refracted_angle);

    refracted_ray->R = ray->R * r_transmitted * R;
    refracted_ray->G = ray->G * r_transmitted * G;
    refracted_ray->B = ray->B * r_transmitted * B;

    // Create mirror-direction ray
    double dot_product = ray->d.px * best_normal.px + ray->d.py * best_normal.py;
    reflected_ray->d.px = -2.0 * dot_product * best_normal.px + ray->d.px;
    reflected_ray->d.py = -2.0 * dot_product * best_normal.py + ray->d.py;
    reflected_ray->R *= r_theta * R;
    reflected_ray->G *= r_theta * G;
    reflected_ray->B *= r_theta * B;

    // Optimization in case we have light rays that are black
    if (refracted_ray->R > TOL && refracted_ray->G > TOL && refracted_ray->B > TOL)
    {
      propagateRay(refracted_ray, depth + 1);
    }
    free(refracted_ray);
    free(reflected_ray);
  }
}

void intersectRay(struct ray2D *ray, struct point2D *p, struct point2D *n, double *lambda, int *type, double *r_idx, double *Rv, double *Gv, double *Bv)
{
  /*
   This function checks for intersection between the ray and any objects in the objects
   array. The objects are circles, so we are in fact solving for the intersection
   between a ray and a circle.
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
      if (solution[j] <= TOL)
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

        // Set the colour
        *(Rv) = closest_object.R;
        *(Gv) = closest_object.G;
        *(Bv) = closest_object.B;
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