#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>
#include <iostream>
#include <queue>
#include <cmath>

struct Point { int x, y; };

enum class State { Left, Right, Up, Down };

const int HARITA_SATIR = 36;
const int HARITA_SUTUN = 28;

Point getBlinkyNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int bx, int by, int px, int py) {
    if (bx == px && by == py) 
        return {-1, -1};
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    bool visited[HARITA_SATIR][HARITA_SUTUN] = {};
    Point parent[HARITA_SATIR][HARITA_SUTUN];
    for (int i = 0; i < HARITA_SATIR; i++)
        for (int j = 0; j < HARITA_SUTUN; j++)
            parent[i][j] = {-1, -1};

    std::queue<Point> q;
    q.push({bx, by});
    visited[by][bx] = true;

    bool found = false;
    while (!q.empty()) {
        Point curr = q.front(); q.pop();
        if (curr.x == px && curr.y == py) { 
            found = true; 
            break; 
        }
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }
    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = bx + dx[i], ny = by + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                return {nx, ny};
            }
        }
        return {-1, -1};
    }

    Point step = {px, py};
    while (parent[step.y][step.x].x != bx || parent[step.y][step.x].y != by) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) break;
        step = next;
    }
    return step;
}

Point getInkyNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int ix, int iy, int bx, int by, int px, int py, State currentState) {
    if (ix == px && iy == py) 
        return {-1, -1};

    // 1. Inky'nin Orijinal Hedef (Target) Hesaplaması
    int t2x = px, t2y = py;
    if (currentState == State::Right)      t2x = px + 2;
    else if (currentState == State::Left)  t2x = px - 2;
    else if (currentState == State::Down)  t2y = py + 2;
    else if (currentState == State::Up)    t2y = py - 2;
    
    int tx = bx + 2 * (t2x - bx);
    int ty = by + 2 * (t2y - by); 
    tx = std::max(0, std::min(HARITA_SUTUN - 1, tx));
    ty = std::max(0, std::min(HARITA_SATIR - 1, ty));

    // Hedef bir duvarın içindeyse, duvar dışına çıkana kadar kaydır
    while (tx >= 0 && tx < HARITA_SUTUN && ty >= 0 && ty < HARITA_SATIR && harita[ty][tx] == 1) {
        if      (currentState == State::Right) tx--;
        else if (currentState == State::Left)  tx++;
        else if (currentState == State::Down)  ty--;
        else if (currentState == State::Up)    ty++;
    }
    
    // Sınırları tekrar güvene al
    tx = std::max(0, std::min(HARITA_SUTUN - 1, tx));
    ty = std::max(0, std::min(HARITA_SATIR - 1, ty));

    if (ix == tx && iy == ty) 
        return {-1, -1};
        
    // 2. BFS Algoritması (Takılmaları önleyen ve labirenti çözen sistem)
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool visited[HARITA_SATIR][HARITA_SUTUN] = {};
    Point parent[HARITA_SATIR][HARITA_SUTUN];
    for (int i = 0; i < HARITA_SATIR; i++)
        for (int j = 0; j < HARITA_SUTUN; j++)
            parent[i][j] = {-1, -1};

    std::queue<Point> q;
    q.push({ix, iy});
    visited[iy][ix] = true;

    bool found = false;
    while (!q.empty()) {
        Point curr = q.front(); q.pop();
        if (curr.x == tx && curr.y == ty) { 
            found = true; 
            break; 
        }
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    // Hedefe ulaşılamıyorsa (etrafı tamamen kapalıysa) olduğu yerde donmaması için ilk geçerli adıma git
    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = ix + dx[i], ny = iy + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                return {nx, ny};
            }
        }
        return {-1, -1};
    }

    // Bulunan rotadan geriye doğru giderek Inky'nin atması gereken İLK adımı bul
    Point step = {tx, ty};
    while (parent[step.y][step.x].x != ix || parent[step.y][step.x].y != iy) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) break;
        step = next;
    }
    return step;
}

Point getPinkyNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int bx, int by, int px, int py, State currentState) {
    int tx = px, ty = py;
    if (currentState == State::Right) 
        tx = px + 4;
    else if (currentState == State::Left)  
        tx = px - 4;
    else if (currentState == State::Down)  
        ty = py + 4;
    else if (currentState == State::Up)    
        ty = py - 4;

    tx = std::max(0, std::min(HARITA_SUTUN - 1, tx));
    ty = std::max(0, std::min(HARITA_SATIR - 1, ty));

    while (harita[ty][tx] == 1) {
        if(currentState == State::Right) tx--;
        else if (currentState == State::Left)  tx++;
        else if (currentState == State::Down)  ty--;
        else if (currentState == State::Up)    ty++;
    }

    if (bx == tx && by == ty) 
        return {-1, -1};

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    bool visited[HARITA_SATIR][HARITA_SUTUN] = {};
    Point parent[HARITA_SATIR][HARITA_SUTUN];
    for (int i = 0; i < HARITA_SATIR; i++)
        for (int j = 0; j < HARITA_SUTUN; j++)
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
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }
    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = bx + dx[i], ny = by + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                return {nx, ny};
            }
        }
        return {-1, -1};
    }

    Point step = {tx, ty};
    while (parent[step.y][step.x].x != bx || parent[step.y][step.x].y != by) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) break;
        step = next;
    }
    return step;
}

Point getClydeNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int cx, int cy, int px, int py) {
    if (cx == px && cy == py) 
        return {-1, -1};

    float dX = cx - px;
    float dY = cy - py;
    float dTp = std::sqrt(dX * dX + dY * dY); 
    
    int tx, ty;
    if(dTp > 8.0f){
        tx = px;
        ty = py;
    } else {
        tx = 1;
        ty = 29;
    }

    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    bool visited[HARITA_SATIR][HARITA_SUTUN] = {};
    Point parent[HARITA_SATIR][HARITA_SUTUN];
    for (int i = 0; i < HARITA_SATIR; i++)
        for (int j = 0; j < HARITA_SUTUN; j++)
            parent[i][j] = {-1, -1};

    std::queue<Point> q;
    q.push({cx, cy});
    visited[cy][cx] = true;

    bool found = false;
    while (!q.empty()) {
        Point curr = q.front(); q.pop();
        if (curr.x == tx && curr.y == ty) { 
            found = true; 
            break; 
        }
        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i], ny = curr.y + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i], ny = cy + dy[i];
            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                return {nx, ny};
            }
        }
        return {-1, -1};
    }

    Point step = {tx, ty};
    while (parent[step.y][step.x].x != cx || parent[step.y][step.x].y != cy) {
        Point next = parent[step.y][step.x];
        if (next.x < 0 || next.y < 0) break;
        step = next;
    }
    return step;
}

int main() {
    unsigned int width = 1920, height = 1080;
    sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({width, height}), "PACMAN");

    float tileSize = 30.f;
    float offsetX = (width - (HARITA_SUTUN * tileSize)) / 2.f; 
    float offsetY = 0.f; 

    std::vector<sf::Texture> texUp(3), texDown(3), texLeft(3), texRight(3);
    sf::Texture blinkyTexture, pinkyTexture, inkyTexture, clydeTexture;
    blinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/blinky.png");
    pinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/pinky.png");
    inkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/inky.png");
    clydeTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/clyde.png");
    
    for (int i = 0; i < 3; i++) {
        texUp[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_" + std::to_string(i+1) + ".png");
        texDown[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_" + std::to_string(i+1) + ".png");
        texLeft[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_" + std::to_string(i+1) + ".png");
        texRight[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_" + std::to_string(i+1) + ".png");
    }

    float texW = texRight[0].getSize().x;
    float texH = texRight[0].getSize().y;

    sf::Sprite pacman(texRight[0]);
    pacman.setScale({tileSize / texW, tileSize / texH});
    pacman.setOrigin({texW / 2.f, texH / 2.f});

    sf::Sprite blinkySprite(blinkyTexture); 
    blinkySprite.setScale({tileSize / texW, tileSize / texH});
    blinkySprite.setOrigin({texW / 2.f, texH / 2.f});

    sf::Sprite pinkySprite(pinkyTexture);   
    pinkySprite.setScale({tileSize / texW, tileSize / texH});
    pinkySprite.setOrigin({texW / 2.f, texH / 2.f});
    
    sf::Sprite inkySprite(inkyTexture);
    inkySprite.setScale({tileSize / texW, tileSize / texH});
    inkySprite.setOrigin({texW / 2.f, texH / 2.f});
    
    sf::Sprite clydeSprite(clydeTexture);
    clydeSprite.setScale({tileSize / texW, tileSize / texH});
    clydeSprite.setOrigin({texW / 2.f, texH / 2.f});

    sf::Clock textureClock;
    float frameTime = 0.1f;
    int currentTexture = 0;
    State currentState = State::Right;
    State previousState = State::Right;

    sf::RectangleShape duvar;
    duvar.setSize({tileSize, tileSize});
    duvar.setOutlineColor(sf::Color::Red);
    duvar.setOutlineThickness(2.f);
    duvar.setFillColor(sf::Color::Blue);

    sf::CircleShape pointShape;
    pointShape.setRadius(tileSize / 8.f);
    pointShape.setFillColor(sf::Color::White);

    int harita[HARITA_SATIR][HARITA_SUTUN] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,1},
    {1,3,1,1,1,1,3,1,1,1,1,1,3,1,1,3,1,1,1,1,1,3,1,1,1,1,3,1},
    {1,3,1,1,1,1,3,1,1,1,1,1,3,1,1,3,1,1,1,1,1,3,1,1,1,1,3,1},
    {1,3,1,1,1,1,3,1,1,1,1,1,3,1,1,3,1,1,1,1,1,3,1,1,1,1,3,1},
    {1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
    {1,3,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,3,1},
    {1,3,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,3,1},
    {1,3,3,3,3,3,3,1,1,3,3,3,3,1,1,3,3,3,3,1,1,3,3,3,3,3,3,1},
    {1,1,1,1,1,1,3,1,1,1,1,1,0,1,1,0,1,1,1,1,1,3,1,1,1,1,1,1},
    {0,0,0,0,0,1,3,1,1,1,1,1,0,1,1,0,1,1,1,1,1,3,1,0,0,0,0,0},
    {0,0,0,0,0,1,3,1,1,0,0,0,0,0,0,0,0,0,0,1,1,3,1,0,0,0,0,0},
    {0,0,0,0,0,1,3,1,1,0,1,1,1,0,0,1,1,1,0,1,1,3,1,0,0,0,0,0},
    {1,1,1,1,1,1,3,1,1,0,1,0,0,0,0,0,0,1,0,1,1,3,1,1,1,1,1,1},
    {9,0,0,0,0,0,3,0,0,0,1,0,4,5,6,7,0,1,0,0,0,3,0,0,0,0,0,10}, 
    {1,1,1,1,1,1,3,1,1,0,1,0,0,0,0,0,0,1,0,1,1,3,1,1,1,1,1,1},
    {0,0,0,0,0,1,3,1,1,0,1,1,1,1,1,1,1,1,0,1,1,3,1,0,0,0,0,0},
    {0,0,0,0,0,1,3,1,1,0,0,0,0,0,0,0,0,0,0,1,1,3,1,0,0,0,0,0},
    {0,0,0,0,0,1,3,1,1,0,1,1,1,1,1,1,1,1,0,1,1,3,1,0,0,0,0,0},
    {1,1,1,1,1,1,3,1,1,0,1,1,1,1,1,1,1,1,0,1,1,3,1,1,1,1,1,1},
    {1,3,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,3,1},
    {1,3,1,1,1,1,3,1,1,1,1,1,3,1,1,3,1,1,1,1,1,3,1,1,1,1,3,1},
    {1,3,1,1,1,1,3,1,1,1,1,1,3,1,1,3,1,1,1,1,1,3,1,1,1,1,3,1},
    {1,3,3,3,1,1,3,3,3,3,3,3,3,2,0,3,3,3,3,3,3,3,1,1,3,3,3,1}, 
    {1,1,1,3,1,1,3,1,1,3,1,1,1,1,1,1,1,1,3,1,1,3,1,1,3,1,1,1},
    {1,1,1,3,1,1,3,1,1,3,1,1,1,1,1,1,1,1,3,1,1,3,1,1,3,1,1,1},
    {1,3,3,3,3,3,3,1,1,3,3,3,3,1,1,3,3,3,3,1,1,3,3,3,3,3,3,1},
    {1,3,1,1,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,1,1,1,3,1},
    {1,3,1,1,1,1,1,1,1,1,1,1,3,1,1,3,1,1,1,1,1,1,1,1,1,1,3,1},
    {1,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    int player_gx = 0, player_gy = 0;
    int blinky_gx = 0, blinky_gy = 0, blinkyUnder = 0;
    int pinky_gx  = 0, pinky_gy  = 0, pinkyUnder  = 0;
    int inky_gx   = 0, inky_gy   = 0, inkyUnder   = 0;
    int clyde_gx  = 0, clyde_gy  = 0, clydeUnder  = 0;

    for (int y = 0; y < HARITA_SATIR; y++) {
        for (int x = 0; x < HARITA_SUTUN; x++) {
            if      (harita[y][x] == 2) { player_gx = x; player_gy = y; }
            else if (harita[y][x] == 4) { blinky_gx = x; blinky_gy = y; }
            else if (harita[y][x] == 5) { pinky_gx  = x; pinky_gy  = y; }
            else if (harita[y][x] == 6) { inky_gx   = x; inky_gy   = y; }  
            else if (harita[y][x] == 7) { clyde_gx  = x; clyde_gy  = y; }  
        }
    }

    float ppx = player_gx * tileSize + tileSize / 2.f + offsetX, ppy = player_gy * tileSize + tileSize / 2.f + offsetY;
    float bpx = blinky_gx * tileSize + tileSize / 2.f + offsetX, bpy = blinky_gy * tileSize + tileSize / 2.f + offsetY;
    float pkx = pinky_gx  * tileSize + tileSize / 2.f + offsetX, pky = pinky_gy  * tileSize + tileSize / 2.f + offsetY; 
    float ikx = inky_gx   * tileSize + tileSize / 2.f + offsetX, iky = inky_gy   * tileSize + tileSize / 2.f + offsetY;
    float ckx = clyde_gx  * tileSize + tileSize / 2.f + offsetX, cky = clyde_gy  * tileSize + tileSize / 2.f + offsetY;

    int player_tgx = player_gx, player_tgy = player_gy;
    int blinky_tgx = blinky_gx, blinky_tgy = blinky_gy;
    int pinky_tgx  = pinky_gx,  pinky_tgy  = pinky_gy;
    int inky_tgx   = inky_gx,   inky_tgy   = inky_gy;
    int clyde_tgx  = clyde_gx,  clyde_tgy  = clyde_gy;

    bool player_moving = false;
    bool blinky_moving = false;
    bool pinky_moving  = false;
    bool inky_moving   = false;
    bool clyde_moving  = false;

    float player_speed = 150.f;
    float blinky_speed = 100.f;
    float pinky_speed  = 110.f;
    float inky_speed   = 110.f;
    float clyde_speed  = 110.f;
    
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
        text.setPosition({offsetX, 10.f});

        if (currentState != previousState) {
            currentTexture = 0;
            textureClock.restart();
            previousState = currentState;
        }
        if (textureClock.getElapsedTime().asSeconds() >= frameTime) {
            currentTexture++;
            std::vector<sf::Texture>* frames = nullptr;
            if      (currentState == State::Right) frames = &texRight;
            else if (currentState == State::Left)  frames = &texLeft;
            else if (currentState == State::Up)    frames = &texUp;
            else                                   frames = &texDown;
            
            if (currentTexture >= (int)frames->size()) currentTexture = 0;
            pacman.setTexture((*frames)[currentTexture]);
            textureClock.restart();
        }

        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) window->close();
            else if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                if      (kp->scancode == sf::Keyboard::Scancode::Escape) window->close();
                else if (kp->scancode == sf::Keyboard::Scancode::D)      currentState = State::Right;
                else if (kp->scancode == sf::Keyboard::Scancode::A)      currentState = State::Left;
                else if (kp->scancode == sf::Keyboard::Scancode::S)      currentState = State::Down;
                else if (kp->scancode == sf::Keyboard::Scancode::W)      currentState = State::Up;
            }
        }

        if (!player_moving) {
            int nx = player_gx, ny = player_gy;
            if (currentState == State::Right)      nx++;
            else if (currentState == State::Left)  nx--;
            else if (currentState == State::Down)  ny++;
            else if (currentState == State::Up)    ny--;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                if (harita[ny][nx] == 3) point++;
                if (harita[ny][nx] == 4 || harita[ny][nx] == 5 || harita[ny][nx] == 6 || harita[ny][nx] == 7) window->close();
                harita[player_gy][player_gx] = 0;
                harita[ny][nx] = 2;
                player_tgx = nx; player_tgy = ny;
                player_moving = true;
            }
        } else {
            float tx = player_tgx * tileSize + tileSize / 2.f + offsetX, ty = player_tgy * tileSize + tileSize / 2.f + offsetY;
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
                int val = harita[next.y][next.x];
                blinkyUnder = (val == 2 || val == 4 || val == 5 || val == 6 || val == 7) ? 0 : val;
                harita[next.y][next.x] = 4;
                blinky_tgx = next.x; blinky_tgy = next.y;
                blinky_moving = true;
            }
        } else {
            float tx = blinky_tgx * tileSize + tileSize / 2.f + offsetX, ty = blinky_tgy * tileSize + tileSize / 2.f + offsetY;
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

        if (!inky_moving) {
            Point next = getInkyNextStep(harita, inky_gx, inky_gy, blinky_gx, blinky_gy, player_gx, player_gy, currentState);
            if(next.x >= 0) {
                if (next.x == player_gx && next.y == player_gy) window->close();
                harita[inky_gy][inky_gx] = inkyUnder;
                int val = harita[next.y][next.x];
                inkyUnder = (val == 2 || val == 4 || val == 5 || val == 6 || val == 7) ? 0 : val;
                harita[next.y][next.x] = 6;
                inky_tgx = next.x; inky_tgy = next.y;
                inky_moving = true;
            }
        } else {
            float tx = inky_tgx * tileSize + tileSize / 2.f + offsetX, ty = inky_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - ikx, dy = ty - iky;
            float dist = std::sqrt(dx*dx + dy*dy);
            float step = inky_speed * dt;
            if (step >= dist) {
                ikx = tx; iky = ty;
                inky_gx = inky_tgx; inky_gy = inky_tgy;
                inky_moving = false;
                if(inky_gx == player_gx && inky_gy == player_gy) window->close();
            } else {
                ikx += (dx / dist) * step;
                iky += (dy / dist) * step;
            }
        }

        if (!pinky_moving) {
            Point next = getPinkyNextStep(harita, pinky_gx, pinky_gy, player_gx, player_gy, currentState);
            if (next.x >= 0) {
                if (next.x == player_gx && next.y == player_gy) window->close();
                harita[pinky_gy][pinky_gx] = pinkyUnder;
                int val = harita[next.y][next.x];
                pinkyUnder = (val == 2 || val == 4 || val == 5 || val == 6 || val == 7) ? 0 : val;
                harita[next.y][next.x] = 5;
                pinky_tgx = next.x; pinky_tgy = next.y;
                pinky_moving = true;
            }
        } else {
            float tx = pinky_tgx * tileSize + tileSize / 2.f + offsetX, ty = pinky_tgy * tileSize + tileSize / 2.f + offsetY;
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

        if (!clyde_moving) {
            Point next = getClydeNextStep(harita, clyde_gx, clyde_gy, player_gx, player_gy);
            if (next.x >= 0) {
                if (next.x == player_gx && next.y == player_gy) window->close();
                harita[clyde_gy][clyde_gx] = clydeUnder;
                int val = harita[next.y][next.x];
                clydeUnder = (val == 2 || val == 4 || val == 5 || val == 6 || val == 7) ? 0 : val;
                harita[next.y][next.x] = 7;
                clyde_tgx = next.x; clyde_tgy = next.y;
                clyde_moving = true;
            }
        } else {
            float tx = clyde_tgx * tileSize + tileSize / 2.f + offsetX, ty = clyde_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - ckx, dy = ty - cky;
            float dist = std::sqrt(dx*dx + dy*dy);
            float step = clyde_speed * dt;
            if (step >= dist) {
                ckx = tx; cky = ty;
                clyde_gx = clyde_tgx; clyde_gy = clyde_tgy; 
                clyde_moving = false;
                if (clyde_gx == player_gx && clyde_gy == player_gy) window->close();
            } else {
                ckx += (dx / dist) * step;
                cky += (dy / dist) * step;
            }
        }

        window->clear();
        for (int y = 0; y < HARITA_SATIR; y++) {
            for (int x = 0; x < HARITA_SUTUN; x++) {
                if (harita[y][x] == 1) {
                    duvar.setPosition({x * tileSize + offsetX, y * tileSize + offsetY});
                    window->draw(duvar);
                } else if (harita[y][x] == 3) {
                    pointShape.setPosition({x * tileSize + offsetX + (tileSize / 2.f - pointShape.getRadius()), 
                                            y * tileSize + offsetY + (tileSize / 2.f - pointShape.getRadius())});
                    window->draw(pointShape);
                }
            }
        }

        pacman.setPosition({ppx, ppy});
        blinkySprite.setPosition({bpx, bpy});
        pinkySprite.setPosition({pkx, pky});
        inkySprite.setPosition({ikx, iky});
        clydeSprite.setPosition({ckx, cky});
        
        window->draw(clydeSprite);
        window->draw(blinkySprite);
        window->draw(pinkySprite);
        window->draw(inkySprite);
        window->draw(pacman);
        window->draw(text);
        
        window->display();
    }
    delete window;
    return 0;
}