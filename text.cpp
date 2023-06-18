#include "text.hpp"
#include "font.hpp"
#include "DIRenderer/shader.hpp"
#include "DIRenderer/mesh.hpp"
#include "DIRenderer/material.hpp"
#include "GL/glew.h"

namespace DI{

  Text::Text(const std::string string, glm::vec2 pos, glm::vec3 col) :
    string(string), position(pos), color(col), scale(1.f){
    font = nullptr;
  }

  Text::~Text(){
    
  }

  void Text::Draw(){
    DI::ShaderHandler::Use(*font->shader);
    DI::ShaderHandler::SetUniform(*font->shader,"textColor",color);
    DI::ShaderHandler::SetUniform(*font->shader,"projection",*font->projection);
    // For transparent background
    glDisable( GL_DEPTH_TEST );
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->quad->buffer.vao);

    // iterate through all characters
    glm::vec2 old_pos = position;
    for (const auto& c : string){
        Character ch = font->characters[c];

        float xpos = position.x + ch.bearing.x * scale;
        float ypos = position.y - (ch.size.y - ch.bearing.y) * scale + size.y/4;// some offset up

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        // update VBO for each character
        font->quad->verticies.data = new float[]{
          xpos + w, ypos,       1.0f, 1.0f, 
          xpos + w, ypos + h,   1.0f, 0.0f,          
          xpos,     ypos + h,   0.0f, 0.0f, 
          xpos,     ypos,       0.0f, 1.0f, 
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.id);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, font->quad->buffer.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * font->quad->verticies.size, font->quad->verticies.data); 
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->quad->buffer.ebo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, sizeof(unsigned int) * font->quad->elements.count, font->quad->elements.data);
        // render quad
        glDrawElements(GL_TRIANGLES,font->quad->elements.count,GL_UNSIGNED_INT,0);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    position = old_pos;
    glEnable( GL_DEPTH_TEST );
  }
  void Text::ApplyFont(Font* font){
    this->font = font; 
    SetColor(this->font->color);
    CalculateSize(string);
  }
  void Text::SetPosition(glm::vec2 pos){
    this->position = glm::vec2(pos.x - size.x/2,pos.y - size.y/2);
  }
  void Text::SetColor(glm::vec3 col){
    this->color = col; 
  }
  void Text::ApplyText(const char* string){
    this->string = string; 
    CalculateSize(string);
  }
  void Text::SetScale(float scale){
    this->scale = scale;
  }

  void Text::CalculateSize(std::string string){
    size = glm::vec2(0);
    std::string::const_iterator c;
    for (c = string.begin(); c != string.end(); c++){
      Character ch = font->characters[*c];
      // Calculate whole width of string
      size.x += (ch.advance>>6);
      // Set the most heighest character hight
      
      size.y = (size.y > ch.size.y) ? size.y : ch.size.y; 
    }
    size.y *= 2;
  }
}
