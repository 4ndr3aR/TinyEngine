#version 330 core

in vec4 ex_Color;
out vec4 fragColor;

void main(void) {
  fragColor = vec4(ex_Color.xyz, 0.2);
}
