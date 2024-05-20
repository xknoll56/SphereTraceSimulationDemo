#pragma once

struct Material
{
	Texture ambientTexture;
	//Specular Exponent
	float Ns;
	// Ambient Color
	ST_Vector3 Ka;
	// Diffuse Color
	ST_Vector3 Kd;
	// Specular Color
	ST_Vector3 Ks;
    bool hasTexture;

	Material()
	{
		Ns = 8.0;
		Ka = ST_VECTOR3(0, 0, 0);
		Kd = ST_VECTOR3(0.4, 0.4, 0.4);
		Ks = ST_VECTOR3(0, 0, 0);
        hasTexture = false;
	}
};
struct Model
{
	struct ModelComponent
	{
		VertexBuffer vb;
		Material materia;
        std::string name;
        
	};
	std::vector<ModelComponent> components;

	static std::map<std::string, Material> loadMaterials(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, 
		ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp,std::string materialPath)
	{
		std::map<std::string, Material> materials;
        std::ifstream inputFile(materialPath);

        if (!inputFile) 
        {
            return materials;
        }

		std::string line;
		std::string materialName;
		Material mat;
		std::stringstream ss;
        bool hasTexture = false;
        while (std::getline(inputFile, line)) 
        {
			std::vector<std::string> tokens = StringManipulation::splitString(line, ' ');
			if (tokens.size() > 0)
			{
				if (tokens[0] == "newmtl")
				{
					if (!materialName.empty())
					{
                        mat.hasTexture = hasTexture;
						materials[materialName] = mat;
						mat = Material();
					}
					materialName = tokens[1];
                    hasTexture = false;
				}
				else if (tokens[0] == "Ns")
				{
					ss = std::stringstream(tokens[1]);
					ss >> mat.Ns;
				}
				else if (tokens[0] == "Ka")
				{
					mat.Ka = StringManipulation::vector3FromStrings(tokens[1], tokens[2], tokens[3]);
				}
				else if (tokens[0] == "Kd")
				{
					mat.Kd = StringManipulation::vector3FromStrings(tokens[1], tokens[2], tokens[3]);
				}
				else if (tokens[0] == "Ks")
				{
					mat.Ks = StringManipulation::vector3FromStrings(tokens[1], tokens[2], tokens[3]);
				}
				else if (tokens[0] == "map_Ka")
				{
                    hasTexture = true;
					std::string texturePath = materialPath;
					size_t index = texturePath.find_last_of('/');  // Find the position of the last slash

					if (index != std::string::npos) {
						texturePath = texturePath.substr(0, index);  // Extract the substring up to the last slash
					}
					texturePath = texturePath + "/" + tokens[1];
					mat.ambientTexture.init(pDevice, pCommandList, textureUploadHeap, dhp, texturePath.c_str());
				}
			}
        }
		materials[materialName] = mat;
        inputFile.close();
		return materials;
	}

    static Model modelFromObjFile(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList,
        ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp, std::string materialPath, std::string objPath)
    {
        Model model;
        std::map<std::string, Material> materialMap = loadMaterials(pDevice, pCommandList, textureUploadHeap, dhp, materialPath);
        std::ifstream inputFile(objPath);

        if (!inputFile) {
            return model;
        }

        std::vector<ST_Vector3> positions;
        std::vector<ST_Vector3> normals;
        std::vector<ST_Vector2> uvs;
        std::vector<Vertex> verts;

        std::string line;
        std::string material;
        while (std::getline(inputFile, line)) {
            std::vector<std::string> tokens = StringManipulation::splitString(line, ' ');
            std::stringstream ss;
            if (tokens[0].compare("o") == 0)
            {
                if (!verts.empty())
                {
                    ModelComponent modelComponent;
                    modelComponent.vb.init(pDevice, (float*)verts.data(), verts.size() * sizeof(Vertex), sizeof(Vertex));
                    if (materialMap.find(material) != materialMap.end())
                    {
                        modelComponent.materia = materialMap[material];
                    }
                    else
                    {
                        printf("did not find material\n");
                    }
                    modelComponent.name = tokens[1];
                    model.components.push_back(modelComponent);
                    verts.clear();
                }
            }
            else if (tokens[0].compare("v") == 0)
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
                for (int i = 1; i <= 3; ++i)
                {
                    Vertex vertex;
                    UINT index;
                    std::vector<std::string> faces = StringManipulation::splitString(tokens[i], '/');

                    // Position
                    ss = std::stringstream(faces[0]);
                    ss >> index;
                    vertex.position = positions[index - 1];

                    // UV (check if UV index is available)
                    if (faces.size() > 1 && !faces[1].empty())
                    {
                        ss = std::stringstream(faces[1]);
                        ss >> index;
                        vertex.uv = uvs[index - 1];
                    }

                    // Normal (check if normal index is available)
                    if (faces.size() > 2 && !faces[2].empty())
                    {
                        ss = std::stringstream(faces[2]);
                        ss >> index;
                        vertex.normal = normals[index - 1];
                    }

                    verts.push_back(vertex);
                }
            }
            else if (tokens[0] == "usemtl")
            {
                if (!verts.empty())
                {
                    ModelComponent modelComponent;
                    modelComponent.vb.init(pDevice, (float*)verts.data(), verts.size() * sizeof(Vertex), sizeof(Vertex));
                    if (materialMap.find(material) != materialMap.end())
                    {
                        modelComponent.materia = materialMap[material];
                    }
                    else
                    {
                        printf("did not find material\n");
                    }
                    modelComponent.name = material;
                    model.components.push_back(modelComponent);
                    verts.clear();
                }
                material = tokens[1];
            }
        }

        // Add the final model component with whatever is left
        if (!verts.empty())
        {
            ModelComponent modelComponent;
            modelComponent.vb.init(pDevice, (float*)verts.data(), verts.size() * sizeof(Vertex), sizeof(Vertex));
            if (materialMap.find(material) != materialMap.end())
            {
                modelComponent.materia = materialMap[material];
            }
            model.components.push_back(modelComponent);
        }

        inputFile.close();

        return model;
    }
};