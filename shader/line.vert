attribute vec3 vertex;
attribute vec4 color;
uniform mat4 matrix;

varying vec4 vcolor;

void main(void)
{
  vcolor = color;
  gl_Position = matrix * vec4(vertex, 1.0);
}

