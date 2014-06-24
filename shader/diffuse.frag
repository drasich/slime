varying vec2 f_texcoord;
uniform sampler2D texture;
uniform float repeat_x;
uniform float repeat_y;

void main (void)
{
  vec2 texc = f_texcoord;
  //repeat = 1.0;
  texc.x = texc.x * repeat_x;
  texc.y = texc.y * repeat_y;
  //vec4 diffuse_tex = texture2D(texture, f_texcoord);
  vec4 diffuse_tex = texture2D(texture, texc);
  gl_FragColor = diffuse_tex;
}

