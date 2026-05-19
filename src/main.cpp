#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <queue>


struct Point {
    int x, y;
};

void moveBlinky(int harita[15][20], int &blinky_x, int &blinky_y, int pacman_x, int pacman_y, int &blinkyUnder) {
    int dx[] = {0, 0, -1, 1}; 
    int dy[] = {-1, 1, 0, 0};

    bool visited[15][20] = {false};
    Point parent[15][20]; 

    std::queue<Point> q;
    q.push({blinky_x, blinky_y});
    visited[blinky_y][blinky_x] = true;
    parent[blinky_y][blinky_x] = {-1, -1};

    bool found = false;

    while (!q.empty()) {
        Point curr = q.front();
        q.pop();

        if (curr.x == pacman_x && curr.y == pacman_y) {
            found = true;
            break;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            if (nx >= 0 && nx < 20 && ny >= 0 && ny < 15 && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (found) {
        Point step = {pacman_x, pacman_y};
        while (parent[step.y][step.x].x != blinky_x || parent[step.y][step.x].y != blinky_y) {
            step = parent[step.y][step.x];
        }

        harita[blinky_y][blinky_x] = blinkyUnder;
        

        blinkyUnder = harita[step.y][step.x];
        
        if (blinkyUnder == 2) {
            std::cout << "ÖLDÜN" << std::endl;
        }

        harita[step.y][step.x] = 4;
        blinky_x = step.x;
        blinky_y = step.y;
    }
}

enum class State {
    Left,
    Right,
    Up,
    Down
};

void bfs(int *array,int x, int y)
{
    
}

int main()
{
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    unsigned int width = 1920;
    unsigned int height = 1080;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width,height}),"Tutorials");
    
    unsigned int player_x = 0;
    unsigned int player_y = 0;
    unsigned int point = 0;

    //texture
    std::vector<sf::Texture> Up(3);
    std::vector<sf::Texture> Down(3);
    std::vector<sf::Texture> Left(3);
    std::vector<sf::Texture> Right(3);
    sf::Texture blinkyTexture;
    blinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/blinky.png");
    Up[0].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_1.png");
    Up[1].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_2.png");
    Up[2].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_3.png");
    Down[0].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_1.png");
    Down[1].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_2.png");
    Down[2].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_3.png");
    Left[0].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_1.png");
    Left[1].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_2.png");
    Left[2].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_3.png");
    Right[0].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_1.png");
    Right[1].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_2.png");
    Right[2].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_3.png");
    float widtTexture = Right[0].getSize().x;
    float heightTexture = Right[0].getSize().y;
    //clock
    sf::Clock textureClock;
    float frameTime = 0.1f;
    int currentTexture = 0;
    State currentState = State::Right;
    State previousState = State::Right;
    sf::RectangleShape duvar;
    duvar.setSize({40.f,40.f});
    duvar.setOutlineColor(sf::Color::Red);
    duvar.setOutlineThickness(2.f);
    duvar.setFillColor(sf::Color::Blue);
    
    //CircleShape
    sf::CircleShape pointShape(5.f);
    pointShape.setFillColor(sf::Color::White);
    pointShape.setOrigin({0.f,0.f});


    sf::Sprite pacman(Right[0]);
    pacman.setScale({30.f/widtTexture,30.f/heightTexture});
    pacman.setOrigin({widtTexture / 2.f,heightTexture / 2.f});
    sf::Sprite blinky(blinkyTexture);
    blinky.setScale({30.f/widtTexture,30.f / heightTexture});
    blinky.setOrigin({widtTexture / 2.f,heightTexture / 2.f});
    
    int harita[15][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 3, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 0, 3, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 2, 3, 0, 0, 3, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int blinky_x = 0;
    int blinky_y = 0;
    int blinkyUnder = 0; 
    
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 20; x++) {
            if (harita[y][x] == 2) {
                player_x = x;
                player_y = y;
            } else if (harita[y][x] == 4) {
                blinky_x = x;
                blinky_y = y;
            }
        }
    }
    sf::Font font("/Users/enes/Desktop/PACMAN/src/arial.ttf");
    sf::Text text(font);
    while (window -> isOpen())
    {
        
        text.setString(std::to_string(point));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        
        if(currentState != previousState){
            currentTexture = 0;             
            textureClock.restart();         
            previousState = currentState; 
        }
        if (textureClock.getElapsedTime().asSeconds() >= frameTime) {
                currentTexture++;
                if (currentState == State::Right)
                {
                    if(currentTexture >= Right.size()){
                    textureClock.reset();
                    currentTexture = 0;
                }
                pacman.setTexture(Right[currentTexture]);
                
            }
            else if (currentState == State::Left)
                {
                    if(currentTexture >= Left.size()){
                    textureClock.reset();
                    currentTexture = 0;
                }
                pacman.setTexture(Left[currentTexture]);
            }
            else if (currentState == State::Up)
            {
                    if(currentTexture >= Up.size()){
                    textureClock.reset();
                    currentTexture = 0;
                }
                pacman.setTexture(Up[currentTexture]);  
            }
            else if (currentState == State::Down)
                {
                    if(currentTexture >= Down.size()){
                    textureClock.reset();
                    currentTexture = 0;
                }
                pacman.setTexture(Down[currentTexture]);   
            } 
            textureClock.restart();
        }
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
                else {
                    bool moved = false; 
                    int new_x = player_x;
                    int new_y = player_y;

                    if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
                        new_x = player_x + 1;
                        currentState = State::Right;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
                        new_x = player_x - 1;
                        currentState = State::Left;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
                        new_y = player_y + 1;
                        currentState = State::Down;
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
                        new_y = player_y - 1;
                        currentState = State::Up;
                    }

                    if (harita[new_y][new_x] != 1) {
                        if (harita[new_y][new_x] == 3) { 
                            point++;
                        }
                        harita[player_y][player_x] = 0;  
                        harita[new_y][new_x] = 2;        
                        player_x = new_x;
                        player_y = new_y;
                        moved = true;
                    }

                    if (moved) {
                        moveBlinky(harita, blinky_x, blinky_y, player_x, player_y, blinkyUnder);
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
                    window->draw(duvar);
                }
                else if(harita[y][x] == 2){
                    player_x = x;
                    player_y = y;
                    pacman.setPosition({((x*40.f)+20.f),((y*40.f)+20.f)});
                }
                else if(harita[y][x] == 3)
                {
                    pointShape.setPosition({(float(x)*40.f)+12.f,(float(y)*40.f)+14.f});
                    window->draw(pointShape);
                }
                else if(harita[y][x] == 4){
                    blinky.setPosition({(float(x)*40.f)+12.f,(float(y)*40.f)+14.f});
                }
            }
        }
        window->draw(blinky);
        window->draw(pacman);
        window->draw(text);
        window->display();
        
    }
    delete window;
    return 0;
}