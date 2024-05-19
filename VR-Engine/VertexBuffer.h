#pragma once

#include "stdafx.h"
#include <sstream>

struct Vertex
{
    ST_Vector3 position;
    ST_Vector3 normal;
    ST_Vector2 uv;
};


struct VertexBuffer
{
    ID3D12Resource* m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    UINT numVerts;


    void init(ID3D12Device* pDevice, float* triangleVertices, UINT sizeofVerts, UINT stride)
    {
        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeofVerts);
        ThrowIfFailed(pDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeofVerts);
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = stride;
        m_vertexBufferView.SizeInBytes = sizeofVerts;
        this->numVerts = sizeofVerts / stride;
    }


    void draw(ID3D12GraphicsCommandList* pCommandList)
    {
        pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        pCommandList->DrawInstanced(numVerts, 1, 0, 0);
    }

    void drawInstanced(ID3D12GraphicsCommandList* pCommandList, UINT numInstances)
    {
        pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        pCommandList->DrawInstanced(numVerts, numInstances, 0, 0);
    }

    static VertexBuffer createPlane(ID3D12Device* pDevice)
    {
        float planeVertexData[] = {

            //top face
            -0.5f, 0.0f,0.5f,0,1,0, 0.0f, 0.0f,
            0.5f, 0.0f,-0.5f,0,1,0, 1.0f, 1.0f,
            -0.5f, 0.0f,-0.5f,0,1,0, 0.0f, 1.0f,
            -0.5f, 0.0f,0.5f,0,1,0, 0.0f, 0.0f,
            0.5f, 0.0f,0.5f,0,1,0, 1.0f, 0.0f,
            0.5f, 0.0f,-0.5f,0,1,0, 1.0f, 1.0f,

            //bottom face
            -0.5f, 0.0f,0.5f,0,-1,0, 0.0f, 0.0f,
            -0.5f, 0.0f,-0.5f,0,-1,0, 0.0f, 1.0f,
            0.5f, 0.0f,-0.5f,0,-1,0, 1.0f, 1.0f,
            -0.5f, 0.0f,0.5f,0,-1,0, 0.0f, 0.0f,
            0.5f, 0.0f,-0.5f,0,-1,0, 1.0f, 1.0f,
            0.5f, 0.0f,0.5f,0,-1,0, 1.0f, 0.0f,
        };

        VertexBuffer vb;
        vb.init(pDevice, (float*)planeVertexData, sizeof(planeVertexData), sizeof(Vertex));
        return vb;
    }

    static VertexBuffer createCube(ID3D12Device* pDevice)
    {
        VertexBuffer vb;
        // Create the vertex buffer.
        {
            // Define the geometry for a triangle.
            Vertex triangleVertices[] =
            {
                //back face
                -0.5f, -0.5f,-0.5f, 0.0f, 0.0f, -1, 0.0f, 0.0f,
                -0.5f, 0.5f,-0.5f,0.0f, 0.0f, -1, 0.0f, 1.0f,
                0.5f, 0.5f,-0.5f,0.0f, 0.0f, -1, 1.0f, 1.0f,
                -0.5f, -0.5f,-0.5f,0.0f, 0.0f, -1, 0.0f, 0.0f,
                0.5f, 0.5f,-0.5f,0.0f, 0.0f, -1, 1.0f, 1.0f,
                0.5f, -0.5f,-0.5f,0.0f, 0.0f, -1, 1.0f, 0.0f,

                //right face
                0.5f, -0.5f,-0.5f, 1, 0, 0, 0.0f, 0.0f,
                0.5f, 0.5f,-0.5f,1, 0, 0, 0.0f, 1.0f,
                0.5f, 0.5f,0.5f,1, 0, 0, 1.0f, 1.0f,
                0.5f, -0.5f,-0.5f,1, 0, 0, 0.0f, 0.0f,
                0.5f, 0.5f,0.5f,1, 0, 0, 1.0f, 1.0f,
                0.5f, -0.5f,0.5f,1, 0, 0, 1.0f, 0.0f,

                //front face
                -0.5f, -0.5f,0.5f,0,0,1, 0.0f, 0.0f,
                0.5f, 0.5f,0.5f,0,0,1, 1.0f, 1.0f,
                -0.5f, 0.5f,0.5f,0,0,1, 0.0f, 1.0f,
                -0.5f, -0.5f,0.5f,0,0,1, 0.0f, 0.0f,
                0.5f, -0.5f,0.5f,0,0,1, 1.0f, 0.0f,
                0.5f, 0.5f,0.5f,0,0,1, 1.0f, 1.0f,

                //left face
                -0.5f, -0.5f,-0.5f,-1,0,0, 0.0f, 0.0f,
                -0.5f, 0.5f,0.5f,-1,0,0, 1.0f, 1.0f,
                -0.5f, 0.5f,-0.5f,-1,0,0, 0.0f, 1.0f,
                -0.5f, -0.5f,-0.5f,-1,0,0, 0.0f, 0.0f,
                -0.5f, -0.5f,0.5f,-1,0,0, 1.0f, 0.0f,
                -0.5f, 0.5f,0.5f,-1,0,0, 1.0f, 1.0f,

                //top face
                -0.5f, 0.5f,0.5f,0,1,0, 0.0f, 0.0f,
                0.5f, 0.5f,-0.5f,0,1,0, 1.0f, 1.0f,
                -0.5f, 0.5f,-0.5f,0,1,0, 0.0f, 1.0f,
                -0.5f, 0.5f,0.5f,0,1,0, 0.0f, 0.0f,
                0.5f, 0.5f,0.5f,0,1,0, 1.0f, 0.0f,
                0.5f, 0.5f,-0.5f,0,1,0, 1.0f, 1.0f,

                //bottom face
                -0.5f, -0.5f,0.5f,0,-1,0, 0.0f, 0.0f,
                -0.5f, -0.5f,-0.5f,0,-1,0, 0.0f, 1.0f,
                0.5f, -0.5f,-0.5f,0,-1,0, 1.0f, 1.0f,
                -0.5f, -0.5f,0.5f,0,-1,0, 0.0f, 0.0f,
                0.5f, -0.5f,-0.5f,0,-1,0, 1.0f, 1.0f,
                0.5f, -0.5f,0.5f,0,-1,0, 1.0f, 0.0f,
            };


            vb.init(pDevice, (float*)triangleVertices, sizeof(triangleVertices), sizeof(Vertex));
        }

        return vb;
    }

    static VertexBuffer createGrid(ID3D12Device* pDevice)
    {
        VertexBuffer vb;
        float verts[1212];
        int index = 0;
        int offset = 0;
        for (int i = -50; i <= 50; i++)
        {
            index = offset * 12;

            verts[index] = i;
            verts[index + 1] = 0.0f;
            verts[index + 2] = -50.0f;
            verts[index + 3] = i;
            verts[index + 4] = 0.0f;
            verts[index + 5] = 50.0f;

            verts[index + 6] = -50.0f;
            verts[index + 7] = 0.0f;
            verts[index + 8] = i;
            verts[index + 9] = 50.0f;
            verts[index + 10] = 0.0f;
            verts[index + 11] = i;

            offset++;
        }
        vb.init(pDevice, (float*)verts, 1212 * sizeof(float), sizeof(ST_Vector3));
        return vb;
    }

    static VertexBuffer createCubeWireFrame(ID3D12Device* pDevice)
    {
        VertexBuffer vb;
        {
            float cubeVertexData[] = {

                //back face
                -0.5f, -0.5f, -0.5f,
                -0.5f, 0.5f, -0.5f,
                -0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,

                //connectors
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, 0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, -0.5f,
                -0.5f, 0.5f, 0.5f,

                //front face
                -0.5f, -0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,
                -0.5f, -0.5f, 0.5f
            };
            vb.init(pDevice, (float*)cubeVertexData, sizeof(cubeVertexData), sizeof(ST_Vector3));
        }
        return vb;
    }

    static VertexBuffer createSphereWireFrame(ID3D12Device* pDevice)
    {
        VertexBuffer vb;
        float* verts = (float*)malloc(sizeof(float) * 7200);
        memset(verts, 0, 7200 * sizeof(float));
        int segments = 90;
        int rings = 30;
        int index = 0;
        int offset = 0;
        for (int i = 0; i < segments; i++)
        {
            index = offset * 18;
            float theta = (float)i * 2.0f * M_PI / (float)segments;
            float theta1 = (float)(i + 1) * 2.0f * M_PI / (float)segments;

            //xz plane
            verts[index] = 0.5f * cos(theta);
            verts[index + 1] = 0;
            verts[index + 2] = 0.5f * sin(theta);

            verts[index + 3] = 0.5f * cos(theta1);
            verts[index + 4] = 0;
            verts[index + 5] = 0.5f * sin(theta1);

            //xy play
            verts[index + 6] = 0.5f * cos(theta);
            verts[index + 7] = 0.5f * sin(theta);
            verts[index + 8] = 0;

            verts[index + 9] = 0.5f * cos(theta1);
            verts[index + 10] = 0.5f * sin(theta1);
            verts[index + 11] = 0;

            //yz play
            verts[index + 12] = 0;
            verts[index + 13] = 0.5f * cos(theta);
            verts[index + 14] = 0.5f * sin(theta);

            verts[index + 15] = 0;
            verts[index + 16] = 0.5f * cos(theta1);
            verts[index + 17] = 0.5f * sin(theta1);

            offset++;
        }
        vb.init(pDevice, verts, 1620 * sizeof(float), 3 * sizeof(float));
        free(verts);
        return vb;
    }

    static VertexBuffer createSphere(ID3D12Device* pDevice)
    {
        int segments = 30;
        //int rings = 30;
        int sz = segments * 2 * segments * 48;
        int indSz = segments * segments * 2 * 6;
        //UINT* indices = (UINT*)malloc(sizeof(UINT) * indSz);
        float* verts = (float*)malloc(sizeof(float) * sz);
        int index = 0;
        int offset = 0;
        UINT indicesIndex = 0;
        for (int j = 0; j < segments; j++)
        {
            for (int i = 0; i < 2 * segments; i++)
            {
                index = offset * 48;
                float theta = (float)i * M_PI / (float)segments;
                float theta1 = (float)(i + 1) * M_PI / (float)segments;
                float psi = (float)j * M_PI / (float)segments + M_PI / 2.0;
                float psi1 = (float)(j + 1) * M_PI / (float)segments + M_PI / 2.0;

                ST_Vector3 v3 = { 0.5f * cos(theta) * cos(psi), 0.5f * sin(psi), 0.5f * sin(theta) * cos(psi) };
                ST_Vector3 v2 = { 0.5f * cos(theta) * cos(psi1), 0.5f * sin(psi1), 0.5f * sin(theta) * cos(psi1) };
                ST_Vector3 v1 = { 0.5f * cos(theta1) * cos(psi), 0.5f * sin(psi), 0.5f * sin(theta1) * cos(psi) };

                //ST_Vector3 normal = sphereTraceVector3Cross(sphereTraceVector3Subtract(v2, v1), sphereTraceVector3Subtract(v3, v1));
                //normal = sphereTraceVector3Normalize(normal);

                ST_Vector3 normal1 = sphereTraceVector3Normalize(v1);
                ST_Vector3 normal2 = sphereTraceVector3Normalize(v2);
                ST_Vector3 normal3 = sphereTraceVector3Normalize(v3);

                verts[index] = v1.x;
                verts[index + 1] = v1.y;
                verts[index + 2] = v1.z;
                verts[index + 3] = normal1.x;
                verts[index + 4] = normal1.y;
                verts[index + 5] = normal1.z;
                verts[index + 6] = (float)(i + 1.0f) / (2.0f * segments);
                verts[index + 7] = (float)j / segments;

                verts[index + 8] = v2.x;
                verts[index + 9] = v2.y;
                verts[index + 10] = v2.z;
                verts[index + 11] = normal2.x;
                verts[index + 12] = normal2.y;
                verts[index + 13] = normal2.z;
                verts[index + 14] = (float)i / (2.0f * segments);
                verts[index + 15] = (float)(j + 1.0f) / segments;

                verts[index + 16] = v3.x;
                verts[index + 17] = v3.y;
                verts[index + 18] = v3.z;
                verts[index + 19] = normal3.x;
                verts[index + 20] = normal3.y;
                verts[index + 21] = normal3.z;
                verts[index + 22] = (float)i / (2.0f * segments);
                verts[index + 23] = (float)j / segments;

                //indices[indicesIndex] = indicesIndex++;
                //indices[indicesIndex] = indicesIndex++;
                //indices[indicesIndex] = indicesIndex++;


                v3 = { 0.5f * cosf(theta) * cosf(psi1), 0.5f * sinf(psi1), 0.5f * sinf(theta) * cosf(psi1) };
                v2 = { 0.5f * cosf(theta1) * cosf(psi1), 0.5f * sinf(psi1), 0.5f * sinf(theta1) * cosf(psi1) };
                v1 = { 0.5f * cosf(theta1) * cosf(psi), 0.5f * sinf(psi), 0.5f * sinf(theta1) * cosf(psi) };

                //normal = sphereTraceVector3Cross(sphereTraceVector3Subtract(v2, v1), sphereTraceVector3Subtract(v3, v1));
                //normal = sphereTraceVector3Normalize(normal);
                normal1 = sphereTraceVector3Normalize(v1);
                normal2 = sphereTraceVector3Normalize(v2);
                normal3 = sphereTraceVector3Normalize(v3);

                verts[index + 24] = v1.x;
                verts[index + 25] = v1.y;
                verts[index + 26] = v1.z;
                verts[index + 27] = normal1.x;
                verts[index + 28] = normal1.y;
                verts[index + 29] = normal1.z;
                verts[index + 30] = (float)(i + 1.0f) / (2.0f * segments);
                verts[index + 31] = (float)j / segments;

                verts[index + 32] = v2.x;
                verts[index + 33] = v2.y;
                verts[index + 34] = v2.z;
                verts[index + 35] = normal2.x;
                verts[index + 36] = normal2.y;
                verts[index + 37] = normal2.z;
                verts[index + 38] = (float)(i + 1.0f) / (2.0f * segments);
                verts[index + 39] = (float)(j + 1.0f) / segments;


                verts[index + 40] = v3.x;
                verts[index + 41] = v3.y;
                verts[index + 42] = v3.z;
                verts[index + 43] = normal3.x;
                verts[index + 44] = normal3.y;
                verts[index + 45] = normal3.z;
                verts[index + 46] = (float)i / (2.0f * segments);
                verts[index + 47] = (float)(j + 1.0f) / segments;

                //indices[indicesIndex] = indicesIndex++;
                //indices[indicesIndex] = indicesIndex++;
                //indices[indicesIndex] = indicesIndex++;

                offset++;
            }
        }

        VertexBuffer vb;
        vb.init(pDevice, verts, sz * sizeof(float), 8 * sizeof(float));
        //gIndexBufferSphere = indexBufferCreate(d3d11Device, indSz, indices);
        free(verts);
        //free(indices);
        return vb;
    }

    static VertexBuffer createLine(ID3D12Device* pDevice)
    {
		VertexBuffer vb;
		float verts[] = 
        {
	        0.0f, 0.0f, 0.0f,
	        1.0f, 0.0f, 0.0f
		};
        vb.init(pDevice, verts, sizeof(verts), 3 * sizeof(float));
        return vb;
    }

    static VertexBuffer createCylinder(ID3D12Device* pDevice)
    {
        float verts[2880];
        int segments = 30;
        int rings = 30;
        int index = 0;
        int offset = 0;
        for (int i = 0; i < segments; i++)
        {

            index = offset * 96;
            float theta = (float)i * M_PI / (float)segments * 2.0f;
            float theta1 = (float)(i + 1) * M_PI / (float)segments * 2.0f;

            verts[index] = 0.5f * cosf(theta);
            verts[index + 1] = 0.5f;
            verts[index + 2] = 0.5f * sinf(theta);
            verts[index + 3] = 0.0f;
            verts[index + 4] = 1.0f;
            verts[index + 5] = 0.0f;
            verts[index + 6] = 0.5f * (1.0f + cosf(theta));
            verts[index + 7] = 0.5f * (1.0f + sinf(theta));
            verts[index + 8] = 0.0;
            verts[index + 9] = 0.5f;
            verts[index + 10] = 0.0;
            verts[index + 11] = 0.0f;
            verts[index + 12] = 1.0f;
            verts[index + 13] = 0.0f;
            verts[index + 14] = 0.5f;
            verts[index + 15] = 0.5f;
            verts[index + 16] = 0.5f * cosf(theta1);
            verts[index + 17] = 0.5f;
            verts[index + 18] = 0.5f * sinf(theta1);
            verts[index + 19] = 0.0f;
            verts[index + 20] = 1.0f;
            verts[index + 21] = 0.0f;
            verts[index + 22] = 0.5f * (1.0f + cosf(theta1));
            verts[index + 23] = 0.5f * (1.0f + sinf(theta1));

            ST_Vector3 v1 = { 0.5f * cosf(theta), 0.5f, 0.5f * sinf(theta) };
            ST_Vector3 v2 = { 0.5f * cosf(theta1), 0.5f, 0.5f * sinf(theta1) };
            ST_Vector3 v3 = { 0.5f * cosf(theta), -0.5f, 0.5f * sinf(theta) };
            ST_Vector3 normal = sphereTraceVector3Normalize(sphereTraceVector3Cross(sphereTraceVector3Subtract(v2, v1), sphereTraceVector3Subtract(v3, v1)));

            verts[index + 24] = 0.5f * cosf(theta);
            verts[index + 25] = -0.5f;
            verts[index + 26] = 0.5f * sinf(theta);
            verts[index + 27] = normal.x;
            verts[index + 28] = normal.y;
            verts[index + 29] = normal.z;
            verts[index + 30] = ((float)i / segments);
            verts[index + 31] = 0.0f;
            verts[index + 32] = 0.5f * cosf(theta);
            verts[index + 33] = 0.5f;
            verts[index + 34] = 0.5f * sinf(theta);
            verts[index + 35] = normal.x;
            verts[index + 36] = normal.y;
            verts[index + 37] = normal.z;
            verts[index + 38] = ((float)i / segments);
            verts[index + 39] = 1.0f;
            verts[index + 40] = 0.5f * cosf(theta1);
            verts[index + 41] = 0.5f;
            verts[index + 42] = 0.5f * sinf(theta1);
            verts[index + 43] = normal.x;
            verts[index + 44] = normal.y;
            verts[index + 45] = normal.z;
            verts[index + 46] = ((float)(i + 1.0f) / segments);
            verts[index + 47] = 1.0f;

            verts[index + 48] = 0.5f * cosf(theta);
            verts[index + 49] = -0.5f;
            verts[index + 50] = 0.5f * sinf(theta);
            verts[index + 51] = normal.x;
            verts[index + 52] = normal.y;
            verts[index + 53] = normal.z;
            verts[index + 54] = ((float)i / segments);
            verts[index + 55] = 0.0f;
            verts[index + 56] = 0.5f * cosf(theta1);
            verts[index + 57] = 0.5f;
            verts[index + 58] = 0.5f * sinf(theta1);
            verts[index + 59] = normal.x;
            verts[index + 60] = normal.y;
            verts[index + 61] = normal.z;
            verts[index + 62] = ((float)(i + 1.0f) / segments);
            verts[index + 63] = 1.0f;
            verts[index + 64] = 0.5f * cosf(theta1);
            verts[index + 65] = -0.5f;
            verts[index + 66] = 0.5f * sinf(theta1);
            verts[index + 67] = normal.x;
            verts[index + 68] = normal.y;
            verts[index + 69] = normal.z;
            verts[index + 70] = ((float)(i + 1.0f) / segments);
            verts[index + 71] = 0.0f;

            verts[index + 72] = 0.5f * cosf(theta1);
            verts[index + 73] = -0.5f;
            verts[index + 74] = 0.5f * sinf(theta1);
            verts[index + 75] = 0.0f;
            verts[index + 76] = -1.0f;
            verts[index + 77] = 0.0f;
            verts[index + 78] = 0.5f * (1.0f + cosf(theta1));
            verts[index + 79] = 0.5f * (1.0f + sinf(theta1));
            verts[index + 80] = 0.0;
            verts[index + 81] = -0.5f;
            verts[index + 82] = 0.0;
            verts[index + 83] = 0.0f;
            verts[index + 84] = -1.0f;
            verts[index + 85] = 0.0f;
            verts[index + 86] = 0.5f;
            verts[index + 87] = 0.5f;
            verts[index + 88] = 0.5f * cosf(theta);
            verts[index + 89] = -0.5f;
            verts[index + 90] = 0.5f * sinf(theta);
            verts[index + 91] = 0.0f;
            verts[index + 92] = -1.0f;
            verts[index + 93] = 0.0f;
            verts[index + 94] = 0.5f * (1.0f + cosf(theta));
            verts[index + 95] = 0.5f * (1.0f + sinf(theta));

            offset++;
        }
        VertexBuffer vb;
        vb.init(pDevice,verts, 2880 * sizeof(float), 8 * sizeof(float));
        return vb;
    }

    static std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::istringstream iss(str);
        std::string token;

        while (std::getline(iss, token, delimiter)) {
            result.push_back(token);
        }

        return result;
    }

    static VertexBuffer readFromObj(ID3D12Device* pDevice, const char* objPath)
    {
        VertexBuffer vb;
        vb.numVerts = 0;

        std::ifstream inputFile(objPath);

        if (!inputFile) {
            return vb;
        }

        std::vector<ST_Vector3> positions;
        std::vector<ST_Vector3> normals;
        std::vector<ST_Vector2> uvs;
        std::vector<Vertex> verts;


        std::string line;
        while (std::getline(inputFile, line)) {
            std::vector<std::string> tokens = splitString(line, ' ');
            std::stringstream ss;
            if (tokens[0].compare("v") == 0)
            {
                ST_Vector3 pos;
                ss = std::stringstream(tokens[1]);
                ss >> pos.x;
                ss = std::stringstream(tokens[2]);
                ss >> pos.y;
                ss = std::stringstream(tokens[3]);
                ss >> pos.z;
                positions.push_back(pos);
            }
            else if (tokens[0].compare("vn") == 0)
            {
                ST_Vector3 norm;
                ss = std::stringstream(tokens[1]);
                ss >> norm.x;
                ss = std::stringstream(tokens[2]);
                ss >> norm.y;
                ss = std::stringstream(tokens[3]);
                ss >> norm.z;
                normals.push_back(norm);
            }
            else if (tokens[0].compare("vt") == 0)
            {
                ST_Vector2 uv;
                ss = std::stringstream(tokens[1]);
                ss >> uv.x;
                ss = std::stringstream(tokens[2]);
                ss >> uv.y;
                uvs.push_back(uv);
            }
            else if (tokens[0].compare("f") == 0)
            {
                Vertex vertex;
                UINT index;
                // Vertex 1
                std::vector<std::string> faces = splitString(tokens[1], '/');
                ss = std::stringstream(faces[0]);
                ss >> index;
                vertex.position = positions[index-1];
                ss = std::stringstream(faces[1]);
                ss >> index;
                vertex.uv = uvs[index-1];
                ss = std::stringstream(faces[2]);
                ss >> index;
                vertex.normal = normals[index-1];
                verts.push_back(vertex);

                // Vertex 2
                faces = splitString(tokens[2], '/');
                ss = std::stringstream(faces[0]);
                ss >> index;
                vertex.position = positions[index-1];
                ss = std::stringstream(faces[1]);
                ss >> index;
                vertex.uv = uvs[index-1];
                ss = std::stringstream(faces[2]);
                ss >> index;
                vertex.normal = normals[index-1];
                verts.push_back(vertex);

                // Vertex 3
                faces = splitString(tokens[3], '/');
                ss = std::stringstream(faces[0]);
                ss >> index;
                vertex.position = positions[index-1];
                ss = std::stringstream(faces[1]);
                ss >> index;
                vertex.uv = uvs[index-1];
                ss = std::stringstream(faces[2]);
                ss >> index;
                vertex.normal = normals[index-1];
                verts.push_back(vertex);
            }
        }

        inputFile.close();

        vb.init(pDevice, (float*)verts.data(), verts.size()*sizeof(Vertex), sizeof(Vertex));
        return vb;
    }
};