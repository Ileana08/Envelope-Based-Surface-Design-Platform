#version 330 core

// Define constants
#define M_PI 3.141593
const float ka = 0.4f;
const float kd = 0.4f;
const float ks = 0.4f;
const float p = 8.0f;

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertPos;
in vec3 vertColor;
in vec3 vertNormal;
in vec3 lightMov;

// Specify the Uniforms of the fragment shaders
uniform vec3 lightPos;
uniform vec3 lightCol;

// Specify the output of the fragment shader
out vec4 fColor;

void main() {
  vec3 L = normalize(lightPos - vertPos);
  //vec3 L = normalize(lightMov - vertPos);
  vec3 V = -normalize(vertPos);
  vec3 R = normalize(2.0f*max(0.0f, dot(L, vertNormal)) * vertNormal - L);
  vec3 ambient = vertColor * ka;
  vec3 diffuse = vertColor * lightCol * kd * max(0.0f, dot(L, vertNormal));
  vec3 specular = max(0, pow(dot(V, R), p)) * lightCol * ks;
  vec3 color = ambient + diffuse + specular;

  fColor = vec4(color, 1.0f);

}
