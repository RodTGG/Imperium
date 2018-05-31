#pragma once
#include <glm.hpp>
#include <vulkan\vulkan.hpp>
#include <gtx/hash.hpp>

class Vertex
{
public:
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	Vertex();
	~Vertex();

	bool operator==(const Vertex& other) const;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
