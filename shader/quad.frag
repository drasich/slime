uniform sampler2D texture;
uniform vec2 resolution;

void main (void)
{
  gl_FragColor = texture2D(texture, vec2(gl_FragCoord.x/resolution.x,gl_FragCoord.y/resolution.y));
}

