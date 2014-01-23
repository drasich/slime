varying vec4 diffuse;//, ambient;
varying vec3 eye_normal, light_dir, halfway;
varying vec2 f_texcoord;
uniform sampler2D texture;
uniform vec4 color;

void main (void)
{
  vec3 n, halfv;
	float NdotL, NdotHV;
  vec4 specular = vec4(1.0,1.0,1.0,1);

	vec4 c = color;//ambient;
  vec4 diffuse_tex = texture2D(texture, f_texcoord);
  //vec4 diffuse_tex = texture2D(texture, vec2(0.5,0.5));
  //vec4 diffuse_tex = vec4(0.2, 0.2, 0.8,1);
	n = normalize(eye_normal);

	NdotL = max(dot(n, -light_dir),0.0);

  float shininess = 100.0;

  if (NdotL > 0.0) {
    c += diffuse_tex * NdotL;
    halfv = normalize(halfway);
    NdotHV = max(dot(n, halfv),0.0);
    c += specular * pow(NdotHV, shininess);
  }

  gl_FragColor = c;
}

