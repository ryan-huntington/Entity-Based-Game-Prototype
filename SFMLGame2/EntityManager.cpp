#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	//todo add entities from m_to add to proper locatiuons
		// - add to vector of all entities
		// - add to vector inside map with tag as keu
	for (auto e : m_entitiesToAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();

	//remove from vector of all entities 
	removeDeadEntities(m_entities);

	//remove from map, c++ 17 way of iterating through [key, value] pairs
	for (auto& [tag, entityVec] : m_entityMap) {
		removeDeadEntities(entityVec);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](const std::shared_ptr<Entity>& e) {
				return !e->isActive();
			}),
		vec.end()
	);


}


