uniform sampler2D texture;
uniform sampler2D texture_all;
uniform vec2 resolution;

void main (void)
{
  vec4 allz = texture2D(texture_all, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz1 = texture2D(texture, vec2((gl_FragCoord.x+1.0f)/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz2 = texture2D(texture, vec2((gl_FragCoord.x-1.0f)/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz3 = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,(gl_FragCoord.y+1.0f)/resolution.y));
  vec4 tz4 = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,(gl_FragCoord.y-1.0f)/resolution.y));

  float depth = tz1.r;
  if (depth > tz2.r)
    depth = tz2.r;
  if (depth > tz3.r)
    depth = tz3.r;
  if (depth > tz4.r)
    depth = tz4.r;

  if (tz.r == 1.0f && 
        depth != 1.0f
     ) {

    if (allz.r > depth)
    gl_FragColor = vec4(1,0,0,1);
    else
    gl_FragColor = vec4(0,0,0,0);
  }
  else
  gl_FragColor = vec4(0,0,0,0);

  //gl_FragColor = allz;
  //gl_FragColor = vec4(0,0,1,1);
}

