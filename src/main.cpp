#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
int main()
{
    unsigned int width = 640;
    unsigned int height = 360;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width,height}),"Tutorials");
    sf::CircleShape circle;
    circle.setRadius(50.0f);
    circle.setFillColor(sf::Color::Green);
    circle.setOrigin({40.f, 40.f});
    circle.setPosition({300.f,200.f});
    circle.setOutlineColor(sf::Color::Red);
    circle.setOutlineThickness(5.0f);
    while (window -> isOpen())
    {
        while (const std::optional event = window ->pollEvent())
        {
            if(event->is<sf::Event::Closed>()){
                window->close();
            }
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                if (keyPressed->scancode ==  sf::Keyboard::Scancode::Escape)
                {
                    window->close();
                }
                
            }
        } 
        
        window->clear();
        window->draw(circle);
        window->display();
        
    }
    

    delete window;
    return 0;
}