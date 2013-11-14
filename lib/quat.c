#include "quat.h"
#include "stdio.h"
#include "matrix.h"

double
quat_length2(Quat v)
{
  return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}

const double epsilon = 0.0000001;

Quat
quat_identity()
{
  Quat q = { .x = 0, .y = 0, .z = 0, .w = 1 };
  return q;
}

void
quat_set_identity(Quat* q)
{
  q->x = q->y = q->z = 0;
  q->w = 1;
}


Quat
quat_angle_axis(double angle, Vec3 axis)
{
  double length = vec3_length(axis);
  if (length < epsilon) {
    return quat_identity();
  }

  double inverse_norm = 1/length;
  double cos_half_angle = cos(0.5f*angle);
  double sin_half_angle = sin(0.5f*angle);

  Quat q = { 
    .x = axis.x * sin_half_angle * inverse_norm,
    .y = axis.y * sin_half_angle * inverse_norm,
    .z = axis.z * sin_half_angle * inverse_norm,
    .w = cos_half_angle};

  return q;
}

Quat
quat_mul(Quat ql, Quat qr)
{
  Quat q = { 
    .x = qr.w*ql.x + qr.x*ql.w + qr.y*ql.z - qr.z*ql.y,
    .y = qr.w*ql.y - qr.x*ql.z + qr.y*ql.w + qr.z*ql.x,
    .z = qr.w*ql.z + qr.x*ql.y - qr.y*ql.x + qr.z*ql.w,
    .w = qr.w*ql.w - qr.x*ql.x - qr.y*ql.y - qr.z*ql.z
  };
  return q;
}

Quat
quat_add(Quat ql, Quat qr)
{
  Quat q = { 
    .x = ql.x+qr.x,
    .y = ql.y+qr.y,
    .z = ql.z+qr.z,
    .w = ql.w+qr.w,
  };

  return q;
}


Vec3 
quat_rotate_vec3(Quat q, Vec3 v)
{
  Vec3 uv, uuv;
  Vec3 qvec = {.x = q.x, .y = q.y, .z = q.z} ;
  uv = vec3_cross(qvec, v);
  uuv = vec3_cross(qvec, uv);
  uv = vec3_mul(uv, -2.0f*q.w);
  uuv = vec3_mul(uuv, 2.0f);
  return vec3_add(v, vec3_add(uv, uuv));
}


Quat
quat_conj(Quat q)
{
  Quat r = {
    .x = -q.x, 
    .y = -q.y,
    .z = -q.z,
    .w = q.w
  };

  return r;
}

Quat
quat_inverse(Quat q)
{
  float l = quat_length2(q);
  Quat r = {
    .x = -q.x/l, 
    .y = -q.y/l,
    .z = -q.z/l,
    .w = q.w/l
  };

  return r;
}

Quat 
quat_between_quat(Quat q1, Quat q2)
{
  return quat_mul(quat_conj(q1),q2);
  //return quat_mul(q2,quat_conj(q1));
  //return quat_mul(q1,quat_conj(q2));
  //return quat_mul(q2,quat_inverse(q1));
}

Quat
quat_mul_scalar(Quat q, float s)
{
  Quat r = {
    .x = q.x*s, 
    .y = q.y*s,
    .z = q.z*s,
    .w = q.w*s
  };

  return r;

}

Quat
quat_slerp(Quat from, Quat to, float t)
{
  double omega, cosomega, sinomega, scale_from, scale_to ;

  Quat quatTo = to;
  //printf("  quatto : %f, %f, %f, %f\n", quatTo.x, quatTo.y, quatTo.z, quatTo.w);
  cosomega = vec4_dot(from, to);

  if ( cosomega <0.0 ) { 
    cosomega = -cosomega; 
    quatTo = quat_mul_scalar(to, -1); //quatTo = -to;
  }

  if( (1.0 - cosomega) > epsilon ){
    omega= acos(cosomega) ;  // 0 <= omega <= Pi (see man acos)
    sinomega = sin(omega) ;  // this sinomega should always be +ve so
    // could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
    scale_from = sin((1.0-t)*omega)/sinomega ;
    scale_to = sin(t*omega)/sinomega ;
   } else {
     // --------------------------------------------------
     //   The ends of the vectors are very close
     //   we can use simple linear interpolation - no need
     //   to worry about the "spherical" interpolation
     //   --------------------------------------------------
     scale_from = 1.0 - t ;
     scale_to = t ;
   }

  Quat q = quat_add(quat_mul_scalar(from, scale_from),quat_mul_scalar(quatTo,scale_to));

  return q;
}

Vec4
quat_to_axis_angle(Quat q)
{
  Vec4 r;
  float sinhalfangle = sqrt( q.x*q.x + q.y*q.y + q.z*q.z );

  r.w = 2.0 * atan2( sinhalfangle, q.w );
  if(sinhalfangle)
   {
    r.x = q.x / sinhalfangle;
    r.y = q.y / sinhalfangle;
    r.z = q.z / sinhalfangle;
   }
  else
   {
    r.x = 0.0;
    r.y = 0.0;
    r.z = 1.0;
   }

  return r;
}

Quat
quat_between_vec(Vec3 from, Vec3 to)
{
  //TODO looks like from and to are switched...
  printf("TODO --warning-- check this function 'quat_between_vec', looks like from and to are switched\n");
  Vec3 sourceVector = from;
  Vec3 targetVector = to;

  double fromLen2 = vec3_length2(sourceVector);
  double fromLen;
  // normalize only when necessary, epsilon test
  if ((fromLen2 < 1.0-1e-7) || (fromLen2 > 1.0+1e-7)) {
    fromLen = sqrt(fromLen2);
    sourceVector = vec3_mul(sourceVector, 1.0f/fromLen); //sourceVector /= fromLen;
  } else fromLen = 1.0;

  double toLen2 = vec3_length2(to);
  // normalize only when necessary, epsilon test
  if ((toLen2 < 1.0-1e-7) || (toLen2 > 1.0+1e-7)) {
    double toLen;
    // re-use fromLen for case of mapping 2 vectors of the same length
    if ((toLen2 > fromLen2-1e-7) && (toLen2 < fromLen2+1e-7)) {
      toLen = fromLen;
    } 
    else toLen = sqrt(toLen2);
    targetVector = vec3_mul(targetVector, 1.0f/toLen);// targetVector /= toLen;
  }
  
  

  // Now let's get into the real stuff
  // Use "dot product plus one" as test as it can be re-used later on
  double dotProdPlus1 = 1.0 + vec3_dot(sourceVector, targetVector);

  Quat q;

  // Check for degenerate case of full u-turn. Use epsilon for detection
  if (dotProdPlus1 < 1e-7) {

    // Get an orthogonal vector of the given vector
    // in a plane with maximum vector coordinates.
    // Then use it as quaternion axis with pi angle
    // Trick is to realize one value at least is >0.6 for a normalized vector.
    if (fabs(sourceVector.x) < 0.6) {
      //const double norm = sqrt(1.0 - sourceVector.x() * sourceVector.x());
      const double norm = sqrt(1.0 - sourceVector.x * sourceVector.x);
      q.x = 0.0; 
      q.y = sourceVector.z / norm;
      q.z = -sourceVector.y / norm;
      q.w = 0.0;
    } else if (fabs(sourceVector.y) < 0.6) {
      const double norm = sqrt(1.0 - sourceVector.y * sourceVector.y);
      q.x = -sourceVector.z / norm;
      q.y = 0.0;
      q.z = sourceVector.x / norm;
      q.w = 0.0;
    } else {
      const double norm = sqrt(1.0 - sourceVector.z * sourceVector.z);
      q.x = sourceVector.y / norm;
      q.y = -sourceVector.x / norm;
      q.z = 0.0;
      q.w = 0.0;
    }
  }
  else {
    // Find the shortest angle quaternion that transforms normalized vectors
    // into one other. Formula is still valid when vectors are colinear
    const double s = sqrt(0.5 * dotProdPlus1);
    //const Vec3d tmp = sourceVector ^ targetVector / (2.0*s);
    const Vec3 tmp = vec3_mul(vec3_cross(sourceVector, targetVector), 1.0 / (2.0*s));
    q.x = tmp.x;
    q.y = tmp.y;
    q.z = tmp.z;
    q.w = s;
  }


  return q;


  return quat_identity();

}

Vec3 
quat_rotate_around_angles(Vec3 pivot, Vec3 mypoint, float yaw, float pitch)
{
  mypoint = vec3_sub(mypoint, pivot);

  Quat rotx = quat_identity();
  Quat roty = quat_identity();

  if (yaw != 0) {
    Vec3 axis = {0, 1, 0};
    rotx = quat_angle_axis(yaw, axis);
    //printf("yaw %f \n", yaw);
    mypoint = quat_rotate_vec3(rotx, mypoint);
  }

  if (pitch != 0) {
    Vec3 axis = {1, 0, 0};
    roty = quat_angle_axis(pitch, axis);
    //printf("yaw %f \n", yaw);
    mypoint = quat_rotate_vec3(roty, mypoint);
  }

  mypoint = vec3_add(mypoint, pivot);

  return mypoint;
}

Quat 
quat_lookat(Vec3 from, Vec3 to, Vec3 up)
{
  printf("TODO --warning-- check this function 'quat_lookat', looks wrong\n");
  Matrix4 la;
  mat4_lookat(la, from, to, up);
  return mat4_get_quat(la);
}

Vec3 quat_rotate_around(Quat q, Vec3 pivot, Vec3 mypoint)
{
  mypoint = vec3_sub(mypoint, pivot);
  mypoint = quat_rotate_vec3(q, mypoint);
  mypoint = vec3_add(mypoint, pivot);

  return mypoint;

}

Vec3
quat_to_euler(Quat q)
{
  Vec3 v = {
    atan2(2*(q.w*q.x + q.y*q.z), 1 - 2*(q.x*q.x + q.y*q.y)),
    asin(2*(q.w*q.y - q.z*q.x)),
    atan2(2*(q.w*q.z + q.x*q.y), 1- 2*(q.y*q.y + q.z*q.z))
  };
  return v;
}

Vec3
quat_to_euler_deg(Quat q)
{
  Vec3 v = quat_to_euler(q);
  v = vec3_mul(v, 180.0/M_PI);
  return v;
}

Quat
quat_yaw_pitch_roll_rad(double yaw, double pitch, double roll)
{
  Quat qy = quat_angle_axis(yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(pitch, vec3(1,0,0));
  Quat qr = quat_angle_axis(roll, vec3(0,0,1));

  Quat q1 =  quat_mul(qy, qp);
  return quat_mul(q1, qr);
}

Quat
quat_yaw_pitch_roll_deg(double yaw, double pitch, double roll)
{
  double r = M_PI/180.0;

  return quat_yaw_pitch_roll_rad(
        yaw*r,
        pitch*r,
        roll*r);
}

Quat quat_angles_rad(Vec3 angles)
{
  Quat qx = quat_angle_axis(angles.x, vec3(1,0,0));
  Quat qy = quat_angle_axis(angles.y, vec3(0,1,0));
  Quat qz = quat_angle_axis(angles.z, vec3(0,0,1));

  Quat q1 =  quat_mul(qx, qy);
  return quat_mul(q1, qz);
}

Quat quat_angles_deg(Vec3 angles)
{
  double r = M_PI/180.0;

  return quat_angles_rad(vec3_mul(angles, r));
}

bool
quat_equal(Quat q1, Quat q2)
{
  return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
}
