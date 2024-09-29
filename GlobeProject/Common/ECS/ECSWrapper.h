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
		std::vector<IComponent*> components;  // Store pointers to components
	public:
		//Default constructor
		Entity() {}
		//Main constructor
		Entity(entt::entity entity) : entity(entity) {}

		//Deconstructor
		~Entity() {}


		// Add component, enforcing that T must be derived from IComponent
		template<typename T, typename... Args>
		typename std::enable_if<std::is_base_of<IComponent, T>::value>::type
			addComponent(Args&&... args) {
			// Emplace the component into the registry (this creates it inside the registry)
			T& component = registry.emplace<T>(entity, std::forward<Args>(args)...);
			// Store a pointer to the component inside the components vector (reference to registry entry)
			components.push_back(static_cast<IComponent*>(&component));
		}

		// Get a component from the entity
		template<typename T>
		typename std::enable_if<std::is_base_of<IComponent, T>::value, T&>::type
			getComponent() {
			return registry.get<T>(entity);
		}

		// Check if the entity has a specific component
		template<typename T>
		typename std::enable_if<std::is_base_of<IComponent, T>::value, bool>::type
			hasComponent() const {
			return registry.all_of<T>(entity);
		}

		// Remove a component from the entity
		template<typename T>
		typename std::enable_if<std::is_base_of<IComponent, T>::value>::type
			removeComponent() {
			registry.remove<T>(entity);
		}

		// Implicit conversion to entt::entity (optional)
		operator entt::entity() const {
			return entity;
		}

		// Explicit getter for entt::entity if preferred
		entt::entity getRawEntity() const {
			return entity;
		}

		void renderControls() {
			// Iterate through the internal component store
			for (IComponent* component : components) {
				// Attempt to cast the component to IRenderable
				IRenderable* renderable = dynamic_cast<IRenderable*>(component);

				// If the component implements IRenderable, call guiRender()
				if (renderable) {
					renderable->guiRender();
				}
			}
		}


	};

	// Static method to get a view from the registry
	template<typename... Components>
	inline auto getView() {
		return registry.view<Components...>();
	}

	inline Entity create() {
		Entity entity(registry.create());
		return entity;
	}

	// Utility to destroy an entity
	inline void destroyEntity(Entity entity) {
		registry.destroy(entity);
	}
}
#endif