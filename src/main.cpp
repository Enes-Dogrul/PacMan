#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
int main()
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width,height}),"Tutorials");
    sf::RectangleShape dikdortgen;
    sf::CircleShape player(20.f);
    player.setFillColor(sf::Color::Green);
    player.setOrigin({20.f,20.f});
    dikdortgen.setSize({40.f,40.f});
    dikdortgen.setOutlineColor(sf::Color::Red);
    dikdortgen.setOutlineThickness(2.f);
    dikdortgen.setFillColor(sf::Color::Blue);
    int harita[15][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 2, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
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
        for (size_t y = 0; y < 15; y++)
        {
            for (size_t x = 0; x < 20; x++)
            {
                if(harita[y][x] == 1){
                    dikdortgen.setPosition({x*40.f,y*40.f});
                }
                if(harita[y][x] == 2){

                    player.setPosition({((x*40.f)+20.f),((y*40.f)+20.f)});
                }
                window->draw(dikdortgen);

            }
            window->draw(player);
        }
        
        window->display();
        
    }
    

    delete window;
    return 0;
}