uniform sampler2D texture;
uniform vec2 resolution;
varying vec4 vcolor;

void main (void)
{
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));

  float eps = 0.0001f;
  //if (gl_FragCoord.z <= tz.r + eps ) {
  if (gl_FragCoord.z < tz.r ) {
    //gl_FragColor = vec4(0,1,0,1);
    gl_FragColor = vcolor;
  }
  else {
    vec4 vc = vcolor* 0.15;
    //gl_FragColor = vec4(0,0.17,0,1);
    gl_FragColor = vc;
  }
}

