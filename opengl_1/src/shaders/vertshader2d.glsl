 #version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 vertNormal_in;
layout(location = 2) in vec3 vertColor_in;

 uniform mat4 modelTransform;
uniform mat4 projTransform;

out vec3 color;

void main() {
  // gl_Position is the output (a vec4) of the vertex shader
  // Currently without any transformation
  // gl_Position = vec4(vertCoordinates_in, 1.0F);
  color = vertColor_in;
  gl_Position = projTransform * modelTransform * vec4(vertCoordinates_in, 1.0f);
}
