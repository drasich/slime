uniform float test;

varying vec4 diffuse,ambient;
varying vec3 eye_normal,lightDir,halfway;
varying vec2 f_texcoord;
uniform sampler2D texture;

void main (void)
{
  vec3 n,halfv;
	float NdotL,NdotHV;
  vec4 specular = vec4(1.0,1.0,1.0,1);

	vec4 color = ambient;
  vec4 diffuse_tex = texture2D(texture, f_texcoord);
	n = normalize(eye_normal);

	NdotL = max(dot(n,-lightDir),0.0);

  float shininess = 100.0;

  if (NdotL > 0.0) {
    color += diffuse_tex * NdotL;
    halfv = normalize(halfway);
    NdotHV = max(dot(n,halfv),0.0);
    color += specular * pow(NdotHV, shininess);
  }

  gl_FragColor = color;
}

