#include "Materials.h"


Materials::Materials(const std::string& filepath)
	:m_material_selected("")
{
	std::ifstream fileMaterials(filepath);
	if (fileMaterials.is_open())
	{
		std::string line;
		// Read line and put in an unordered_map
		while (std::getline(fileMaterials, line))		
			ParseStringLineDate(line);
		fileMaterials.close();
	}
	else
		std::cout << "ERROR:Opening file Materials: " << filepath << std::endl;
}

void Materials::ParseStringLineDate(const std::string& line)
{
	std::string name;
	std::vector<float> datas;

	std::string word;
	float value_f;
	std::stringstream ss_line(line);

	unsigned int idx = 0;
	while (ss_line >> word)
	{ // if "word" can be cast in a <float>variables it means it is a data, otherwise is the name
		std::stringstream ss_word(word);
		if (!(ss_word >> value_f).fail())
		{
			datas.push_back(value_f);
		}
		else
		{
			if (idx == 0)
				name = word;
			else
				name += " " + word;
			idx++;
		}
	}

	if (datas.size() >= 10)
	{
		m_data[name].Ambient =  glm::vec3(datas[0], datas[1], datas[2]);
		m_data[name].Diffuse =  glm::vec3(datas[3], datas[4], datas[5]);
		m_data[name].Specular = glm::vec3(datas[6], datas[7], datas[8]);
		m_data[name].Shininess = datas[9];
		m_data[name].selected = false;
		m_data[name].selected_i_1 = false;
	}
}
