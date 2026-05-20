#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <queue>
#include <cmath>

struct Point { int x, y; };

enum class State { Left, Right, Up, Down };

Point getBlinkyNextStep(int harita[15][20], int bx, int by, int px, int py) {
    if (bx == px && by == py) 
        return {-1, -1};

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool visited[15][20] = {};
    Point parent[15][20];
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 20; j++)
            parent[i][j] = {-1, -1};

    std::queue<Point> q;
    q.push({bx, by});
    visited[by][bx] = true;

    bool found = false;
    while (!q.empty()) {
        Point curr = q.front(); q.pop();
        if (curr.x == px && curr.y == py) 
        { 
            found = true; 
            break; 
        }
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < 20 && ny >= 0 && ny < 15 && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }
    if (!found) return {-1, -1};

    Point step = {px, py};
    while (parent[step.y][step.x].x != bx || parent[step.y][step.x].y != by) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) break;
        step = next;
    }
    return step;
}

Point getPinkyNextStep(int harita[15][20], int bx, int by, int px, int py, State currentState) {
    int tx = px, ty = py;
    if (currentState == State::Right) 
        tx = px + 4;
    else if (currentState == State::Left)  
        tx = px - 4;
    else if (currentState == State::Down)  
        ty = py + 4;
    else if (currentState == State::Up)    
        ty = py - 4;

    tx = std::max(0, std::min(19, tx));
    ty = std::max(0, std::min(14, ty));

    while (harita[ty][tx] == 1) {
        if      (currentState == State::Right) 
            tx--;
        else if (currentState == State::Left)  
            tx++;
        else if (currentState == State::Down)  
            ty--;
        else if (currentState == State::Up)    
            ty++;
    }

    if (bx == tx && by == ty) 
        return {-1, -1};

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool visited[15][20] = {};
    Point parent[15][20];
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 20; j++)
            parent[i][j] = {-1, -1};

    std::queue<Point> q;
    q.push({bx, by});
    visited[by][bx] = true;

    bool found = false;
    while (!q.empty()) {
        Point curr = q.front(); q.pop();
        if (curr.x == tx && curr.y == ty) { found = true; break; }
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < 20 && ny >= 0 && ny < 15 && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }
    if (!found) return {-1, -1};

    Point step = {tx, ty};
    while (parent[step.y][step.x].x != bx || parent[step.y][step.x].y != by) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) 
            break;
        step = next;
    }
    return step;
}

int main() {
    unsigned int width = 1920, height = 1080;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width, height}), "PACMAN");

    std::vector<sf::Texture> texUp(3), texDown(3), texLeft(3), texRight(3);
    sf::Texture blinkyTexture, pinkyTexture;
    blinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/blinky.png");
    pinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/pinky.png");
    for (int i = 0; i < 3; i++) {
        texUp[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_" + std::to_string(i+1) + ".png");
        texDown[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_" + std::to_string(i+1) + ".png");
        texLeft[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_" + std::to_string(i+1) + ".png");
        texRight[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_" + std::to_string(i+1) + ".png");
    }

    float texW = texRight[0].getSize().x;
    float texH = texRight[0].getSize().y;

    sf::Sprite pacman(texRight[0]);
    pacman.setScale({30.f / texW, 30.f / texH});
    pacman.setOrigin({texW / 2.f, texH / 2.f});

    sf::Sprite blinkySprite(blinkyTexture); 
    blinkySprite.setScale({30.f / texW, 30.f / texH});
    blinkySprite.setOrigin({texW / 2.f, texH / 2.f});

    sf::Sprite pinkySprite(pinkyTexture);   
    pinkySprite.setScale({30.f / texW, 30.f / texH});
    pinkySprite.setOrigin({texW / 2.f, texH / 2.f});

    sf::Clock textureClock;
    float frameTime = 0.1f;
    int currentTexture = 0;
    State currentState = State::Right;
    State previousState = State::Right;

    sf::RectangleShape duvar;
    duvar.setSize({40.f, 40.f});
    duvar.setOutlineColor(sf::Color::Red);
    duvar.setOutlineThickness(2.f);
    duvar.setFillColor(sf::Color::Blue);

    sf::CircleShape pointShape(5.f);
    pointShape.setFillColor(sf::Color::White);

    int harita[15][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 3, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 0, 3, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
        {1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 2, 3, 0, 0, 3, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int player_gx = 0;
    int player_gy = 0;
    int blinky_gx = 0;
    int blinky_gy = 0;
    int blinkyUnder = 0;
    int pinky_gx  = 0;
    int pinky_gy  = 0;
    int pinkyUnder  = 0;

    for (int y = 0; y < 15; y++)
        for (int x = 0; x < 20; x++) {
            if      (harita[y][x] == 2)
            {
                player_gx = x;
                player_gy = y; 
            }
            else if (harita[y][x] == 4)
            {
                blinky_gx = x;
                blinky_gy = y; 
            }
            else if (harita[y][x] == 5)
            {
                pinky_gx  = x;
                pinky_gy  = y; 
            } 
        }

    float ppx = player_gx * 40.f + 20.f, ppy = player_gy * 40.f + 20.f;
    float bpx = blinky_gx * 40.f + 20.f, bpy = blinky_gy * 40.f + 20.f;
    float pkx = pinky_gx  * 40.f + 20.f, pky = pinky_gy  * 40.f + 20.f; 

    int player_tgx = player_gx;
    int player_tgy = player_gy;
    int blinky_tgx = blinky_gx;
    int blinky_tgy = blinky_gy;
    int pinky_tgx  = pinky_gx;
    int pinky_tgy  = pinky_gy;

    bool player_moving = false;
    bool blinky_moving = false;
    bool pinky_moving  = false;

    float player_speed = 150.f;
    float blinky_speed = 100.f;
    float pinky_speed  = 110.f;
    unsigned int point = 0;

    sf::Clock deltaClock;
    sf::Font font("/Users/enes/Desktop/PACMAN/src/arial.ttf");
    sf::Text text(font);

    while (window->isOpen()) {
        float dt = deltaClock.restart().asSeconds();

        text.setString("Puan: " + std::to_string(point));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        if (currentState != previousState) {
            currentTexture = 0;
            textureClock.restart();
            previousState = currentState;
        }
        if (textureClock.getElapsedTime().asSeconds() >= frameTime) {
            currentTexture++;
            std::vector<sf::Texture>* frames = nullptr;
            if      (currentState == State::Right)
                frames = &texRight;
            else if (currentState == State::Left) 
                frames = &texLeft;
            else if (currentState == State::Up)
                frames = &texUp;
            else
                frames = &texDown;
            if (currentTexture >= (int)frames->size())
                currentTexture = 0;
            pacman.setTexture((*frames)[currentTexture]);
            textureClock.restart();
        }

        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) window->close();
            else if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                if      (kp->scancode == sf::Keyboard::Scancode::Escape) 
                    window->close();
                else if (kp->scancode == sf::Keyboard::Scancode::D)
                    currentState = State::Right;
                else if (kp->scancode == sf::Keyboard::Scancode::A) 
                    currentState = State::Left;
                else if (kp->scancode == sf::Keyboard::Scancode::S) 
                    currentState = State::Down;
                else if (kp->scancode == sf::Keyboard::Scancode::W) 
                    currentState = State::Up;
            }
        }

        if (!player_moving) {
            int nx = player_gx, ny = player_gy;
            if (currentState == State::Right) 
                nx++;
            else if (currentState == State::Left)
                nx--;
            else if (currentState == State::Down)
                ny++;
            else if (currentState == State::Up)
                ny--;

            if (harita[ny][nx] != 1) {
                if (harita[ny][nx] == 3) point++;
                if (harita[ny][nx] == 4 || harita[ny][nx] == 5) window->close(); // hayalete girdi
                harita[player_gy][player_gx] = 0;
                harita[ny][nx] = 2;
                player_tgx = nx; player_tgy = ny;
                player_moving = true;
            }
        } else {
            float tx = player_tgx * 40.f + 20.f, ty = player_tgy * 40.f + 20.f;
            float dx = tx - ppx, dy = ty - ppy;
            float dist = std::sqrt(dx*dx + dy*dy);
            float step = player_speed * dt;
            if (step >= dist) {
                ppx = tx; ppy = ty;
                player_gx = player_tgx; player_gy = player_tgy;
                player_moving = false;
            } else {
                ppx += (dx / dist) * step;
                ppy += (dy / dist) * step;
            }
        }

        if (!blinky_moving) {
            Point next = getBlinkyNextStep(harita, blinky_gx, blinky_gy, player_gx, player_gy);
            if (next.x >= 0) {
                if (next.x == player_gx && next.y == player_gy) window->close();
                harita[blinky_gy][blinky_gx] = blinkyUnder;
                blinkyUnder = (harita[next.y][next.x] == 2) ? 0 : harita[next.y][next.x];
                harita[next.y][next.x] = 4;
                blinky_tgx = next.x; blinky_tgy = next.y;
                blinky_moving = true;
            }
        } else {
            float tx = blinky_tgx * 40.f + 20.f, ty = blinky_tgy * 40.f + 20.f;
            float dx = tx - bpx, dy = ty - bpy;
            float dist = std::sqrt(dx*dx + dy*dy);
            float step = blinky_speed * dt;
            if (step >= dist) {
                bpx = tx; bpy = ty;
                blinky_gx = blinky_tgx; blinky_gy = blinky_tgy; 
                blinky_moving = false;
                if (blinky_gx == player_gx && blinky_gy == player_gy) window->close();
            } else {
                bpx += (dx / dist) * step;
                bpy += (dy / dist) * step;
            }
        }

        if (!pinky_moving) {
            Point next = getPinkyNextStep(harita, pinky_gx, pinky_gy, player_gx, player_gy, currentState);
            if (next.x >= 0) {
                if (next.x == player_gx && next.y == player_gy) window->close();
                harita[pinky_gy][pinky_gx] = pinkyUnder;
                pinkyUnder = (harita[next.y][next.x] == 2) ? 0 : harita[next.y][next.x];
                harita[next.y][next.x] = 5; 
                pinky_tgx = next.x; pinky_tgy = next.y;
                pinky_moving = true;
            }
        } else {
            float tx = pinky_tgx * 40.f + 20.f, ty = pinky_tgy * 40.f + 20.f;
            float dx = tx - pkx, dy = ty - pky;
            float dist = std::sqrt(dx*dx + dy*dy);
            float step = pinky_speed * dt;
            if (step >= dist) {
                pkx = tx; pky = ty;
                pinky_gx = pinky_tgx; pinky_gy = pinky_tgy; 
                pinky_moving = false;
                if (pinky_gx == player_gx && pinky_gy == player_gy) window->close();
            } else {
                pkx += (dx / dist) * step;
                pky += (dy / dist) * step;
            }
        }

        window->clear();
        for (int y = 0; y < 15; y++) {
            for (int x = 0; x < 20; x++) {
                if (harita[y][x] == 1) {
                    duvar.setPosition({x * 40.f, y * 40.f});
                    window->draw(duvar);
                } else if (harita[y][x] == 3) {
                    pointShape.setPosition({x * 40.f + 12.f, y * 40.f + 14.f});
                    window->draw(pointShape);
                }
            }
        }

        pacman.setPosition({ppx, ppy});
        blinkySprite.setPosition({bpx, bpy});
        pinkySprite.setPosition({pkx, pky});
        window->draw(blinkySprite);
        window->draw(pinkySprite);
        window->draw(pacman);
        window->draw(text);
        window->display();
    }

    delete window;
    return 0;
}