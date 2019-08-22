#ifndef _SIMPLE_MODEL_H
#define _SIMPLE_MODEL_H
/*
 * 这只是一个示例的简单obj加载类
 */
#include <glad/glad.h>
#include "../../../third_part_src/glm/glm.hpp"
#include "../../../third_part_src/glm/gtc/matrix_transform.hpp"
#include "../../../third_part_src/glm/gtx/string_cast.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "simple_shader.h"


#define CAR_SCALE_X	1

// 表示一个顶点属性
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

// 表示obj文件中一个顶点的位置、纹理坐标和法向量 索引
struct VertexCombineIndex
{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

// 表示一个用于渲染的最小实体
class Mesh
{
public:
	void draw(Shader& shader) // 绘制Mesh
	{
		shader.use();
		glBindVertexArray(this->VAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->textureId);
		glUniform1i(glGetUniformLocation(shader.programId, "tex"), 0);

		glDrawArrays(GL_TRIANGLES, 0, this->vertData.size());

		glBindVertexArray(0);
		glUseProgram(0);
	}
	Mesh(){}
    // 构造一个Mesh
	Mesh(const std::vector<Vertex>& vertData, GLint textureId)
	{
		this->vertData = vertData;
		this->textureId = textureId;
		this->setupMesh();
		printf("[---] this->VAOId:%d this->VBOId:%d\n", this->VAOId, this->VBOId);
	}
	~Mesh()
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
	}
public:
	std::vector<Vertex> vertData;
	GLuint VAOId, VBOId;
	GLint textureId;

	void setupMesh()  // 建立VAO,VBO等缓冲区
	{
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* this->vertData.size(),
			&this->vertData[0], GL_STATIC_DRAW);
		// 顶点位置属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// 顶点纹理坐标
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// 顶点法向量属性
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};

class ObjLoader
{
public:
	static bool loadFromFile(const char* path,
		std::vector<Vertex>& vertData)
	{

		std::vector<VertexCombineIndex> vertComIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_textCoords;
		std::vector<glm::vec3> temp_normals;

		unsigned int vt_cnt = 0;
		unsigned int vn_cnt = 0;
		unsigned int v_cnt = 0;
		unsigned int f_cnt = 0;

		std::ifstream file(path);
		if (!file)
		{
			std::cerr << "Error::ObjLoader, could not open obj file:"
				<< path << " for reading." << std::endl;
			return false;
		}
		std::string line;
		while (getline(file, line))
		{
			if (line.substr(0, 2) == "vt") // 顶点纹理坐标数据
			{
				std::istringstream s(line.substr(2));
				glm::vec2 v;
				s >> v.x;
				s >> v.y;
				v.y = -v.y;  // 注意这里加载的dds纹理 要对y进行反转
				temp_textCoords.push_back(v);

				// std::cout << line << std::endl;
				vt_cnt++;
			}
			else if (line.substr(0, 2) == "vn") // 顶点法向量数据
			{
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_normals.push_back(v);
				vn_cnt++;
			}
			else if (line.substr(0, 1) == "v") // 顶点位置数据
			{
				std::istringstream s(line.substr(2));
				glm::vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				// v.x = v.x / CAR_SCALE_X;
				// v.y = v.y / CAR_SCALE_X;
				// v.z = v.z / CAR_SCALE_X;
				temp_vertices.push_back(v);
				v_cnt++;
			}
			else if (line.substr(0, 1) == "f") // 面数据
			{
				std::istringstream vtns(line.substr(2));
				std::string vtn;
				while (vtns >> vtn) // 处理一行中多个顶点属性
				{
					VertexCombineIndex vertComIndex;
					std::replace(vtn.begin(), vtn.end(), '/', ' ');
					std::istringstream ivtn(vtn);
					if (vtn.find("  ") != std::string::npos) // 没有纹理数据
					{
						std::cerr << "Error:ObjLoader, no texture data found within file:"
							<< path << std::endl;
						return false;
					}
					ivtn >> vertComIndex.posIndex
						>> vertComIndex.textCoordIndex
						>> vertComIndex.normIndex;

					vertComIndex.posIndex--;
					vertComIndex.textCoordIndex--;
					vertComIndex.normIndex--;
					vertComIndices.push_back(vertComIndex);
				}
				f_cnt++;
			}
			else if (line[0] == '#') // 注释忽略
			{ }
			else
			{
				// 其余内容 暂时不处理
			}
		}
		for (std::vector<GLuint>::size_type i = 0; i < vertComIndices.size(); ++i)
		{
			Vertex vert;
			VertexCombineIndex comIndex = vertComIndices[i];

			vert.position = temp_vertices[comIndex.posIndex];
			vert.texCoords = temp_textCoords[comIndex.textCoordIndex];
			vert.normal = temp_normals[comIndex.normIndex];

			vertData.push_back(vert);

			// std::cout << "vertices: (" << vert.position.x << ", " << vert.position.y
			// 	<< ", " << vert.position.z  << ")" << std::endl;
		}

		std::cout << "vt_cnt: " << vt_cnt << std::endl;
		std::cout << "vn_cnt: " << vn_cnt << std::endl;
		std::cout << "v_cnt:  " << v_cnt << std::endl;
		std::cout << "f_cnt:  " << f_cnt << std::endl;

		return true;
	}

// private:
// 	unsigned int vt_cnt;
// 	unsigned int vn_cnt;
// 	unsigned int v_cnt;
// 	unsigned int f_cnt;
};


#endif
