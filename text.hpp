#include "DIRenderer/drawable.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>


namespace DI{

  struct Shader;
  struct Font;
  class Camera;

  class Text : public Drawable{
  friend class RendererSingleton;
  private:
    float scale;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
    std::string string;
    Font* font;
  private:
    void Draw() override;
    void CalculateSize(std::string str);
  public:
    Text(const std::string string, glm::vec2 pos = glm::vec2(0.f), glm::vec3 col = glm::vec3(0.f));
    ~Text();
    void ApplyFont(Font* font);
    void SetPosition(glm::vec2 pos);
    void SetColor(glm::vec3 col);
    void ApplyText(const char* string);
    void SetScale(float scale);
    // Getters
    inline const glm::vec2 GetSize(){ return size; }
    inline const glm::vec2 GetPosition(){ return position; }
    inline const glm::vec3 GetColor() { return color; }
    inline const std::string GetString() { return string; }
    inline const Font* const GetFont() { return font; }
  };
  
}


// TODO: Do something with func Draw it's not supposed to be here
