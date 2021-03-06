#pragma once

#include <vector>
#include <functional>
#include <geometry/geometry.hpp>
#include <simulation/geometry.hpp>
#include <bwgl/bwgl.hpp>
#include <nanogui/opengl.h>

#include "rendering/Texture.hpp"
#include "rendering/BaseShader.hpp"

#include <CL/cl.hpp>

namespace pbd {
    typedef unsigned int uint;

    /**
     * Host (CPU) representation of a regular mesh. Does
     * NOT match the memory layout in device memory.
     */
    class Mesh {
    public:
        /**
         * Constructs a Mesh object with the specified vertices and parameters.
         */
        Mesh(std::vector<Vertex>    && vertices,
             std::vector<Edge>      && edges,
             std::vector<Triangle>  && triangles,
             GLenum usage = GL_STATIC_DRAW);

        virtual /**
         * Renders the mesh with the provided shader.
         * @param shader The shader
         * @param VP The view-projection matrix to use in rendering
         */
        void render(clgl::BaseShader &shader,
                    const glm::mat4 &VP,
                    const glm::mat4 &M);

        /**
         * Flips the normals of each vertex as well as
         * changes the order of the vertex indices in
         * each triangle, to accurately enable OpenGL
         * face culling.
         */
        void flipNormals();

        /**
         * Buffers the data from host memory to OpenGL memory.
         */
        virtual void uploadHostData();

        /**
         * Generates OpenCL buffer objects for the OpenGL buffers.
         * @param context The OpenCL context to use
         */
        virtual void generateBuffersCL(cl::Context &context);

        /**
         * Frees the host memory for the mesh data. Good practice is
         * to call this after calling #uploadHostData.
         */
        virtual void clearHostData();

        /**
         * Gets a vector of all cl::BufferGL memory objects.
         */
        virtual std::vector<cl::Memory> getMemoryCL();

        /**
         * Returns the number of vertices in this mesh.
         */
        unsigned long numVertices();

        /**
         * Returns the number of vertices in this mesh.
         */
        unsigned long numEdges();

        /**
         * Returns the number of triangles in this mesh.
         */
        unsigned long numTriangles();

        std::vector<Vertex> mVertices;
        std::vector<Edge> mEdges;
        std::vector<Triangle> mTriangles;

        Texture mTexDiffuse;
        Texture mTexSpecular;
        Texture mTexBump;

        bwgl::VertexArray mVAO;

        /// Per-vertex data handles for OpenGL and OpenCL
        bwgl::VertexBuffer mVertexBuffer;
        cl::BufferGL mVertexBufferCL;

        // Per-edge data handles for OpenGL and OpenCL
        bwgl::VertexBuffer mEdgeBuffer;
        cl::BufferGL mEdgeBufferCL;

        // Per-triangle data handles for OpenGL and OpenCL
        bwgl::VertexBuffer mTriangleBuffer;
        cl::BufferGL mTriangleBufferCL;

    protected:
        bool mHasUploadedHostData;

        unsigned long mNumVertices, mNumEdges, mNumTriangles;
    };

    /**
     * Host (CPU) representation of a cloth mesh. Does
     * NOT match the memory layout in device memory.
     */
    class ClothMesh : public Mesh {
    public:
        ClothMesh(std::vector<Vertex>               && vertices,
                  std::vector<ClothVertexData>      && clothVertexData,
                  std::vector<Edge>                 && edges,
                  std::vector<ClothEdgeData>        && clothEdgeData,
                  std::vector<Triangle>             && triangles,
                  std::vector<ClothTriangleData>    && clothTriangleData);

        ClothMesh(Mesh && mesh,
                  std::vector<ClothVertexData>      && clothVertexData,
                  std::vector<ClothEdgeData>        && clothEdgeData,
                  std::vector<ClothTriangleData>    && clothTriangleData);

        virtual void uploadHostData() override;

        virtual void clearHostData() override;

        virtual void generateBuffersCL(cl::Context &context) override;

        virtual std::vector<cl::Memory> getMemoryCL() override;

        virtual void render(clgl::BaseShader &shader, const glm::mat4 &VP, const glm::mat4 &M) override;

        std::vector<ClothVertexData>    mVertexClothData;
        std::vector<ClothEdgeData>      mEdgeClothData;
        std::vector<ClothTriangleData>  mTriangleClothData;

        bwgl::VertexBuffer mVertexClothBuffer;
        cl::BufferGL mVertexClothBufferCL;

        bwgl::VertexBuffer mVertexVelocitiesBuffer;
        cl::BufferGL mVertexVelocitiesBufferCL;

        bwgl::VertexBuffer mVertexPredictedPositionsBuffer;
        cl::BufferGL mVertexPredictedPositionsBufferCL;

        bwgl::VertexBuffer mVertexPositionCorrectionsBuffer;
        cl::BufferGL mVertexPositionCorrectionsBufferCL;

        cl::Buffer mTriangleClothBufferCL;
        cl::Buffer mEdgeClothBufferCL;
        cl::Buffer mDistToLineBufferCL;

        cl::Buffer mVertexInBinPosCL;
    };
}
