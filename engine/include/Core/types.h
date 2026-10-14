#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <glad/glad.h>

#include <vector>
#include <string>

//Base for ecs
typedef uint32_t EntityID;
static constexpr uint32_t INVALID_ID = UINT32_MAX;

enum MouseState {
	GAME,
	UI,
	BOTH
};

struct Name
{
	std::string value;
};

struct Hierarchy
{
	EntityID parent{ INVALID_ID };
	EntityID firstChild{ INVALID_ID };
	EntityID nextSibling{ INVALID_ID };
	EntityID previousSibling{ INVALID_ID };
};

struct Transform {
	glm::vec3 position{0, 0, 0};
	glm::quat rotation{1, 0, 0, 0};
	glm::vec3 eulerRotation{ 0, 0, 0 };
	glm::vec3 scale{1, 1, 1};

	glm::mat4 GetMatrix() const
	{
		glm::mat4 matrix(1.0f);

		matrix = glm::translate(matrix, position);
		matrix *= glm::mat4_cast(rotation);
		matrix = glm::scale(matrix, scale);

		return matrix;
	}

	void SetMatrix(const glm::mat4& matrix)
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(
			matrix,
			scale,
			rotation,
			position,
			skew,
			perspective
		);

		eulerRotation = glm::degrees(glm::eulerAngles(rotation));
	}

	void SetEulerRotation(glm::vec3 degrees)
	{
		eulerRotation = degrees;
		rotation = glm::quat(glm::radians(eulerRotation));
	}
};

struct Mesh
{
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	Mesh(std::vector<GLfloat> vertices, std::vector<GLuint> indices) : vertices(vertices), indices(indices)
	{
		//Create a VAO ID
		glGenVertexArrays(1, &VAO);
		//Create a VBO ID
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Bind the VAO
		glBindVertexArray(VAO);

		//Bind the VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//Send the data to the buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		//I could also bind at the same time as the vbo... doesnt matter since its a different type
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		//Create the attribute. Since we're using a vao, this is saved to the vao.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		//Enable attrib
		glEnableVertexAttribArray(0);

		//Create the attribute. Since we're using a vao, this is saved to the vao.
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		//Enable attrib
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		//Enable attrib
		glEnableVertexAttribArray(2);
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
};

///////////////
/// LIGHTS   //
///////////////

struct Light
{
	glm::vec3 color{ 1.0f };
	glm::vec3 reflectionColor{ 1.0f };
	float intensity{ 2.0f };
};

//////////////////////////
/// DIRECTIONAL LIGHT   //
//////////////////////////
struct DirectionalLight : Light {};

////////////////////
/// POINT LIGHT   //
////////////////////

struct PointLight : Light {
	float radius{ 5.0f };
};

///////////////////
/// SPOT LIGHT   //
///////////////////

struct SpotLight : PointLight {
	float innerCutoff{ glm::radians(15.0f) };
	float outerCutoff{ glm::radians(25.0f) };
};

struct PointLightShader
{
	glm::vec3 position;
	EntityID id;
	
	glm::vec3 color;
	float intensity;

	glm::vec3 reflectionColor;
	float radius;
};

struct SpotLightShader
{
	glm::vec3 position;
	EntityID id;

	glm::vec3 color;
	float intensity;

	glm::vec3 reflectionColor;
	float radius;

	glm::vec3 direction;
	float innerCutOff;

	float outerCutoff;
	glm::vec3 padding;
};

PointLightShader ConvertPointLight(PointLight light, EntityID id, Transform transform);
SpotLightShader ConvertSpotLight(SpotLight light, EntityID id, Transform transform);