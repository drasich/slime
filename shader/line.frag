uniform sampler2D texture;
uniform vec2 resolution;

void main (void)
{
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));

  float eps = 0.0001f;
  if (gl_FragCoord.z <= tz.r + eps ) {
    gl_FragColor = vec4(0,1,0,1);
  }
  else {
    gl_FragColor = vec4(0,0.17,0,1);
  }

}

