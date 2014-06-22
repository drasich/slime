varying vec2 f_texcoord;
uniform sampler2D texture;
uniform float repeat;

void main (void)
{
  vec2 texc = f_texcoord;
  //repeat = 1.0;
  texc = texc * repeat;
  //vec4 diffuse_tex = texture2D(texture, f_texcoord);
  vec4 diffuse_tex = texture2D(texture, texc);
  gl_FragColor = diffuse_tex;
}

