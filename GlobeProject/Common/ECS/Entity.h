#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include "Systems/Systems.h"

namespace ECS {
	
	struct DataTransfer {
		glm::mat4 transform;
		glm::vec3 color;

		//Default Contructor
		DataTransfer() {}

		DataTransfer(glm::mat4 transform, glm::vec3 color) : transform(transform) , color(color) {

		}

	};

	class Entity {
	private:
		entt::entity entity;
	public:
		Entity(entt::entity entity)
			: entity(entity)
		{

		}
		~Entity() {

		}
		
		template<typename T, typename... Args>
		typename std::enable_if<std::is_constructible<T, Args...>::value>::type
		addComponent(Args&&... args) {
			registry.emplace<T>(entity, std::forward<Args>(args)...);
		}
		
	};


	inline Entity create() {
		Entity entity(registry.create());
		return entity;
	}

	inline BoundingBoxComponent createBoundingBox(glm::vec3 color) {
		glm::vec3 RBB(0.5f, -0.5f, -0.5f);
		glm::vec3 RBT(0.5f, 0.5f, -0.5f);
		glm::vec3 RFT(0.5f, 0.5f, 0.5f);
		glm::vec3 RFB(0.5f, -0.5f, 0.5f);
		glm::vec3 LBB(-0.5f, -0.5f, -0.5f);
		glm::vec3 LBT(-0.5f, 0.5f, -0.5f);
		glm::vec3 LFT(-0.5f, 0.5f, 0.5f);
		glm::vec3 LFB(-0.5f, -0.5f, 0.5f);

		return BoundingBoxComponent(RBB, RBT, RFT, RFB, LBB, LBT, LFT, LFB, color);
	}

}
#endif