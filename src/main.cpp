#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
int main()
{
    unsigned int width = 1920;
    unsigned int height = 1080;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width,height}),"Tutorials");
    
    unsigned int player_x;
    unsigned int player_y;

    //texture
    sf::Texture playerTexture("/Users/enes/Desktop/PACMAN/src/1.jpeg");

    //RextangleShape
    sf::RectangleShape duvar;
    duvar.setSize({40.f,40.f});
    duvar.setOutlineColor(sf::Color::Red);
    duvar.setOutlineThickness(2.f);
    duvar.setFillColor(sf::Color::Blue);
    
    //CircleShape

    sf::CircleShape player(20.f);
    player.setFillColor(sf::Color::White);
    player.setOrigin({20.f,20.f});
    player.setTexture(&playerTexture);

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
                else if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                {
                    if(!(harita[player_y][player_x+1])){
                        harita[player_y][player_x] = 0;
                        harita[player_y][player_x+1] = 2;
                        player.setPosition({float (player_x) + 20.f,float (player_y)+20.f});
                }
                }
                else if(keyPressed->scancode == sf::Keyboard::Scancode::A){
                    if(harita[player_y][player_x-1]==0){
                        harita[player_y][player_x] = 0;
                        harita[player_y][player_x-1] = 2;
                        
                        player.setPosition({float (player_x) + 20.f,float (player_y)+20.f});
                        }
                }
                else if(keyPressed->scancode == sf::Keyboard::Scancode::S){
                    if(harita[player_y+1][player_x]==0){
                        harita[player_y][player_x] = 0;
                        harita[player_y+1][player_x] = 2;
                        
                        player.setPosition({float (player_x) + 20.f,float (player_y)+20.f});
                        }
                }
                else if(keyPressed->scancode == sf::Keyboard::Scancode::W){
                    if(harita[player_y-1][player_x]==0){
                        harita[player_y][player_x] = 0;
                        harita[player_y-1][player_x] = 2;
                        
                        player.setPosition({float (player_x) + 20.f,float (player_y)+20.f});
                        }
                }
            }
        } 
        
        window->clear();
        for (size_t y = 0; y < 15; y++)
        {
            for (size_t x = 0; x < 20; x++)
            {
                if(harita[y][x] == 1){
                    duvar.setPosition({x*40.f,y*40.f});
                }
                if(harita[y][x] == 2){
                    player_x = x;
                    player_y = y;
                    player.setPosition({((x*40.f)+20.f),((y*40.f)+20.f)});
                }
                window->draw(duvar);
            }
        }
        window->draw(player);
        window->display();
        
    }
    

    delete window;
    return 0;
}