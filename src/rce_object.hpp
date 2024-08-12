#pragma once

#include "rce_model.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>
#include <unordered_map>

namespace rce
{

struct TransformComponent 
{
	glm::vec3 translation{};
	glm::vec3 scale{ 1.f, 1.f, 1.f};
	glm::vec3 rotation{};

    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

struct PointLightComponent
{
	float lightIntensity = 1.f;

};

class RCEObject {
public:
	using id_t = unsigned int;
	using Map = std::unordered_map<id_t, RCEObject>;

	static RCEObject createObject() 
	{
		static id_t currentId = 0;
		return RCEObject{ currentId++ };
	}

	static RCEObject makePointLight(float intensity = 10.f, float radius = .1f, glm::vec3 color = glm::vec3(1.f));

	RCEObject(const RCEObject&) = delete;
	RCEObject& operator=(const RCEObject&) = delete;
	RCEObject(RCEObject&&) = default;
	RCEObject& operator=(RCEObject&&) = default;

	id_t getId() { return id; }

	glm::vec3 color{};
	TransformComponent transform;

	std::shared_ptr<RCEModel> model{};
	std::unique_ptr<PointLightComponent> pointLight = nullptr;

private:
	RCEObject(id_t objId) : id{objId} {}

	id_t id;
};

}