#include "Game.h"
#include <fstream>
#include <iostream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::run()
{
	//add pause here
	// some systems should function while paused (rendering)
	// some systems shouldnt (movement, input)
	while (m_running) {
		m_entities.update();

		if (!m_paused) {
			sEnemySpawner();
			sMovement();
			sCollision();
			sUserInput();
			sLifespan();

			
		}
		sRender();

		//increment the current fram
		//may need to be moved when pause implemented
		m_currentFrame++;
		
		
	}
}

void Game::init(const std::string& path)
{
	//finish this at the end
	//std::ifstream fin(path);
	//fin >> m_playerConfig.SR >> m_playerConfig.CR;
	// read in config, use the premade playerconfig, enemyconfig, bullet config variables
	m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
	m_window.setFramerateLimit(60);

	spawnPlayer();
}

void Game::setPaused(bool paused)
{
	m_paused = paused;
}

void Game::sMovement()
{

	//read m_player->cInput component to determine if theu are moving
	m_player->cTransform->velocity = { 0, 0 };

	//implement player movement
	if (m_player->cInput->up) {
		m_player->cTransform->velocity.y = -5;
	}
	if (m_player->cInput->down) {
		m_player->cTransform->velocity.y = 5;
	}
	if (m_player->cInput->left) {
		m_player->cTransform->velocity.x = -5;
	}
	if (m_player->cInput->right) {
		m_player->cTransform->velocity.x = 5;
	}

	//put check here for bouncing?
	for (auto e : m_entities.getEntities()) {
		if (e->tag() == "enemy") {
			if (e->cTransform->pos.x + e->cCollision->radius > m_window.getSize().x || e->cTransform->pos.x - e->cCollision->radius < 0) {
				e->cTransform->velocity.x *= -1;
			}
			if (e->cTransform->pos.y + e->cCollision->radius > m_window.getSize().y || e->cTransform->pos.y - e->cCollision->radius < 0) {
				e->cTransform->velocity.y *= -1;
			}
		}
		e->cTransform->pos.x += e->cTransform->velocity.x;
		e->cTransform->pos.y += e->cTransform->velocity.y;
		
	}
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}
		//just set the values here, do not implement user movement logic
		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			default:
				break;

			}
		}

		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "Left button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}
			if (event.mouseButton.button == sf::Mouse::Right) {
				std::cout << "Right button clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
				spawnSpecialWeapon(m_player);
			}
		}
	}
}

void Game::sLifespan()
{
	for (auto e : m_entities.getEntities()) {
		if (e->cLifespan) {
			if (e->cLifespan->remaining <= 0) {
				e->destroy();
			}
			e->cLifespan->remaining--;

			float ratio = e->cLifespan->remaining / e->cLifespan->total;
			ratio = std::max(0.f, std::min(1.f, ratio)); // Clamp to [0,1]
			sf::Uint8 alpha = static_cast<sf::Uint8>(255 * ratio);

			sf::Color color = e->cShape->circle.getFillColor();
			color.a = alpha;
			e->cShape->circle.setFillColor(color);

		}
	}
}

void Game::sRender()
{
	//change code to draw LL entities
	m_window.clear();

	for (auto e : m_entities.getEntities()) {
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		m_window.draw(e->cShape->circle);

	}
	m_window.display();
}

void Game::sEnemySpawner()
{
	//code implementing enemy spawner, use m_currentFrame - m_lastEnemySpawnTime to determine how long it has been
	if (m_currentFrame - m_lastEnemySpawnTime > 100) {
		spawnEnemy();
	}
}

void Game::sCollision()
{
	// implement proper collsions between entities, use collision radius not shape radius
	for (auto b : m_entities.getEntities("bullet")) {
		for (auto e : m_entities.getEntities("enemy")) {
			//figure out if bullet overlaps enemy
			Vec2 posB = b->cTransform->pos;
			Vec2 posE = e->cTransform->pos;
			float rB = b->cCollision->radius;
			float rE = e->cCollision->radius;

			float dx = posB.x - posE.x;
			float dy = posB.y - posE.y;
			float distanceSquared = dx * dx + dy * dy;
			float radiusSum = rB + rE;

			if (distanceSquared < (radiusSum * radiusSum)) {
				b->destroy();
				e->destroy();
			}
		}
	}
	for (auto p : m_entities.getEntities("player")) {
		for (auto e : m_entities.getEntities("enemy")) {
			//figure out if bullet overlaps enemy
			Vec2 posP = p->cTransform->pos;
			Vec2 posE = e->cTransform->pos;
			float rP = p->cCollision->radius;
			float rE = e->cCollision->radius;

			float dx = posP.x - posE.x;
			float dy = posP.y - posE.y;
			float distanceSquared = dx * dx + dy * dy;
			float radiusSum = rP + rE;

			if (distanceSquared < (radiusSum * radiusSum)) {
				p->destroy();
				e->destroy();
				spawnSmallEnemies(e);
				spawnPlayer();
			}
		}
	}
}

void Game::spawnPlayer()
{
	//create every entity by entityManager.addEntity(tag)
	// return std::share_ptr<Entity> so auto saves typeing
	auto entity = m_entities.addEntity("player");

	//give entity a Transfor so it spawns at the center with velocity (1,1) and angle 0
	float mx = m_window.getSize().x / 2.0f;
	float my = m_window.getSize().y / 2.0f;

	std::cout << mx << ' ' << my;

	entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0, 0), 0.0f);

	//entity shape with have radius 32, 8 sides, dark grey fill, and red outline
	entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
	entity->cCollision = std::make_shared<CCollision>(32.0f); //fixme collision radius

	//add inputs so player can use inputs
	entity->cInput = std::make_shared<CInput>();

	m_player = entity;

}

//spawn eney and randon location
void Game::spawnEnemy()
{
	//make sure eney is spawne with m_enemyconfig variable, within bound of the window
	// 
	auto entity = m_entities.addEntity("enemy");

	//give entity a Transfor so it spawns at the center with velocity (1,1) and angle 0
	float ex = rand() % m_window.getSize().x;
	float ey = rand() % m_window.getSize().y;

	entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(1.0f, 1.0f), 0.0f);

	//entity shape with have radius 32, 8 sides, dark grey fill, and red outline
	entity->cShape = std::make_shared<CShape>(16.0f, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 4.0f);
	entity->cCollision = std::make_shared<CCollision>(16.0f);
	
	//record when most recent eney was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

//spawn small enements when big one explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// spawn small enemies at the location of input enemy e

}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// bullet speed in config, must set velocity by using formula in notes
	auto bullet = m_entities.addEntity("bullet");
	Vec2 direction = target - entity->cTransform->pos;
	Vec2 normal = direction.normalize();
	Vec2 velocity = normal * 5; // speed is a float you define
	//transform, use entity passed in as the starting point
	bullet->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), velocity, 0);
	bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);
	bullet->cLifespan = std::make_shared<CLifespan>(50); //fix to have life span of the bullet
	bullet->cCollision = std::make_shared<CCollision>(10); //fixme
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
}
