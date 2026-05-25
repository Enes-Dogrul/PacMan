#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <queue>
#include <cmath>
#include <vector>
#include <optional>

struct Point { int x, y; };

enum class State { Left, Right, Up, Down };
enum class GhostMode { Chase, Flee };

const int HARITA_SATIR = 36;
const int HARITA_SUTUN = 28;

Point applyTunnel(int x, int y) {
    if (y == 14 && x < 0) return {HARITA_SUTUN - 1, y};
    if (y == 14 && x >= HARITA_SUTUN) return {0, y};
    return {x, y};
}

bool isTunnelJump(int fromX, int fromY, int toX, int toY) {
    return fromY == 14 && toY == 14 && ((fromX == 0 && toX == HARITA_SUTUN - 1) || (fromX == HARITA_SUTUN - 1 && toX == 0));
}

float distanceSq(int x1, int y1, int x2, int y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

Point getFleeNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int gx, int gy, int px, int py) {
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};

    Point best = {-1, -1};
    float bestDistance = -1.0f;

    for (int i = 0; i < 4; i++) {
        int nx = gx + dx[i];
        int ny = gy + dy[i];

        Point tunnelPoint = applyTunnel(nx, ny);
        nx = tunnelPoint.x;
        ny = tunnelPoint.y;

        if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
            float d = distanceSq(nx, ny, px, py);

            if (d > bestDistance) {
                bestDistance = d;
                best = {nx, ny};
            }
        }
    }

    return best;
}

Point getBlinkyNextStep(int harita[HARITA_SATIR][HARITA_SUTUN], int bx, int by, int px, int py) {
    if (bx == px && by == py) return {-1, -1};

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
        Point curr = q.front();
        q.pop();

        if (curr.x == px && curr.y == py) { 
            found = true; 
            break; 
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = bx + dx[i];
            int ny = by + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

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
    if (ix == px && iy == py) return {-1, -1};

    int tx = px;
    int ty = py;

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
        Point curr = q.front(); 
        q.pop();

        if (curr.x == tx && curr.y == ty) { 
            found = true; 
            break; 
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = ix + dx[i];
            int ny = iy + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                return {nx, ny};
            }
        }
        return {-1, -1};
    }

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

    if (currentState == State::Right) tx = px + 4;
    else if (currentState == State::Left) tx = px - 4;
    else if (currentState == State::Down) ty = py + 4;
    else if (currentState == State::Up) ty = py - 4;

    tx = std::max(0, std::min(HARITA_SUTUN - 1, tx));
    ty = std::max(0, std::min(HARITA_SATIR - 1, ty));

    while (harita[ty][tx] == 1) {
        if (currentState == State::Right) tx--;
        else if (currentState == State::Left) tx++;
        else if (currentState == State::Down) ty--;
        else if (currentState == State::Up) ty++;
    }

    if (bx == tx && by == ty) return {-1, -1};

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
        Point curr = q.front(); 
        q.pop();

        if (curr.x == tx && curr.y == ty) { 
            found = true; 
            break; 
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = bx + dx[i];
            int ny = by + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

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
    if (cx == px && cy == py) return {-1, -1};

    float dX = cx - px;
    float dY = cy - py;
    float dTp = std::sqrt(dX * dX + dY * dY); 
    
    int tx, ty;

    if (dTp > 8.0f) {
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
        Point curr = q.front(); 
        q.pop();

        if (curr.x == tx && curr.y == ty) { 
            found = true; 
            break; 
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && !visited[ny][nx] && harita[ny][nx] != 1) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }

    if (!found) {
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

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
    sf::Texture blinkyTexture, pinkyTexture, inkyTexture, clydeTexture, scaredTexture;

    blinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/blinky.png");
    pinkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/pinky.png");
    inkyTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/inky.png");
    clydeTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/clyde.png");
    scaredTexture.loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/blue_ghost.png");
    
    for (int i = 0; i < 3; i++) {
        texUp[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Up_" + std::to_string(i + 1) + ".png");
        texDown[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Down_" + std::to_string(i + 1) + ".png");
        texLeft[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Left_" + std::to_string(i + 1) + ".png");
        texRight[i].loadFromFile("/Users/enes/Desktop/PACMAN/textures/pacman/Right_" + std::to_string(i + 1) + ".png");
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

    // Death animation state
    bool isDying = false;
    sf::Clock deathClock;
    float deathFrameTime = 0.3f;
    int deathFrame = 2;  // Start from last frame (frame 3 = index 2)

    State currentState = State::Right;
    State previousState = State::Right;

    GhostMode ghostMode = GhostMode::Chase;
    sf::Clock fleeClock;
    float fleeDuration = 7.0f;

    sf::RectangleShape duvar;
    duvar.setSize({tileSize, tileSize});
    duvar.setOutlineColor(sf::Color::Red);
    duvar.setOutlineThickness(2.f);
    duvar.setFillColor(sf::Color::Blue);

    sf::CircleShape pointShape;
    pointShape.setRadius(tileSize / 8.f);
    pointShape.setFillColor(sf::Color::White);

    sf::CircleShape powerPelletShape;
    powerPelletShape.setRadius(tileSize / 3.f);
    powerPelletShape.setFillColor(sf::Color::White);

    int harita[HARITA_SATIR][HARITA_SUTUN] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,8,3,3,3,3,3,3,3,3,3,3,3,1,1,3,3,3,3,3,3,3,3,3,3,3,8,1},
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
    {1,8,3,3,1,1,3,3,3,3,3,3,3,2,0,3,3,3,3,3,3,3,1,1,3,3,8,1}, 
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
    int blinky_gx = 0, blinky_gy = 0;
    int pinky_gx  = 0, pinky_gy  = 0;
    int inky_gx   = 0, inky_gy   = 0;
    int clyde_gx  = 0, clyde_gy  = 0;

    int player_spawn_x = 0, player_spawn_y = 0;
    int blinky_spawn_x = 0, blinky_spawn_y = 0;
    int pinky_spawn_x  = 0, pinky_spawn_y  = 0;
    int inky_spawn_x   = 0, inky_spawn_y   = 0;
    int clyde_spawn_x  = 0, clyde_spawn_y  = 0;

    for (int y = 0; y < HARITA_SATIR; y++) {
        for (int x = 0; x < HARITA_SUTUN; x++) {
            if      (harita[y][x] == 2) { player_gx = x; player_gy = y; player_spawn_x = x; player_spawn_y = y; harita[y][x] = 0; }
            else if (harita[y][x] == 4) { blinky_gx = x; blinky_gy = y; blinky_spawn_x = x; blinky_spawn_y = y; harita[y][x] = 0; }
            else if (harita[y][x] == 5) { pinky_gx  = x; pinky_gy  = y; pinky_spawn_x  = x; pinky_spawn_y  = y; harita[y][x] = 0; }
            else if (harita[y][x] == 6) { inky_gx   = x; inky_gy   = y; inky_spawn_x   = x; inky_spawn_y   = y; harita[y][x] = 0; }
            else if (harita[y][x] == 7) { clyde_gx  = x; clyde_gy  = y; clyde_spawn_x  = x; clyde_spawn_y  = y; harita[y][x] = 0; }
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
    int lives = 3;

    sf::Sprite lifeIcon(texRight[0]);
    float lifeIconScale = 0.6f;
    lifeIcon.setScale({(tileSize / texW) * lifeIconScale, (tileSize / texH) * lifeIconScale});

    sf::Clock deltaClock;
    sf::Font font("/Users/enes/Desktop/PACMAN/src/arial.ttf");
    sf::Text text(font);

    auto isGhostAt = [&](int x, int y) {
        return (x == blinky_gx && y == blinky_gy) || (x == blinky_tgx && y == blinky_tgy) ||
               (x == pinky_gx  && y == pinky_gy)  || (x == pinky_tgx  && y == pinky_tgy) ||
               (x == inky_gx   && y == inky_gy)   || (x == inky_tgx   && y == inky_tgy) ||
               (x == clyde_gx  && y == clyde_gy)  || (x == clyde_tgx  && y == clyde_tgy);
    };

    sf::SoundBuffer startBuffer("/Users/enes/Desktop/PACMAN/src/Pac-Man Start.wav");
    sf::Sound startSound(startBuffer);
    startSound.play();

    while (startSound.getStatus() == sf::Sound::Status::Playing) {
        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
        }
        if (!window->isOpen()) break;

        window->clear();
        for (int y = 0; y < HARITA_SATIR; y++) {
            for (int x = 0; x < HARITA_SUTUN; x++) {
                if (harita[y][x] == 1) {
                    duvar.setPosition({x * tileSize + offsetX, y * tileSize + offsetY});
                    window->draw(duvar);
                } else if (harita[y][x] == 3) {
                    pointShape.setPosition({
                        x * tileSize + offsetX + (tileSize / 2.f - pointShape.getRadius()),
                        y * tileSize + offsetY + (tileSize / 2.f - pointShape.getRadius())
                    });
                    window->draw(pointShape);
                } else if (harita[y][x] == 8) {
                    powerPelletShape.setPosition({
                        x * tileSize + offsetX + (tileSize / 2.f - powerPelletShape.getRadius()),
                        y * tileSize + offsetY + (tileSize / 2.f - powerPelletShape.getRadius())
                    });
                    window->draw(powerPelletShape);
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

    deltaClock.restart();

    while (window->isOpen()) {
        float dt = deltaClock.restart().asSeconds();

        text.setString("Puan: " + std::to_string(point));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition({offsetX, 10.f});

        // Death animation logic
        if (isDying) {
            if (deathClock.getElapsedTime().asSeconds() >= deathFrameTime) {
                deathFrame--;
                if (deathFrame < 0) {
                    lives--;
                    if (lives <= 0) {
                        sf::Text gameOverText(font);
                        gameOverText.setString("GAME OVER");
                        gameOverText.setCharacterSize(72);
                        gameOverText.setFillColor(sf::Color::Red);
                        gameOverText.setStyle(sf::Text::Bold);
                        sf::FloatRect goBounds = gameOverText.getLocalBounds();
                        gameOverText.setOrigin({goBounds.size.x / 2.f, goBounds.size.y / 2.f});
                        gameOverText.setPosition({width / 2.f, height / 2.f - 50.f});

                        sf::Text scoreText(font);
                        scoreText.setString("Skor: " + std::to_string(point));
                        scoreText.setCharacterSize(36);
                        scoreText.setFillColor(sf::Color::White);
                        sf::FloatRect scBounds = scoreText.getLocalBounds();
                        scoreText.setOrigin({scBounds.size.x / 2.f, scBounds.size.y / 2.f});
                        scoreText.setPosition({width / 2.f, height / 2.f + 50.f});

                        while (window->isOpen()) {
                            while (const std::optional ev = window->pollEvent()) {
                                if (ev->is<sf::Event::Closed>()) window->close();
                                else if (const auto* kp2 = ev->getIf<sf::Event::KeyPressed>()) {
                                    if (kp2->scancode == sf::Keyboard::Scancode::Escape) window->close();
                                }
                            }
                            window->clear();
                            for (int y = 0; y < HARITA_SATIR; y++) {
                                for (int x = 0; x < HARITA_SUTUN; x++) {
                                    if (harita[y][x] == 1) {
                                        duvar.setPosition({x * tileSize + offsetX, y * tileSize + offsetY});
                                        window->draw(duvar);
                                    }
                                }
                            }
                            window->draw(gameOverText);
                            window->draw(scoreText);
                            window->display();
                        }
                        continue;
                    }
                    isDying = false;

                    player_gx = player_spawn_x; player_gy = player_spawn_y;
                    player_tgx = player_gx; player_tgy = player_gy;
                    ppx = player_gx * tileSize + tileSize / 2.f + offsetX;
                    ppy = player_gy * tileSize + tileSize / 2.f + offsetY;
                    player_moving = false;

                    blinky_gx = blinky_spawn_x; blinky_gy = blinky_spawn_y;
                    blinky_tgx = blinky_gx; blinky_tgy = blinky_gy;
                    bpx = blinky_gx * tileSize + tileSize / 2.f + offsetX;
                    bpy = blinky_gy * tileSize + tileSize / 2.f + offsetY;
                    blinky_moving = false;

                    pinky_gx = pinky_spawn_x; pinky_gy = pinky_spawn_y;
                    pinky_tgx = pinky_gx; pinky_tgy = pinky_gy;
                    pkx = pinky_gx * tileSize + tileSize / 2.f + offsetX;
                    pky = pinky_gy * tileSize + tileSize / 2.f + offsetY;
                    pinky_moving = false;

                    inky_gx = inky_spawn_x; inky_gy = inky_spawn_y;
                    inky_tgx = inky_gx; inky_tgy = inky_gy;
                    ikx = inky_gx * tileSize + tileSize / 2.f + offsetX;
                    iky = inky_gy * tileSize + tileSize / 2.f + offsetY;
                    inky_moving = false;

                    clyde_gx = clyde_spawn_x; clyde_gy = clyde_spawn_y;
                    clyde_tgx = clyde_gx; clyde_tgy = clyde_gy;
                    ckx = clyde_gx * tileSize + tileSize / 2.f + offsetX;
                    cky = clyde_gy * tileSize + tileSize / 2.f + offsetY;
                    clyde_moving = false;

                    currentState = State::Right;
                    previousState = State::Right;
                    ghostMode = GhostMode::Chase;
                    pacman.setTexture(texRight[0]);
                    currentTexture = 0;
                    deltaClock.restart();
                    continue;
                }
                std::vector<sf::Texture>* frames = nullptr;
                if      (currentState == State::Right) frames = &texRight;
                else if (currentState == State::Left)  frames = &texLeft;
                else if (currentState == State::Up)    frames = &texUp;
                else                                   frames = &texDown;
                pacman.setTexture((*frames)[deathFrame]);
                deathClock.restart();
            }

            // Still render during death animation
            window->clear();
            for (int y = 0; y < HARITA_SATIR; y++) {
                for (int x = 0; x < HARITA_SUTUN; x++) {
                    if (harita[y][x] == 1) {
                        duvar.setPosition({x * tileSize + offsetX, y * tileSize + offsetY});
                        window->draw(duvar);
                    } else if (harita[y][x] == 3) {
                        pointShape.setPosition({
                            x * tileSize + offsetX + (tileSize / 2.f - pointShape.getRadius()), 
                            y * tileSize + offsetY + (tileSize / 2.f - pointShape.getRadius())
                        });
                        window->draw(pointShape);
                    } else if (harita[y][x] == 8) {
                        powerPelletShape.setPosition({
                            x * tileSize + offsetX + (tileSize / 2.f - powerPelletShape.getRadius()),
                            y * tileSize + offsetY + (tileSize / 2.f - powerPelletShape.getRadius())
                        });
                        window->draw(powerPelletShape);
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
            for (int i = 0; i < lives; i++) {
                lifeIcon.setPosition({offsetX + i * (tileSize * lifeIconScale + 5.f), 940.f});
                window->draw(lifeIcon);
            }
            window->display();
            continue;
        }

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
            if (event->is<sf::Event::Closed>()) {
                window->close();
            } else if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {
                if      (kp->scancode == sf::Keyboard::Scancode::Escape) window->close();
                else if (kp->scancode == sf::Keyboard::Scancode::D)      currentState = State::Right;
                else if (kp->scancode == sf::Keyboard::Scancode::A)      currentState = State::Left;
                else if (kp->scancode == sf::Keyboard::Scancode::S)      currentState = State::Down;
                else if (kp->scancode == sf::Keyboard::Scancode::W)      currentState = State::Up;
            }
        }

        if (ghostMode == GhostMode::Flee && fleeClock.getElapsedTime().asSeconds() >= fleeDuration) {
            ghostMode = GhostMode::Chase;
        }

        if (ghostMode == GhostMode::Flee) {
            blinkySprite.setTexture(scaredTexture);
            pinkySprite.setTexture(scaredTexture);
            inkySprite.setTexture(scaredTexture);
            clydeSprite.setTexture(scaredTexture);
        } else {
            blinkySprite.setTexture(blinkyTexture);
            pinkySprite.setTexture(pinkyTexture);
            inkySprite.setTexture(inkyTexture);
            clydeSprite.setTexture(clydeTexture);
        }

        if (!player_moving) {
            int nx = player_gx;
            int ny = player_gy;

            if      (currentState == State::Right) nx++;
            else if (currentState == State::Left)  nx--;
            else if (currentState == State::Down)  ny++;
            else if (currentState == State::Up)    ny--;

            Point tunnelPoint = applyTunnel(nx, ny);
            nx = tunnelPoint.x;
            ny = tunnelPoint.y;

            bool playerTunnelJump = isTunnelJump(player_gx, player_gy, nx, ny);

            if (nx >= 0 && nx < HARITA_SUTUN && ny >= 0 && ny < HARITA_SATIR && harita[ny][nx] != 1) {
                if (harita[ny][nx] == 3) point++;
                if (harita[ny][nx] == 8) {
                    point += 50;
                    ghostMode = GhostMode::Flee;
                    fleeClock.restart();
                }

                if (isGhostAt(nx, ny)) {
                    if (ghostMode == GhostMode::Flee) point += 10;
                    else { isDying = true; deathFrame = 2; deathClock.restart(); std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown; pacman.setTexture((*f)[deathFrame]); }
                }

                harita[player_gy][player_gx] = 0;
                harita[ny][nx] = 0; 

                player_tgx = nx;
                player_tgy = ny;

                if (playerTunnelJump) {
                    ppx = nx * tileSize + tileSize / 2.f + offsetX;
                    ppy = ny * tileSize + tileSize / 2.f + offsetY;
                    player_gx = nx;
                    player_gy = ny;
                    player_moving = false;
                } else {
                    player_moving = true;
                }
            }
        } else {
            float tx = player_tgx * tileSize + tileSize / 2.f + offsetX;
            float ty = player_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - ppx;
            float dy = ty - ppy;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = player_speed * dt;

            if (step >= dist) {
                ppx = tx;
                ppy = ty;
                player_gx = player_tgx;
                player_gy = player_tgy;
                player_moving = false;
            } else {
                ppx += (dx / dist) * step;
                ppy += (dy / dist) * step;
            }
        }

        if (!blinky_moving) {
            Point next;
            if (ghostMode == GhostMode::Flee) {
                next = getFleeNextStep(harita, blinky_gx, blinky_gy, player_gx, player_gy);
            } else {
                next = getBlinkyNextStep(harita, blinky_gx, blinky_gy, player_gx, player_gy);
            }

            if (next.x >= 0) {
                bool blinkyTunnelJump = isTunnelJump(blinky_gx, blinky_gy, next.x, next.y);

                if (next.x == player_gx && next.y == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }

                blinky_tgx = next.x;
                blinky_tgy = next.y;

                if (blinkyTunnelJump) {
                    bpx = next.x * tileSize + tileSize / 2.f + offsetX;
                    bpy = next.y * tileSize + tileSize / 2.f + offsetY;
                    blinky_gx = next.x;
                    blinky_gy = next.y;
                    blinky_moving = false;
                } else {
                    blinky_moving = true;
                }
            }
        } else {
            float tx = blinky_tgx * tileSize + tileSize / 2.f + offsetX;
            float ty = blinky_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - bpx;
            float dy = ty - bpy;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = blinky_speed * dt;

            if (step >= dist) {
                bpx = tx;
                bpy = ty;
                blinky_gx = blinky_tgx;
                blinky_gy = blinky_tgy; 
                blinky_moving = false;

                if (blinky_gx == player_gx && blinky_gy == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }
            } else {
                bpx += (dx / dist) * step;
                bpy += (dy / dist) * step;
            }
        }

        if (!inky_moving) {
            Point next;
            if (ghostMode == GhostMode::Flee) {
                next = getFleeNextStep(harita, inky_gx, inky_gy, player_gx, player_gy);
            } else {
                next = getInkyNextStep(harita, inky_gx, inky_gy, blinky_gx, blinky_gy, player_gx, player_gy, currentState);
            }

            if (next.x >= 0) {
                bool inkyTunnelJump = isTunnelJump(inky_gx, inky_gy, next.x, next.y);

                if (next.x == player_gx && next.y == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }

                inky_tgx = next.x;
                inky_tgy = next.y;

                if (inkyTunnelJump) {
                    ikx = next.x * tileSize + tileSize / 2.f + offsetX;
                    iky = next.y * tileSize + tileSize / 2.f + offsetY;
                    inky_gx = next.x;
                    inky_gy = next.y;
                    inky_moving = false;
                } else {
                    inky_moving = true;
                }
            }
        } else {
            float tx = inky_tgx * tileSize + tileSize / 2.f + offsetX;
            float ty = inky_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - ikx;
            float dy = ty - iky;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = inky_speed * dt;

            if (step >= dist) {
                ikx = tx;
                iky = ty;
                inky_gx = inky_tgx;
                inky_gy = inky_tgy;
                inky_moving = false;

                if (inky_gx == player_gx && inky_gy == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }
            } else {
                ikx += (dx / dist) * step;
                iky += (dy / dist) * step;
            }
        }

        if (!pinky_moving) {
            Point next;
            if (ghostMode == GhostMode::Flee) {
                next = getFleeNextStep(harita, pinky_gx, pinky_gy, player_gx, player_gy);
            } else {
                next = getPinkyNextStep(harita, pinky_gx, pinky_gy, player_gx, player_gy, currentState);
            }

            if (next.x >= 0) {
                bool pinkyTunnelJump = isTunnelJump(pinky_gx, pinky_gy, next.x, next.y);

                if (next.x == player_gx && next.y == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }

                pinky_tgx = next.x;
                pinky_tgy = next.y;

                if (pinkyTunnelJump) {
                    pkx = next.x * tileSize + tileSize / 2.f + offsetX;
                    pky = next.y * tileSize + tileSize / 2.f + offsetY;
                    pinky_gx = next.x;
                    pinky_gy = next.y;
                    pinky_moving = false;
                } else {
                    pinky_moving = true;
                }
            }
        } else {
            float tx = pinky_tgx * tileSize + tileSize / 2.f + offsetX;
            float ty = pinky_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - pkx;
            float dy = ty - pky;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = pinky_speed * dt;

            if (step >= dist) {
                pkx = tx;
                pky = ty;
                pinky_gx = pinky_tgx;
                pinky_gy = pinky_tgy; 
                pinky_moving = false;

                if (pinky_gx == player_gx && pinky_gy == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }
            } else {
                pkx += (dx / dist) * step;
                pky += (dy / dist) * step;
            }
        }

        if (!clyde_moving) {
            Point next;
            if (ghostMode == GhostMode::Flee) {
                next = getFleeNextStep(harita, clyde_gx, clyde_gy, player_gx, player_gy);
            } else {
                next = getClydeNextStep(harita, clyde_gx, clyde_gy, player_gx, player_gy);
            }

            if (next.x >= 0) {
                bool clydeTunnelJump = isTunnelJump(clyde_gx, clyde_gy, next.x, next.y);

                if (next.x == player_gx && next.y == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }

                clyde_tgx = next.x;
                clyde_tgy = next.y;

                if (clydeTunnelJump) {
                    ckx = next.x * tileSize + tileSize / 2.f + offsetX;
                    cky = next.y * tileSize + tileSize / 2.f + offsetY;
                    clyde_gx = next.x;
                    clyde_gy = next.y;
                    clyde_moving = false;
                } else {
                    clyde_moving = true;
                }
            }
        } else {
            float tx = clyde_tgx * tileSize + tileSize / 2.f + offsetX;
            float ty = clyde_tgy * tileSize + tileSize / 2.f + offsetY;
            float dx = tx - ckx;
            float dy = ty - cky;
            float dist = std::sqrt(dx * dx + dy * dy);
            float step = clyde_speed * dt;

            if (step >= dist) {
                ckx = tx;
                cky = ty;
                clyde_gx = clyde_tgx;
                clyde_gy = clyde_tgy; 
                clyde_moving = false;

                if (clyde_gx == player_gx && clyde_gy == player_gy && ghostMode == GhostMode::Chase) {
                    isDying = true; deathFrame = 2; deathClock.restart();
                    std::vector<sf::Texture>* f = (currentState == State::Right) ? &texRight : (currentState == State::Left) ? &texLeft : (currentState == State::Up) ? &texUp : &texDown;
                    pacman.setTexture((*f)[deathFrame]);
                }
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
                    pointShape.setPosition({
                        x * tileSize + offsetX + (tileSize / 2.f - pointShape.getRadius()), 
                        y * tileSize + offsetY + (tileSize / 2.f - pointShape.getRadius())
                    });
                    window->draw(pointShape);
                } else if (harita[y][x] == 8) {
                    powerPelletShape.setPosition({
                        x * tileSize + offsetX + (tileSize / 2.f - powerPelletShape.getRadius()),
                        y * tileSize + offsetY + (tileSize / 2.f - powerPelletShape.getRadius())
                    });
                    window->draw(powerPelletShape);
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
        for (int i = 0; i < lives; i++) {
            lifeIcon.setPosition({offsetX + i * (tileSize * lifeIconScale + 5.f), 940.f});
            window->draw(lifeIcon);
        }
        
        window->display();
    }

    delete window;
    return 0;
}