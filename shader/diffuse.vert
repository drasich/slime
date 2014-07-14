attribute vec3 vertex;
attribute vec2 texcoord;
uniform mat4 matrix;
uniform mat4 light_mat;
varying vec4 shadow_coord;

varying vec2 f_texcoord;

void main(void)
{
  shadow_coord = light_mat * vec4(vertex, 1.0);
  gl_Position = matrix * vec4(vertex, 1.0);
  f_texcoord = texcoord;
}

