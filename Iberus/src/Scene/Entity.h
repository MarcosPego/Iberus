#pragma once

#include <memory>
#include <unordered_map>

class Entity
{
	Entity();
	virtual ~Entity() = default;

	Entity* parent{ nullptr };
	std::unordered_map<std::string, Entity*> childMap;

};

