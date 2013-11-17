varying vec2 f_texcoord;
uniform sampler2D texture;

void main (void)
{
  vec4 diffuse_tex = texture2D(texture, f_texcoord);
  gl_FragColor = diffuse_tex;
}

