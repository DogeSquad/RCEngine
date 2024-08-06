#pragma once

#include "rce_model.hpp"

#include <memory>

namespace rce
{

struct Transform2dComponent {
	glm::vec2 translation{};
	glm::vec2 scale{ 1.0f, 1.0f };
	float rotation = 0.0f;

	glm::mat2 mat2() {
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);
		glm::mat2 rotMat{ {c, s}, {-s, c} };

		glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
		return rotMat * scaleMat;
	}
};

class RCEObject {
public:
	using id_t = unsigned int;

	static RCEObject createObject() {
		static id_t currentId = 0;
		return RCEObject{ currentId++ };
	}

	RCEObject(const RCEObject&) = delete;
	RCEObject& operator=(const RCEObject&) = delete;
	RCEObject(RCEObject&&) = default;
	RCEObject& operator=(RCEObject&&) = default;

	id_t getId() { return id; }

	std::shared_ptr<RCEModel> model{};
	glm::vec3 color{};
	Transform2dComponent transform2d;

private:
	RCEObject(id_t objId) : id{objId} {}

	id_t id;
};

}