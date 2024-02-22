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

  if (lightsource.light_type == 0)
  {
    // Point light source
    double angle = (rand() / (double)RAND_MAX) * 2 * PI;
    ray.d.px = cos(angle); // Ray's direction
    ray.d.py = sin(angle);

    // Normalize the direction vector
    normalize(&ray.d);
  }
  else if (lightsource.light_type == 1)
  {
    ray.d.px = lightsource.l.d.px; // Ray's direction
    ray.d.py = lightsource.l.d.py;
  }

  ray.inside_out = 0;    // Initially 0 since the ray starts outside an object
  ray.monochromatic = 0; // Initially 0 since the ray is white (from lightsource)
  ray.R = lightsource.R; // Ray colour in RGB must be the same as the lightsource
  ray.G = lightsource.G;
  ray.B = lightsource.B;

  return (ray); // Currently this returns dummy ray
}

void propagateRay(struct ray2D *ray, int depth)
{
  /*
    This function carries out the light propagation process. It is provided with access
    to a ray data structure, and must perform the following steps (in order!):

    - Check if maximum recursion depth has been reached (in which case, it just returns)
    - Find the *closest* intersection between the ray and objects in the scene. This
      means you have to check against the 4 walls, and any circles added in buildScene,
      determine which intersection is closest, and obtain the intersection point, the
      normal at the intersection, and the lambda value at which the intersection happens.
    - Renders the ray onto the image from its starting point all the way up to the
      intersection point.
    - At the intersection, use the material properties to determine how the propagation
      process proceeds:
          * For mirror materials, compute the mirror reflection direction, create a ray
            along that direction whose origin is the intersection point, and propagate
            that ray
          * For scattering materials, choose a random direction within +- 90 degrees of
            the normal, create a ray with that direction with origin at the intersection
            point, and propagate that ray
          * For refracting materials you will need to propagate two rays - one in the
            mirror reflection direction (just like for reflecting materials), and
            another in the refraction direction. Propagate both rays one after the other!

    NOTE: You should only care about intersections for which lambda is POSITIVE (in front
          of the ray), and greater than zero (e.g. if the ray is reflected from some
          object, you do not care about the intersection with the object itself which will
          have a lambda of *very close* to zero)

    In every case, make sure the ray's direction vector has unit length. You will need to
    complete other functions as part of your work here.
 */

  /*********************************************************************************
   * TO DO: Complete this function to implement light propagation!
   ********************************************************************************/

  // Define your local variables here

  if (depth >= max_depth)
    return; // Leave this be, it makes sure you don't
            // recurse forever

  // Step 1 - Find *closest* intersection with the 4 walls (the written part of A1
  //          should help you figure out how to do that.

  // How many walls can the ray intersect? how many walls can the ray intersect in the
  // forward direction?

  struct point2D
      best_intersection;
  double best_lambda = 1e6; // Initialize to a large value
  int material_type;
  for (int wall_id = 0; wall_id < 4; wall_id++)
  {
    // Check for intersection with the wall
    // If there is an intersection, update the closest intersection

    struct wall2D wall = walls[wall_id];
    // Intersect the wall ray with the current ray using wall.w.p and wall.w.d
    // with p(lambda) = ray->p + lambda*ray->d and
    // p_wall(lambda) = wall.w.p + lambda*wall.w.d
    // Solve for lambda_1 and lambda_2 using Cramer's rule
    double lambda_1 = (wall.w.d.py * (wall.w.p.px - ray->p.px) - wall.w.d.px * (wall.w.p.py - ray->p.py)) / (ray->d.px * wall.w.d.py - ray->d.py * wall.w.d.px);
    double lambda_2 = (ray->d.px * (wall.w.p.py - ray->p.py) - ray->d.py * (wall.w.p.px - ray->p.px)) / (ray->d.px * wall.w.d.py - ray->d.py * wall.w.d.px);

    // If lambda_1 and lambda_2 are both positive, then there is an intersection.
    // Note that lambda_2 must be in [0, 1].
    if (lambda_1 > 0 && lambda_2 > 0 && lambda_2 < 1)
    {
      // If the intersection is closer than the current closest intersection, update the closest intersection
      if (lambda_1 < best_lambda)
      {
        best_lambda = lambda_1;
        best_intersection.px = ray->p.px + lambda_1 * ray->d.px;
        best_intersection.py = ray->p.py + lambda_1 * ray->d.py;
        material_type = wall.material_type;
      }
    }
  }

  // Print wall intersection point
  printf("Wall intersection point: (%lf, %lf)\n", best_intersection.px, best_intersection.py);

  // Step 2 - Check for intersection against objects in the object array - you must
  //          complete the intersectRay() function, call it, and obtain the closest
  //          intersection (in the forward ray direction) with objects in the scene.
  //          Note that you must provide variables for intersectRay() to return
  //          the point of intersection, normal at intersection, lambda, material type,
  //          and refraction index for the closest object hit by the ray.
  struct point2D intersect_p;
  struct point2D intersect_n;
  double intersect_lambda;
  int intersect_mat_type;
  double intersect_r_idx;
  intersectRay(ray, &intersect_p, &intersect_n, &intersect_lambda, &intersect_mat_type, &intersect_r_idx);

  // Step 3 - Check whether the closest intersection with objects is closer than the
  //          closest intersection with a wall. Choose whichever is closer.

  if (intersect_lambda < best_lambda)
  {
    best_lambda = intersect_lambda;
    best_intersection = intersect_p;
    material_type = intersect_mat_type;
  }

  // Print the closest intersection point
  printf("Closest intersection point: (%lf, %lf)\n", best_intersection.px, best_intersection.py);

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

  if (material_type == 0)
  {
    // Mirror-reflecting object
    // Compute the mirror reflection direction
    double dot_product = dot(&ray->d, &intersect_n);
    struct point2D reflection_d;
    reflection_d.px = ray->d.px - 2 * dot_product * intersect_n.px;
    reflection_d.py = ray->d.py - 2 * dot_product * intersect_n.py;
    // Create a ray in the mirror reflection direction
    struct ray2D reflection_ray;
    reflection_ray.p = best_intersection;
    reflection_ray.d = reflection_d;
    reflection_ray.R = ray->R;
    reflection_ray.G = ray->G;
    reflection_ray.B = ray->B;
    reflection_ray.inside_out = 0;
    reflection_ray.monochromatic = 0;
    // Propagate the mirror reflection ray
    propagateRay(&reflection_ray, depth + 1);
  }
  else if (material_type == 1)
  {
    // Scattering surface
    // Choose a random direction within +- 90 degrees from the normal
    double angle = (rand() / (double)RAND_MAX) * PI / 2;
    double cos_angle = cos(angle);
    double sin_angle = sin(angle);
    struct point2D scattering_d;
    scattering_d.px = cos_angle * intersect_n.px - sin_angle * intersect_n.py;
    scattering_d.py = sin_angle * intersect_n.px + cos_angle * intersect_n.py;
    // Create a ray in the scattering direction
    struct ray2D scattering_ray;
    scattering_ray.p = best_intersection;
    scattering_ray.d = scattering_d;
    scattering_ray.R = ray->R;
    scattering_ray.G = ray->G;
    scattering_ray.B = ray->B;
    scattering_ray.inside_out = 0;
    scattering_ray.monochromatic = 0;
    // Propagate the scattering ray
    propagateRay(&scattering_ray, depth + 1);
  }
  else if (material_type == 2)
  {
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

  // For each object, check for intersection with the ray

  for (int i = 0; i < MAX_OBJECTS; i++)
  {
    struct circ2D object = objects[i];
    if (object.r == 0)
      continue;
    // Check for intersection with the object
    // If there is an intersection, update the closest intersection
    // If the intersection is closer than the current closest intersection, update the closest intersection

    // Equation: (p + lambda*d)(p + lambda*d) - r^2 = 0
    // A*lambda^2 + B*lambda + C = 0

    double A = dot(&ray->d, &ray->d);
    double B = 2 * dot(&ray->d, &ray->p) - 2 * dot(&ray->d, &object.c);
    double C = dot(&ray->p, &ray->p) - 2 * dot(&ray->p, &object.c) + dot(&object.c, &object.c) - object.r * object.r;

    double discriminant = B * B - 4 * A * C;
    if (discriminant < 0)
      continue;

    double lambda_1 = (-B + sqrt(discriminant)) / (2 * A);
    double lambda_2 = (-B - sqrt(discriminant)) / (2 * A);

    double number_of_intersections = 0;
    if (lambda_1 > 0)
      number_of_intersections++;
    if (lambda_2 > 0)
      number_of_intersections++;

    if (number_of_intersections == 0)
      continue;

    double candidate_lambda;
    if (number_of_intersections == 1)
      candidate_lambda = (lambda_1 > 0) ? lambda_1 : lambda_2;
    else
      candidate_lambda = lambda_1 < lambda_2 ? lambda_1 : lambda_2;

    // If the intersection is closer than the current closest intersection, update the closest intersection
    if (candidate_lambda < *lambda)
    {
      *lambda = candidate_lambda;
      p->px = ray->p.px + candidate_lambda * ray->d.px;
      p->py = ray->p.py + candidate_lambda * ray->d.py;
      n->px = p->px - object.c.px;
      n->py = p->py - object.c.py;
      normalize(n);
      *type = object.material_type;
      *r_idx = object.r_idx;
    }
  }
}
