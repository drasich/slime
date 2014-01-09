attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 matrix;
uniform mat3 normal_matrix;
uniform vec3 light;

varying vec4 diffuse, ambient;
varying vec3 eye_normal, light_dir, halfway;
varying vec2 f_texcoord;
varying vec3 bc;

void main(void)
{
  eye_normal = normalize(normal_matrix * normal);

  //light_dir = normalize(vec3(0.2,-0.5,-1));
  light_dir = normalize(light);
  //light_dir = normalize(vec3(1,-1,-1));

  diffuse = vec4(0.4,0.4,0.9,1);
  ambient = vec4(0.01,0.01,0.02,1);

  halfway = normalize(-light_dir + eye_normal);

  f_texcoord = texcoord;
  gl_Position = matrix * vec4(vertex, 1.0);
}

