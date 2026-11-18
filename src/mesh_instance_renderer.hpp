#ifndef MESH_INSTANCE_RENDERER_H
#define MESH_INSTANCE_RENDERER_H
#include <raylib-cpp.hpp>
#include <vector>
#include "particle.hpp"

class MeshInstanceRenderer {
    private:
        raylib::Shader shader;
        raylib::Material material;
        raylib::Mesh mesh;

    // Default constructor initializes
    // `shader` with base_instance.vs and flat.fs
    // `material` with the `shader` and raylib's default material
    // `mesh` with a sphere of radius 1
    public:
        MeshInstanceRenderer() {}
        
        void initialize() {
            shader = raylib::Shader::Load("res/base_instance.vs", "res/flat.fs");
            shader.locs[SHADER_LOC_MATRIX_MVP] = shader.GetLocation("mvp");
            material = LoadMaterialDefault();
            material.shader = shader;

            mesh = raylib::Mesh::Sphere(1.0f, 16, 16);
        }

        void render(const std::vector<raylib::Matrix> &transformVec, const std::vector<raylib::Color> &colorVec) {
            const Matrix* transforms = &transformVec[0];
            const Color* colors = &colorVec[0];
            int instances = transformVec.size();
            
            // the code below is based on the DrawMeshInstanced function in rmodels.c
        #ifndef MAX_MATERIAL_MAPS
            #define MAX_MATERIAL_MAPS       12      // Maximum number of maps supported
        #endif
        #if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
            // Instancing required variables
            float16 *instanceTransform = NULL;
            float3 *instanceColor = NULL;
            unsigned int instancesVboId = 0;

            // Bind shader program
            rlEnableShader(material.shader.id);

            // Send required data to shader (matrices, values)
            //-----------------------------------------------------
            // Upload to shader material.colDiffuse
            if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1)
            {
                float values[4] = {
                    (float)material.maps[MATERIAL_MAP_DIFFUSE].color.r/255.0f,
                    (float)material.maps[MATERIAL_MAP_DIFFUSE].color.g/255.0f,
                    (float)material.maps[MATERIAL_MAP_DIFFUSE].color.b/255.0f,
                    (float)material.maps[MATERIAL_MAP_DIFFUSE].color.a/255.0f
                };

                rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
            }

            // Upload to shader material.colSpecular (if location available)
            if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
            {
                float values[4] = {
                    (float)material.maps[MATERIAL_MAP_SPECULAR].color.r/255.0f,
                    (float)material.maps[MATERIAL_MAP_SPECULAR].color.g/255.0f,
                    (float)material.maps[MATERIAL_MAP_SPECULAR].color.b/255.0f,
                    (float)material.maps[MATERIAL_MAP_SPECULAR].color.a/255.0f
                };

                rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
            }

            // Get a copy of current matrices to work with,
            // in case stereo render is required, and they need to be modified
            // NOTE: At this point the modelview matrix contains the view matrix (camera)
            // That's because BeginMode3D() sets it and there is no model-drawing function
            // that modifies it, all use rlPushMatrix() and rlPopMatrix()
            Matrix matModel = MatrixIdentity();
            Matrix matView = rlGetMatrixModelview();
            Matrix matModelView = MatrixIdentity();
            Matrix matProjection = rlGetMatrixProjection();

            // Upload view and projection matrices (if locations available)
            if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
            if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);

            // Create instances buffer
            instanceTransform = (float16 *)RL_CALLOC(instances, sizeof(float16));
            instanceColor = (float3 *)RL_CALLOC(instances, sizeof(float3));

            // Fill buffer with instances transformations as float16 arrays
            for (int i = 0; i < instances; i++) instanceTransform[i] = MatrixToFloatV(transforms[i]);

            // Enable mesh VAO to attach new buffer
            rlEnableVertexArray(mesh.vaoId);

            // This could alternatively use a static VBO and either glMapBuffer() or glBufferSubData()
            // It isn't clear which would be reliably faster in all cases and on all platforms,
            // anecdotally glMapBuffer() seems quite slow (syncs) while glBufferSubData() seems
            // no faster, since all the transform matrices are transferred anyway
            instancesVboId = rlLoadVertexBuffer(instanceTransform, instances*sizeof(float16), false);

            // Instances transformation matrices are sent to shader attribute location: SHADER_LOC_VERTEX_INSTANCETRANSFORM
            if (material.shader.locs[SHADER_LOC_VERTEX_INSTANCETRANSFORM] != -1)
            {
                for (unsigned int i = 0; i < 4; i++)
                {
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_INSTANCETRANSFORM] + i);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_INSTANCETRANSFORM] + i, 4, RL_FLOAT, 0, sizeof(Matrix), i*sizeof(Vector4));
                    rlSetVertexAttributeDivisor(material.shader.locs[SHADER_LOC_VERTEX_INSTANCETRANSFORM] + i, 1);
                }
            }

            rlDisableVertexBuffer();
            rlDisableVertexArray();

            // Accumulate internal matrix transform (push/pop) and view matrix
            // NOTE: In this case, model instance transformation must be computed in the shader
            matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

            // Upload model normal matrix (if locations available)
            if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
            //-----------------------------------------------------

            // Bind active texture maps (if available)
            for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
            {
                if (material.maps[i].texture.id > 0)
                {
                    // Select current shader texture slot
                    rlActiveTextureSlot(i);

                    // Enable texture for active slot
                    if ((i == MATERIAL_MAP_IRRADIANCE) ||
                        (i == MATERIAL_MAP_PREFILTER) ||
                        (i == MATERIAL_MAP_CUBEMAP)) rlEnableTextureCubemap(material.maps[i].texture.id);
                    else rlEnableTexture(material.maps[i].texture.id);

                    rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
                }
            }

            // Try binding vertex array objects (VAO)
            // or use VBOs if not possible
            if (!rlEnableVertexArray(mesh.vaoId))
            {
                // Bind mesh VBO data: vertex position (shader-location = 0)
                rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION]);
                rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, 0, 0, 0);
                rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

                // Bind mesh VBO data: vertex texcoords (shader-location = 1)
                rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD]);
                rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, 0, 0, 0);
                rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

                if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
                {
                    // Bind mesh VBO data: vertex normals (shader-location = 2)
                    rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL]);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, 0, 0, 0);
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
                }

                // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
                if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
                {
                    if (mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR] != 0)
                    {
                        rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR]);
                        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, 1, 0, 0);
                        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
                    }
                    else
                    {
                        // Set default value for unused attribute
                        // NOTE: Required when using default shader and no VAO support
                        float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                        rlSetVertexAttributeDefault(material.shader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                        rlDisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
                    }
                }

                // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
                if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
                {
                    rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT]);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0, 0, 0);
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
                }

                // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
                if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
                {
                    rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2]);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, 0, 0, 0);
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
                }

        #if SUPPORT_GPU_SKINNING
                // Bind mesh VBO data: vertex bone ids (shader-location = 6, if available)
                if (material.shader.locs[SHADER_LOC_VERTEX_BONEIDS] != -1)
                {
                    rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES]);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEIDS], 4, RL_UNSIGNED_BYTE, 0, 0, 0);
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEIDS]);
                }

                // Bind mesh VBO data: vertex bone weights (shader-location = 7, if available)
                if (material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS] != -1)
                {
                    rlEnableVertexBuffer(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS]);
                    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS], 4, RL_FLOAT, 0, 0, 0);
                    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_BONEWEIGHTS]);
                }
        #endif

                if (mesh.indices != NULL) rlEnableVertexBufferElement(mesh.vboId[RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES]);
            }

            int eyeCount = 1;
            if (rlIsStereoRenderEnabled()) eyeCount = 2;

            for (int eye = 0; eye < eyeCount; eye++)
            {
                // Calculate model-view-projection matrix (MVP)
                Matrix matModelViewProjection = MatrixIdentity();
                if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
                else
                {
                    // Setup current eye viewport (half screen width)
                    rlViewport(eye*rlGetFramebufferWidth()/2, 0, rlGetFramebufferWidth()/2, rlGetFramebufferHeight());
                    matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
                }

                // Send combined model-view-projection matrix to shader
                rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

                // Draw mesh instanced
                if (mesh.indices != NULL) rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount*3, 0, instances);
                else rlDrawVertexArrayInstanced(0, mesh.vertexCount, instances);
            }

            // Unbind all bound texture maps
            for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
            {
                if (material.maps[i].texture.id > 0)
                {
                    // Select current shader texture slot
                    rlActiveTextureSlot(i);

                    // Disable texture for active slot
                    if ((i == MATERIAL_MAP_IRRADIANCE) ||
                        (i == MATERIAL_MAP_PREFILTER) ||
                        (i == MATERIAL_MAP_CUBEMAP)) rlDisableTextureCubemap();
                    else rlDisableTexture();
                }
            }

            // Disable all possible vertex array objects (or VBOs)
            rlDisableVertexArray();
            rlDisableVertexBuffer();
            rlDisableVertexBufferElement();

            // Disable shader program
            rlDisableShader();

            // Remove instance transforms buffer
            rlUnloadVertexBuffer(instancesVboId);
            RL_FREE(instanceTransform);
        #endif
        }

        void renderParticles(const std::vector<Particle> &particles) {
            std::vector<raylib::Matrix> transforms;
            std::vector<raylib::Color> colors;
            transforms.reserve(particles.size());
            colors.reserve(particles.size());

            for (const auto &particle : particles){
                auto scaleMatrix = raylib::Matrix::Scale(particle.radius, particle.radius, particle.radius);
                auto translateMatrix = raylib::Matrix::Translate(particle.posCur.x, particle.posCur.y, particle.posCur.z);
                transforms.push_back(scaleMatrix * translateMatrix);
                colors.push_back(particle.color);
            }
            render(transforms, colors);
        }

        ~MeshInstanceRenderer() {
            material.Unload();
            shader.Unload();
            mesh.Unload();
        }


        static void SphereInstancingDemo() {
            static const int instanceCount = 10;

            static MeshInstanceRenderer renderer;
            static std::vector<raylib::Matrix> transforms;
            static std::vector<raylib::Color> colors;

            static bool initialized = false;
            
            if (!initialized) {
                initialized = true;
                // set the transforms of each instance
                transforms.resize(instanceCount);
                colors.resize(instanceCount);
                transforms[0] = raylib::Matrix::Identity();
                colors[0] = raylib::Color::RayWhite();
                for (int i = 1; i < instanceCount; i++) {
                    transforms[i] = raylib::Matrix::Translate(1.0f, 0.0f, 1.0f) * transforms[i - 1];
                    colors[i] = raylib::Color::Violet();
                }
            }

            renderer.render(transforms, colors);
        }
};


#endif