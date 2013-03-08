uniform sampler2D texture;
uniform vec2 resolution;

void main (void)
{
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));

  if (tz.g == 1.0f) {
  gl_FragColor = vec4(1,0,0,1);
  }
  else
  gl_FragColor = vec4(0,1,0,1);
  gl_FragColor = vec4(0,tz.b,0,1);
}

