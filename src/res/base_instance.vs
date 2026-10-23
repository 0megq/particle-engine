// Used raylib "lighting_instancing.vs" as an example.

#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec4 vertexColor;

in mat4 instanceTransform;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec4 fragColor;

void main()
{
    // Send vertex attributes to fragment shader
    fragColor = vertexColor;

    // Calculate final vertex position, note that we multiply mvp by instanceTransform
    gl_Position = mvp*instanceTransform*vec4(vertexPosition, 1.0);
}