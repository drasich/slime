varying vec2 f_texcoord;
uniform sampler2D texture;
uniform sampler2D light_tex;
uniform float repeat_x;
uniform float repeat_y;
varying vec4 shadow_coord;

void main (void)
{
  vec2 texc = f_texcoord;
  texc.x = texc.x * repeat_x;
  texc.y = texc.y * repeat_y;
  vec4 diffuse_tex = texture2D(texture, texc);

  vec4 shadowCoordinateWdivide = shadow_coord / shadow_coord.w ;
  // Used to lower moiré pattern and self-shadowing
  shadowCoordinateWdivide.z -= 0.00001;
  float distanceFromLight = texture2D(light_tex,shadowCoordinateWdivide.st).z;
  if (shadowCoordinateWdivide.x < 0.0 || 
        shadowCoordinateWdivide.x > 1.0 ||
        shadowCoordinateWdivide.y < 0.0 ||
        shadowCoordinateWdivide.y > 1.0)
  distanceFromLight = 1.0;
  float shadow = 1.0;
  float diff = shadowCoordinateWdivide.z - distanceFromLight;
  if (shadow_coord.w > 0.0) {
    if (diff > 0.00019)
    shadow = 0.5;
    else if (diff > 0.0)
    shadow = 1.0- diff; //0.9;
    else
    shadow = 1.0;
    //shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
  }
  diffuse_tex.xyz = diffuse_tex.xyz * shadow;
  gl_FragColor = diffuse_tex;

}

