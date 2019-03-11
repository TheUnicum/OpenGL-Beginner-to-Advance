#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

#include "glm/glm.hpp"

#include <iostream>

struct MaterialData
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	float Shininess;
	bool selected;
	bool selected_i_1;
};

class Materials
{
public:
	Materials(const std::string& filepath);
	~Materials() {};

	inline std::unordered_map<std::string, MaterialData>& GetMaterial() { return m_data; }
	inline const std::string GetSelected() { return m_material_selected; }
	inline void SetSelected(std::string selected) { m_material_selected = selected; }
private:
	void ParseStringLineDate(const std::string& line);
private:
	std::unordered_map<std::string, MaterialData> m_data;
	std::string m_material_selected;
};