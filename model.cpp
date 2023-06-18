#include "model.hpp"
#include "DIRenderer/mesh.hpp"
#include "DIRenderer/texture.hpp"
#include "DIRenderer/material.hpp"
#include "DIRenderer/renderer.hpp"
#include "DIDebuger/debuger.hpp"
#include "DIDebuger/log.hpp"

namespace DI{
   
	extern DebugData* eg_debugData;
  
  void prs(){
  }
	void ModelHandler::Load(Model& model,std::string path){
    Assimp::Importer importer;
  
    model.path = processPath(path);
    
    const aiScene *scene = importer.ReadFile(path,aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
      DI_LOG_ERROR("ModelHandler say: {0}",importer.GetErrorString());
    }
    else{
      eg_debugData->counterDIModels_inMem++;
      processNode(model,scene->mRootNode, scene);
     	DI_LOG_INFO("ModelHandler say: Loaded model, success.");
    } 	
  }
    void ModelHandler::Scale(Model &model, const glm::vec3 offset){
    	for (int i = 0; i < model.meshes.size(); ++i){
    		MeshHandler::Scale(*model.meshes[i],offset);
    	}
    }
    void ModelHandler::Translate(Model &model, const glm::vec3 offset){
    	for (int i = 0; i < model.meshes.size(); ++i){
            model.meshes[i]->model_matrix = glm::mat4(1.0f);
            MeshHandler::Translate(*model.meshes[i],offset);
    	}
    }
    void ModelHandler::Rotate(Model &model,const float angle, const glm::vec3 offset){
		  for (int i = 0; i < model.meshes.size(); ++i){
    		MeshHandler::Rotate(*model.meshes[i],angle,offset);
    	}
    }
    std::string ModelHandler::processPath(std::string path){
    	int end = path.rfind("/");
    	return path.substr(0,end) + "/";
    }
    void ModelHandler::processNode(Model& model, aiNode *node, const aiScene *scene){
    	
   		for(unsigned int i = 0; i < node->mNumMeshes; i++){
      		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
      		model.meshes.push_back(processMesh(model,mesh, scene));
      		model.materials.push_back(processMaterial(model,mesh, scene));
   		}
   		// then do the same for each of its children
   		for(unsigned int i = 0; i < node->mNumChildren; i++){
      		processNode(model, node->mChildren[i], scene);
   		}
    }
    Mesh* ModelHandler::processMesh(Model& model, aiMesh *mesh, const aiScene *scene){
		  Mesh* m = new Mesh();
			// 								3     3      3         2
	   	//process vertecies data(pos, colors, normals, texcoord)
	   	m->verticies.count = mesh->mNumVertices; 
			m->verticies.size = m->verticies.count * 11;// I dont like this 11 (how many floats in one vertices)
	   	m->verticies.data = new float[m->verticies.size];
	   	int counter = 0;
	   	for(unsigned int i = 0; i < m->verticies.count; i++){
	   		static_cast<float*>(m->verticies.data)[counter]   = mesh->mVertices[i].x;
	      static_cast<float*>(m->verticies.data)[counter+1] = mesh->mVertices[i].y;
	      static_cast<float*>(m->verticies.data)[counter+2] = mesh->mVertices[i].z;
	      counter += 3;
	      if (mesh->HasVertexColors(i)){
	      	static_cast<float*>(m->verticies.data)[counter] = mesh->mColors[i]->r;
	        static_cast<float*>(m->verticies.data)[counter+1] = mesh->mColors[i]->g;
	        static_cast<float*>(m->verticies.data)[counter+2] = mesh->mColors[i]->b;
	      	counter += 3;
	      }
	      if (mesh->HasNormals()){//*.x3d have not normals
	        static_cast<float*>(m->verticies.data)[counter] = mesh->mNormals[i].x;
	        static_cast<float*>(m->verticies.data)[counter+1] = mesh->mNormals[i].y;
	        static_cast<float*>(m->verticies.data)[counter+2] = mesh->mNormals[i].z;
	      	counter += 3;
	      }
	      if (mesh->mTextureCoords[0]){
	        static_cast<float*>(m->verticies.data)[counter] = mesh->mTextureCoords[0][i].x;
	        static_cast<float*>(m->verticies.data)[counter+1] = mesh->mTextureCoords[0][i].y;
	      	counter += 2;
	       }
	   	}
	   	counter = 0;

	   	// process elements(indicies) data
	   	m->elements.count = 0;
	   	for(unsigned int i = 0; i < mesh->mNumFaces; i++){
	      	aiFace face = mesh->mFaces[i];
	      	m->elements.count += face.mNumIndices;
	   	} 
	   	m->elements.data = new unsigned int[m->elements.count];
   		for(unsigned int i = 0; i < mesh->mNumFaces; i++){
      		aiFace face = mesh->mFaces[i];
      		for(unsigned int j = 0; j < face.mNumIndices; j++){
         		static_cast<unsigned int*>(m->elements.data)[counter] = face.mIndices[j];
         		counter++;
         	}
   		} 
   		counter = 0;

	   	return m;
    }
    Material* ModelHandler::processMaterial(Model& model, aiMesh *mesh, const aiScene *scene){
    	Material* m = new Material();
    	std::vector<Texture> textures;
    	
    	// proccess textures
   		if (mesh->mMaterialIndex >= 0){	// If mesh have materials
   			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];// Get material for mesh
   			// 1. diffuse maps
    		std::vector<Texture> diffuseMaps = processMaterialTextures(material, aiTextureType_DIFFUSE, model.path);
    		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
  	  	// 2. specular maps
  	  	std::vector<Texture> specularMaps = processMaterialTextures(material, aiTextureType_SPECULAR, model.path);
			  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			  // 3. normal maps
			  std::vector<Texture> normalMaps = processMaterialTextures(material, aiTextureType_HEIGHT, model.path);
			  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			  // 4. height maps
			  std::vector<Texture> heightMaps = processMaterialTextures(material, aiTextureType_AMBIENT, model.path);
			  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		  }
   		for (int i = 0; i < textures.size(); ++i)
   			MaterialHandler::SetSampler(*m,textures[i]);

		  return m;
    }

  	std::vector<Texture> ModelHandler::processMaterialTextures(aiMaterial *mat, aiTextureType type, std::string path){
	    // For now this function assume that all textures used by model in the same directory
	    std::vector<Texture> tx;
	    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
	        aiString str;
	        mat->GetTexture(type, i, &str);
	    	// Add check on already loaded textures
	        Texture tx_tmp;
	        std::string filep = std::string(str.C_Str());
	        filep = path + filep;
	        DI::TextureHandler::Set(tx_tmp,filep);
	        tx_tmp.path = filep;
	        tx_tmp.sampler = "";
	        tx.push_back(tx_tmp);
	    }
	    return tx;
	}  
} // namespace DI


// NOTES:
/*
 	1)in processMaterialTextures func there is no check if texture already loaded
	2)in processMaterial func I'm not sending samplers name and when I use this there is no name for unifomr sender
	I dont know what is a kind of magic ¯\_(ツ)_/¯ 
*/
