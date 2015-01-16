#version 130

in vec2 Position;
uniform mat4 gWorld;
void main()
{
    gl_Position = gWorld * vec4(Position, 0.0, 1.0);
}