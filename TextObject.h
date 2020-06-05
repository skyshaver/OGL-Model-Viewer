#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <string>
#include <iostream>

#include "Shader.h"

//using CharacterMap = std::map<GLchar, Character>;

// text 
struct Character {
	unsigned int TextureID;		// id handle of the glyph texture
	glm::ivec2 Size;		// size of glyph
	glm::ivec2 Bearing;		// offset from baseline to left/top of glyph
	unsigned int Advance;			// offset to advance to next glyph
};

using CharacterMap = std::map<GLchar, Character>;

class TextObject
{

public:
	
	TextObject(std::string fontPath) 
	{
		setUpBuffer();
		initFreeType(fontPath);
		Characters = createCharacterMap();
	}

	void initFreeType(std::string fontPath);

	CharacterMap createCharacterMap();

	void RenderText(Shader& s, const std::string& text, glm::vec2 pos, GLfloat scale, const glm::vec3& color);
private:
	CharacterMap Characters;
	FT_Library ft;
	FT_Face face;

	unsigned int textVAO, textVBO;
	void setUpBuffer();
};

