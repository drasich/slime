#include "quat.h"

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
    .Y = qr.W*ql.Y - qr.X*ql.Z + qr.Y*ql.W - qr.Z*ql.X,
    .Z = qr.W*ql.Z + qr.X*ql.Y + qr.Y*ql.X - qr.Z*ql.W,
    .W = qr.W*ql.W - qr.X*ql.X + qr.Y*ql.Y - qr.Z*ql.Z
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
  uv = vec3_mul(uv, 2.0f*q.W);
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
quat_from_quat_to_quat(Quat q1, Quat q2)
{
  return quat_mul(quat_inverse(q1),q2);
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
slerp(Quat from, Quat to, float t)
{
  const double epsilon = 0.00001;
  double omega, cosomega, sinomega, scale_from, scale_to ;

  Quat quatTo = to;

  // this is a dot product
  cosomega = vec4_dot(from, to);

  if ( cosomega <0.0 ) { 
    cosomega = -cosomega; 
    quatTo = quat_mul_scalar(to, -1); //quatTo = -to;
  }

    if( (1.0 - cosomega) > epsilon )
    {
        omega= acos(cosomega) ;  // 0 <= omega <= Pi (see man acos)
        sinomega = sin(omega) ;  // this sinomega should always be +ve so
        // could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
        scale_from = sin((1.0-t)*omega)/sinomega ;
        scale_to = sin(t*omega)/sinomega ;
    }
    else
    {
        // --------------------------------------------------
        //   The ends of the vectors are very close
        //   we can use simple linear interpolation - no need
        //   to worry about the "spherical" interpolation
        //   --------------------------------------------------
        scale_from = 1.0 - t ;
        scale_to = t ;
    }


    //TODO quat add Quat q = quat_mul_scalar(from, scale_from) + quat_mul_scalar(quatTo,scale_to);
    //*this = (from*scale_from) + (quatTo*scale_to);
    // so that we get a Vec4


  return to;
}
