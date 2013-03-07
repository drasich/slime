attribute vec3 vertex;
//attribute vec3 normal;
//attribute vec2 texcoord;
uniform mat4 matrix;
//varying vec2 f_texcoord;

void main(void)
{
  //f_texcoord = texcoord;
  gl_Position = matrix * vec4(vertex, 1.0);
}

