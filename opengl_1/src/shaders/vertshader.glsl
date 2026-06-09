 #version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;
layout(location = 2) in vec3 vertColor_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat3 normalTransform;
uniform mat4 projTransform;
uniform vec3 lightPos;

// Specify the output of the vertex stage
out vec3 vertPos;
out vec3 vertNormal;
out vec3 vertColor;
out vec3 lightMov;

void main() {
  // gl_Position is the output (a vec4) of the vertex shader
  // Currently without any transformation
  // gl_Position = vec4(vertCoordinates_in, 1.0F);
  gl_Position = projTransform * modelTransform * vec4(vertCoordinates_in, 1.0f);

  lightMov = vec3(modelTransform * vec4(lightPos, 1.0f));
  vertPos = vec3(modelTransform * vec4(vertCoordinates_in, 1.0f));;
  vertNormal = normalize(normalTransform * vertNormal_in);
  vertColor = vertColor_in;
}
