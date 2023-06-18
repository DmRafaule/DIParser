#include "font.hpp"

#include "DIRenderer/renderer.hpp"
#include "DIRenderer/shader.hpp"
#include "DIRenderer/mesh.hpp"
#include "DIRenderer/layout.hpp"
#include "DIRenderer/material.hpp"
#include "DIRenderer/texture.hpp"

#include <DIDebuger/debuger.hpp>
#include <DIDebuger/log.hpp>

#include "ft2build.h"
#include FT_FREETYPE_H 


namespace DI{

  namespace FontHandler{
    void Set(Font* font, const char* path, const int size, glm::vec3 color){
      font->size = size;
      font->color = color;
      FT_Library ft;
      if (FT_Init_FreeType(&ft)){
        DI_LOG_ERROR("TextHandler say: Could not init freetype library");
      }
      FT_Face face;
      if (FT_New_Face(ft,path,0,&face)){
        DI_LOG_ERROR("TextHandler say: Could not load ttf font");
      }
      FT_Set_Pixel_Sizes(face,0,font->size);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
      for (unsigned char c = 0; c < 128; c++){
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            DI_LOG_ERROR("TextHandler say: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          // now store character for later use
          Character character;
          character.id = texture;
          character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
          character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
          character.advance = face->glyph->advance.x; 

          font->characters.insert(std::pair<char, Character>(c, character));
      }
      FT_Done_Face(face);
      FT_Done_FreeType(ft);
      // Set default mesh(quad)
      font->quad = new DI::Mesh();
      glGenVertexArrays(1,&font->quad->buffer.vao);
      glBindVertexArray(font->quad->buffer.vao);
      // Now set verticies 
      font->quad->verticies.count = 4;
      font->quad->verticies.size  = font->quad->verticies.count * 4; 
      glGenBuffers(1,&font->quad->buffer.vbo);
      glBindBuffer(GL_ARRAY_BUFFER,font->quad->buffer.vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * font->quad->verticies.size, NULL, GL_DYNAMIC_DRAW);
      // Now set elements
      font->quad->elements.count = 6;
      font->quad->elements.data = new unsigned int[]{
        0, 1, 3,
        3, 2, 1,
      };
      glGenBuffers(1,&font->quad->buffer.ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->quad->buffer.ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * font->quad->elements.count, NULL, GL_DYNAMIC_DRAW);

      font->shader = new DI::Shader();
      DI::ShaderHandler::Set(*font->shader,"assets/shaders/tPosColTex.vert","assets/shaders/tPosColTex.frag");
      LayoutHandler::Set(font->shader->layout);
      //font->projection = glm::ortho(0.0f, eg_winData->size.x, 0.0f, eg_winData->size.y);
      DI::ShaderHandler::Use(*font->shader);
      DI::ShaderHandler::SetUniform(*font->shader,"textColor",color);
      //DI::ShaderHandler::SetUniform(*font->shader,"projection",font->projection);
    }

    void SetShader(Font* font, Shader* shader){
      font->shader = shader;
    }
    void SetProjection(Font* font, glm::mat4 &projection){
      font->projection = &projection;
    }

    void SetColor(Font* font, glm::vec3 color){
      font->color = color;  
    }

    void SetSize(Font* font, const int size){
      font->size = size; 
    }

    void UnSet(Font* font){
      font->characters.clear();
      delete font->shader;
      delete font;
    }
  }
}
