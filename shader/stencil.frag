uniform sampler2D texture;
uniform vec2 resolution;

void main (void)
{
  vec4 tz = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz1 = texture2D(texture, vec2((gl_FragCoord.x+1.0f)/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz2 = texture2D(texture, vec2((gl_FragCoord.x-1.0f)/resolution.x,gl_FragCoord.y/resolution.y));
  vec4 tz3 = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,(gl_FragCoord.y+1.0f)/resolution.y));
  vec4 tz4 = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,(gl_FragCoord.y-1.0f)/resolution.y));

  if (tz.g == 1.0f && 
        (
         //1 == 1
         tz1.r != 1.0f ||
         tz2.r != 1.0f ||
         tz3.r != 1.0f ||
         tz4.r != 1.0f
        )
        ) {

  gl_FragColor = vec4(1,0,0,1);
  }
  else
  gl_FragColor = vec4(0,0,0,0);
  //gl_FragColor = vec4(0,tz.a,0,1);
  //gl_FragColor = tz;
}

