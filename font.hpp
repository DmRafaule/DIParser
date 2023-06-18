#pragma once 

#include "DIRenderer/camera.hpp"
#include <map>

namespace DI{

  struct Material;
  struct Mesh;
  struct Shader;

  struct Character {
    unsigned int id;  // ID handle of the glyph texture
    unsigned int advance;    // Offset to advance to next glyph
    glm::ivec2   size;       // Size of glyph
    glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
  };

  struct Font{
    uint size; // Font size
    glm::vec3 color;
    glm::mat4* projection;
    //unsigned int VAO, VBO;
    Mesh* quad;
    Shader* shader; // How to draw glyph
    std::map<char, Character> characters; // Generated map of textures
  };

  namespace FontHandler{
    void Set(Font* font, const char* path, const int size = 26, glm::vec3 color = glm::vec3(0.f));
    void SetShader(Font* font, Shader* shader);
    void SetCamera(Camera* camera);
    void SetSize(Font* font, const int size);
    void SetColor(Font* font, glm::vec3 color);
    void SetProjection(Font* font, glm::mat4& projection);
    void UnSet(Font* font);
  }

}
