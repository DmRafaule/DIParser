#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace DI{

   struct Mesh;
   struct Texture;
   struct Material;
   struct Shader;

   // Model consist from meshes and materials
   struct Model{
      std::vector<Mesh*> meshes;
      std::vector<Material*> materials;
      std::string path;
   };
   // Setting up Model struct 
   namespace ModelHandler{
      // Load data to model
      void Load(Model& model,std::string path);
      void Scale(Model &model, const glm::vec3 offset);
      void Translate(Model &model, const glm::vec3 offset);
      void Rotate(Model &model,const float angle, const glm::vec3 offset);
      // Get path to model
      std::string processPath(std::string path);
      // Loop throu all nodes in model
      void processNode(Model& model, aiNode *node, const aiScene *scene);
      // Process all mesh data from one model's node
      Mesh* processMesh(Model& model, aiMesh *mesh, const aiScene *scene);
      // Process all material data from one model's node
      Material* processMaterial(Model& model, aiMesh *mesh, const aiScene *scene);
      // Process one material data by specific type
      std::vector<Texture> processMaterialTextures(aiMaterial *mat, aiTextureType type, std::string path);
   };

} // namespace DI
