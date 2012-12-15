attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texcoord;
uniform mat4 matrix;
uniform mat3 normal_matrix;
uniform float test;

varying vec4 diffuse,ambient;
varying vec3 eye_normal,lightDir,halfway;
varying vec2 f_texcoord;

void main(void)
{
  eye_normal = normalize(normal_matrix * normal);

  lightDir = normalize(vec3(0.2,-0.5,-1));
  //lightDir = normalize(vec3(1,-1,-1));

  diffuse = vec4(0.4,0.4,0.9,1);
  ambient = vec4(0.01,0.01,0.02,1);

  halfway = normalize(-lightDir + eye_normal);

  //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  //gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 1.0);
  //gl_Position =   gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(vertex, 1.0);
  //gl_Position =   gl_ProjectionMatrix * matrix * vec4(vertex, 1.0);
  //gl_Position =   gl_ProjectionMatrix * matrix * vec4(vertex, test);
  gl_Position = matrix * vec4(vertex, test);
  f_texcoord = texcoord;
}

