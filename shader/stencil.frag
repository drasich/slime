uniform sampler2D texture;
uniform vec2 resolution;

void main (void)
{
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));

  gl_FragColor = vec4(tz.r,0,0,1);
}

