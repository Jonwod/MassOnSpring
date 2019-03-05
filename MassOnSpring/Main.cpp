#include <SFML/Graphics.hpp>


/*
	A simple mass with one dimensional velocity
*/
class Mass: public sf::CircleShape {
public:
	Mass(float r, float m) 
		:sf::CircleShape(r),
		mass(m),
		velocity(0.f)
	{
		setOrigin(r, r);
		setFillColor(sf::Color::Red);
	}

	void force(float F, float time) {
		const float a = F / mass;
		velocity += a * time;
	}

	void step(float dt) {
		move({ 0, velocity * dt });
	}

private:
	float mass;
	float velocity;
};


class Spring: public sf::Transformable, public sf::Drawable{
public:
	// sf::Drawable
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
		const float current_length = mass ? mass->getPosition().y - getPosition().y : rest_length;
		sf::RectangleShape rectangle({ 5.f, current_length});
		rectangle.setOrigin({rectangle.getSize().x / 2.f, 0});	// The origin is the top point of the spring
		rectangle.setPosition(getPosition());
		target.draw(rectangle);
	}
	// ____________

	// Teleports the mass to the rest point of the spring.
	void attachMass(Mass *  target_mass) {
		mass = target_mass;
		if (mass) {
			mass->setPosition(getPosition() + sf::Vector2f{0.f, rest_length});
		}
	}

	void step(float dt) {
		if (mass) {
			const float mass_displacement = mass->getPosition().y - (getPosition().y + rest_length);
			mass->force(-spring_constant * mass_displacement, dt);
		}
	}

	float spring_constant = 20.f;
	float rest_length = 200.f;
private:
	Mass * mass = nullptr;
};


int main()
{
	const sf::Vector2u window_size(800, 800);
	sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Mass On A Spring");

	Spring spring;
	spring.setPosition(sf::Vector2f(window_size.x/2.f, 200.f));

	Mass mass(25.f, 20.f);
	spring.attachMass(&mass);
	mass.move({ 0, -100.f });	//Apply initial displacement

	constexpr float timestep = 1.f / 60.f;
	window.setFramerateLimit(1.f / timestep);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Graphics
		window.clear();
		window.draw(mass);
		window.draw(spring);
		window.display();
		// __________

		// Simulation
		spring.step(timestep);
		mass.step(timestep);
		// __________
	}

	return 0;
}