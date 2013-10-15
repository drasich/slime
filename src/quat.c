#include "quat.h"
#include "stdio.h"
#include "matrix.h"

double
quat_length2(Quat v)
{
  return v.X*v.X + v.Y*v.Y + v.Z*v.Z + v.W*v.W;
}

const double epsilon = 0.0000001;

Quat
quat_identity()
{
  Quat q = { .X = 0, .Y = 0, .Z = 0, .W = 1 };
  return q;
}

void
quat_set_identity(Quat* q)
{
  q->X = q->Y = q->Z = 0;
  q->W = 1;
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
    .X = axis.X * sin_half_angle * inverse_norm,
    .Y = axis.Y * sin_half_angle * inverse_norm,
    .Z = axis.Z * sin_half_angle * inverse_norm,
    .W = cos_half_angle};

  return q;
}

Quat
quat_mul(Quat ql, Quat qr)
{
  Quat q = { 
    .X = qr.W*ql.X + qr.X*ql.W + qr.Y*ql.Z - qr.Z*ql.Y,
    .Y = qr.W*ql.Y - qr.X*ql.Z + qr.Y*ql.W + qr.Z*ql.X,
    .Z = qr.W*ql.Z + qr.X*ql.Y - qr.Y*ql.X + qr.Z*ql.W,
    .W = qr.W*ql.W - qr.X*ql.X - qr.Y*ql.Y - qr.Z*ql.Z
  };
  return q;
}

Quat
quat_add(Quat ql, Quat qr)
{
  Quat q = { 
    .X = ql.X+qr.X,
    .Y = ql.Y+qr.Y,
    .Z = ql.Z+qr.Z,
    .W = ql.W+qr.W,
  };

  return q;
}


Vec3 
quat_rotate_vec3(Quat q, Vec3 v)
{
  Vec3 uv, uuv;
  Vec3 qvec = {.X = q.X, .Y = q.Y, .Z = q.Z} ;
  uv = vec3_cross(qvec, v);
  uuv = vec3_cross(qvec, uv);
  uv = vec3_mul(uv, -2.0f*q.W);
  uuv = vec3_mul(uuv, 2.0f);
  return vec3_add(v, vec3_add(uv, uuv));
}


Quat
quat_conj(Quat q)
{
  Quat r = {
    .X = -q.X, 
    .Y = -q.Y,
    .Z = -q.Z,
    .W = q.W
  };

  return r;
}

Quat
quat_inverse(Quat q)
{
  float l = quat_length2(q);
  Quat r = {
    .X = -q.X/l, 
    .Y = -q.Y/l,
    .Z = -q.Z/l,
    .W = q.W/l
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
    .X = q.X*s, 
    .Y = q.Y*s,
    .Z = q.Z*s,
    .W = q.W*s
  };

  return r;

}

Quat
quat_slerp(Quat from, Quat to, float t)
{
  double omega, cosomega, sinomega, scale_from, scale_to ;

  Quat quatTo = to;
  //printf("  quatto : %f, %f, %f, %f\n", quatTo.X, quatTo.Y, quatTo.Z, quatTo.W);
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


  //TODO quat add 
  Quat q = quat_add(quat_mul_scalar(from, scale_from),quat_mul_scalar(quatTo,scale_to));
  //*this = (from*scale_from) + (quatTo*scale_to);
  // so that we get a Vec4

  return q;
}

Vec4
quat_to_axis_angle(Quat q)
{
  Vec4 r;
  float sinhalfangle = sqrt( q.X*q.X + q.Y*q.Y + q.Z*q.Z );

  r.W = 2.0 * atan2( sinhalfangle, q.W );
  if(sinhalfangle)
   {
    r.X = q.X / sinhalfangle;
    r.Y = q.Y / sinhalfangle;
    r.Z = q.Z / sinhalfangle;
   }
  else
   {
    r.X = 0.0;
    r.Y = 0.0;
    r.Z = 1.0;
   }

  return r;
}

Quat
quat_between_vec(Vec3 from, Vec3 to)
{
  Vec3 sourceVector = from;
  Vec3 targetVector = to;

  double fromLen2 = vec3_length2(from);
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
    if (fabs(sourceVector.X) < 0.6) {
      //const double norm = sqrt(1.0 - sourceVector.x() * sourceVector.x());
      const double norm = sqrt(1.0 - sourceVector.X * sourceVector.X);
      q.X = 0.0; 
      q.Y = sourceVector.Z / norm;
      q.Z = -sourceVector.Y / norm;
      q.W = 0.0;
    } else if (fabs(sourceVector.Y) < 0.6) {
      const double norm = sqrt(1.0 - sourceVector.Y * sourceVector.Y);
      q.X = -sourceVector.Z / norm;
      q.Y = 0.0;
      q.Z = sourceVector.X / norm;
      q.W = 0.0;
    } else {
      const double norm = sqrt(1.0 - sourceVector.Z * sourceVector.Z);
      q.X = sourceVector.Y / norm;
      q.Y = -sourceVector.X / norm;
      q.Z = 0.0;
      q.W = 0.0;
    }
  }
  else {
    // Find the shortest angle quaternion that transforms normalized vectors
    // into one other. Formula is still valid when vectors are colinear
    const double s = sqrt(0.5 * dotProdPlus1);
    //const Vec3d tmp = sourceVector ^ targetVector / (2.0*s);
    const Vec3 tmp = vec3_mul(vec3_cross(sourceVector, targetVector), 1.0 / (2.0*s));
    q.X = tmp.X;
    q.Y = tmp.Y;
    q.Z = tmp.Z;
    q.W = s;
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
    atan2(2*(q.W*q.X + q.Y*q.Z), 1- (q.X*q.X + q.Y*q.Y)),
    asin(2*(q.W*q.Y - q.Z*q.X)),
    atan2(2*(q.W*q.Z + q.X*q.Y), 1- (q.Y*q.Y + q.Z*q.Z))
  };
  /*
  Vec3 v = {
    asin(2*(q.X*q.Y + q.Z*q.W)),
    atan2(2*(q.Y*q.W - q.X*q.Z), 1- 2*(q.Y*q.Y + q.Z*q.Z)),
    atan2(2*(q.X*q.W - q.Y*q.Z), 1- 2*(q.X*q.X + q.Z*q.Z))
  };
  //heading = atan2(2*qy*qw-2*qx*qz , 1 - 2*qy2 - 2*qz2) = yaw = y
  //attitude = asin(2*qx*qy + 2*qz*qw) = pitch = x
  //bank = atan2(2*qx*qw-2*qy*qz , 1 - 2*qx2 - 2*qz2) = roll = z
*/
  return v;
}

Quat
quat_angles_rad(double yaw, double pitch, double roll)
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

  return quat_angles_rad(
        yaw*r,
        pitch*r,
        roll*r);
}

