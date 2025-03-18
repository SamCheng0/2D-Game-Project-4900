#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint> // 為了使用 std::uint8_t

using namespace sf;
using namespace std;

const float ENEMY_SPAWN_INTERVAL = 0.5f; // 設定每 0.5 秒生成一個敵人
const int PLAYER_MAX_HEALTH = 100;       // 角色最大血量
const float DAMAGE_COOLDOWN = 1.0f;      // 敵人攻擊間隔
Clock attackClock;
const float attackCooldown = 0.5f;       // 攻擊冷卻時間（0.5秒）
const float attackRange = 200.f;         // 攻擊範圍半徑

// 粒子結構
struct Particle {
    CircleShape shape;    // 粒子的形狀
    Vector2f velocity;    // 移動速度
    float lifetime;       // 存活時間
};

// 敵人結構
struct Enemy {
    Sprite sprite;
    int health;

    // 添加構造函數，接受 Texture 並初始化 sprite
    Enemy(const Texture& texture) : sprite(texture), health(20) {}
};

class GameMenu {
private:
    RenderWindow menuWindow;
    Font font;
    Font healthFont;
    Font pausefont;
    vector<Text> menuItems;
    vector<Text> healthText;
    int selectedItemIndex = 0;
    Texture backgroundTexture;
    Music music;
    const vector<String> menuTexts = { "Play", "Options", "Exit" };

    // 粒子效果相關
    vector<Particle> attackParticles; // 儲存所有攻擊粒子
    vector<Enemy> enemies;            // 儲存所有敵人

public:
    float volume = 50.0f;
    GameMenu() : menuWindow(VideoMode({ 1920, 1080 }), "SFML Game Menu") {
        initializeMenu();
    }

    void initializeMenu() {
        if (!font.openFromFile("arial.ttf")) {
            cerr << "Failed to load font!" << endl;
            return;
        }

        if (!backgroundTexture.loadFromFile("castle-1920x1080.png")) {
            cerr << "Failed to load background image!" << endl;
            return;
        }
        backgroundTexture.setSmooth(true);

        if (!music.openFromFile("backgroundmusic.wav")) {
            cerr << "Failed to load background music!" << endl;
            return;
        }

        music.setLooping(true);
        music.setVolume(volume);
        music.play();

        float yPosition = 500.f;
        for (const auto& text : menuTexts) {
            Text menuItem(font);
            menuItem.setString(text);
            menuItem.setCharacterSize(50);
            menuItem.setFillColor(Color::White);

            FloatRect textRect = menuItem.getLocalBounds();
            float textWidth = 100.0f;
            float textHeight = 50.0f;

            menuItem.setOrigin(Vector2f(textWidth / 2.0f, textHeight / 2.0f));
            menuItem.setPosition(Vector2f(menuWindow.getSize().x / 2.0f, yPosition));

            yPosition += 80.f;
            menuItems.push_back(menuItem);
        }

        menuItems[selectedItemIndex].setFillColor(Color::Red);
    }

    void processInput() {
        while (optional event = menuWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                menuWindow.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == Keyboard::Scancode::Escape)
                    menuWindow.close();
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));
                    for (size_t i = 0; i < menuItems.size(); i++) {
                        if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                            selectedItemIndex = i;
                            handleSelection();
                            break;
                        }
                    }
                }
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseMoved>()) {
                Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));
                for (size_t i = 0; i < menuItems.size(); i++) {
                    if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                        for (auto& item : menuItems) {
                            item.setFillColor(Color::White);
                        }
                        menuItems[i].setFillColor(Color::Red);
                        break;
                    }
                }
            }
        }
    }

    void handleSelection() {
        switch (selectedItemIndex) {
        case 0:
            menuWindow.close();
            runGame();
            break;
        case 1:
            menuWindow.close();
            openOptionsWindow(volume);
            cout << "Option button is clicked!" << endl;
            break;
        case 2:
            menuWindow.close();
            break;
        }
    }

    void render() {
        menuWindow.clear(Color::Black);
        Sprite sprite(backgroundTexture);
        menuWindow.draw(sprite);

        for (const auto& item : menuItems) {
            menuWindow.draw(item);
        }

        menuWindow.display();
    }

    void run() {
        while (menuWindow.isOpen()) {
            processInput();
            render();
        }
    }

    void openOptionsWindow(float& volume) {
        sf::RenderWindow optionsWindow(VideoMode({ 1920, 1080 }), "Options");

        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "Error loading font!\n";
            return;
        }

        Text volumeText(font);
        volumeText.setCharacterSize(30);
        volumeText.setString("Volume: " + std::to_string(static_cast<int>(volume)));
        volumeText.setFillColor(Color::White);
        volumeText.setPosition(Vector2f(800.f, 500.f));

        while (optionsWindow.isOpen()) {
            while (optional event = optionsWindow.pollEvent()) {
                if (event->is<sf::Event::Closed>())
                    optionsWindow.close();

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == Keyboard::Scancode::D && volume < 100) {
                        volume += 5;
                    }
                    else if (keyPressed->scancode == Keyboard::Scancode::A && volume > 0) {
                        volume -= 5;
                    }
                    else if (keyPressed->scancode == Keyboard::Scancode::Escape) {
                        optionsWindow.close();
                        run();
                    }
                    volumeText.setString("Volume: " + std::to_string(static_cast<int>(volume)));
                    cout << "Volume set to: " << volume << "%\n";
                }
            }

            optionsWindow.clear();
            optionsWindow.draw(volumeText);
            optionsWindow.display();
        }
    }

    void runGame() {
        RenderWindow gameWindow(VideoMode({ 1920, 1080 }), "Game CISC 4900");
        gameWindow.setFramerateLimit(60);

        // 載入背景（）
        Texture gameBackgroundTexture;
        if (!gameBackgroundTexture.loadFromFile("map5.png")) {
            cerr << "Failed to load dungeon background texture!" << endl;
            return;
        }
        Sprite gameBackground(gameBackgroundTexture);
        gameBackground.setPosition(Vector2f(0.f, 0.f));

        Music gameplayMusic;
        if (!gameplayMusic.openFromFile("Dark Descent.wav")) {
            cerr << "Failed to load gameplay music!" << endl;
            return;
        }
        gameplayMusic.setLooping(true);
        gameplayMusic.setVolume(volume);
        gameplayMusic.play();

        bool isPaused = false;

        View view;
        view.setSize({ 1920.f, 1080.f });
        //view.zoom(0.5f);
        view.setCenter({ 800.f, 500.f });

        Texture vampireTexture;
        if (!vampireTexture.loadFromFile("vampire-m-002.png")) {
            cerr << "Failed to load vampire texture!" << endl;
            return;
        }

        Sprite vampireSprite(vampireTexture);
        vampireSprite.setPosition({1500.f, 1500.f });
        vampireSprite.setTextureRect({ {0, 0}, {48, 64} });
        vampireSprite.setScale({ 2.0f, 2.0f });

        float speed = 300.f;
        float animationTimer = 0.f;
        int frameIndex = 0;
        int direction = 2;
        Clock enemySpawnClock;

        Texture enemyTexture;
        if (!enemyTexture.loadFromFile("spellun-sprite.png")) {
            cerr << "Failed to load enemy texture!" << endl;
            return;
        }

        srand(static_cast<unsigned>(time(nullptr)));

        int playerHealth = PLAYER_MAX_HEALTH;
        Clock damageCooldownClock;

        if (!healthFont.openFromFile("arial.ttf")) {
            cerr << "Failed to load health font!" << endl;
            return;
        }

        Text healthText(healthFont);
        healthText.setCharacterSize(50);
        healthText.setFillColor(Color::Red);
        healthText.setPosition({ 40.f, 50.f });

        Clock clock;

        while (gameWindow.isOpen()) {
            music.stop();
            float deltaTime = clock.restart().asSeconds();

            while (optional event = gameWindow.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    gameWindow.close();
                }
                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == Keyboard::Scancode::Escape) {
                        isPaused = !isPaused;
                    }
                }
            }

            if (!isPaused) {
                Vector2f movement(0.f, 0.f);
                if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
                    movement.y = -speed;
                    direction = 0;
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                    movement.x = -speed;
                    direction = 3;
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
                    movement.y = speed;
                    direction = 2;
                }
                if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                    movement.x = speed;
                    direction = 1;
                }

                vampireSprite.move(movement * deltaTime);

                healthText.setPosition(Vector2f(vampireSprite.getPosition().x - 85.f, vampireSprite.getPosition().y - 70.f));
                view.setCenter(vampireSprite.getPosition());
                gameWindow.setView(view);

                if (movement.x != 0 || movement.y != 0) {
                    animationTimer += deltaTime;
                    if (animationTimer >= 0.15f) {
                        animationTimer = 0.f;
                        frameIndex = (frameIndex + 1) % 3;
                        vampireSprite.setTextureRect(sf::IntRect(sf::Vector2i(frameIndex * 48, direction * 64), sf::Vector2i(48, 64)));
                    }
                }

                // 攻擊邏輯與粒子生成
                if (Keyboard::isKeyPressed(Keyboard::Key::F) && attackClock.getElapsedTime().asSeconds() >= attackCooldown) {
                    attackClock.restart();
                    cout << "Attack!" << endl;

                    Vector2f spawnOffset(0.f, 0.f);
                    switch (direction) {  // direction: 0=上, 1=右, 2=下, 3=左
                    case 0: spawnOffset = Vector2f(0.f, -50.f); break;  // 上方偏移
                    case 1: spawnOffset = Vector2f(50.f, 0.f); break;   // 右方偏移
                    case 2: spawnOffset = Vector2f(0.f, 50.f); break;   // 下方偏移
                    case 3: spawnOffset = Vector2f(-50.f, 0.f); break;  // 左方偏移
                    }

                    for (int i = 0; i < 50; i++) {
                        Particle p;
                        p.shape = CircleShape(5.f);
                        p.shape.setFillColor(Color(255, 0, 0, 200));
                        // 將粒子生成位置設定為吸血鬼位置加上偏移量
                        p.shape.setPosition(vampireSprite.getPosition() + spawnOffset);
                        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
                        float speed = static_cast<float>(rand() % 300 + 100);
                        p.velocity = Vector2f(cosf(angle) * speed, sinf(angle) * speed);
                        p.lifetime = 0.5f;
                        attackParticles.push_back(p);
                    }
                }

                // 更新粒子並與敵人互動
                for (auto it = attackParticles.begin(); it != attackParticles.end();) {
                    it->shape.move(it->velocity * deltaTime);
                    it->lifetime -= deltaTime;
                    it->velocity *= 0.95f;

                    if (it->lifetime <= 0) {
                        it = attackParticles.erase(it);
                    }
                    else {
                        float lifetimeRatio = it->lifetime / 0.5f;
                        int alpha = static_cast<int>(lifetimeRatio * 200);
                        if (alpha > 255) alpha = 255;
                        if (alpha < 0) alpha = 0;
                        cout << "lifetime: " << it->lifetime << ", alpha: " << alpha << endl;
                        it->shape.setFillColor(Color(255, 0, 0, static_cast<std::uint8_t>(alpha)));

                        Vector2f particlePos = it->shape.getPosition();
                        for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
                            Vector2f enemyPos = enemyIt->sprite.getPosition();
                            float distance = sqrtf(powf(particlePos.x - enemyPos.x, 2) + powf(particlePos.y - enemyPos.y, 2));
                            if (distance <= 50.f) {
                                enemyIt->health -= 20;
                                cout << "Particle hit enemy! Enemy health: " << enemyIt->health << endl;
                                it = attackParticles.erase(it);
                                break;
                            }
                            else {
                                ++enemyIt;
                            }
                        }

                        if (it != attackParticles.end()) ++it;
                    }
                }

                // 刪除血量為 0 的敵人
                enemies.erase(remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                    return enemy.health <= 0;
                }), enemies.end());

                // 敵人生成機制
                if (enemySpawnClock.getElapsedTime().asSeconds() >= ENEMY_SPAWN_INTERVAL) {
                    enemySpawnClock.restart();

                    // 使用構造函數創建 Enemy 對象
                    Enemy newEnemy(enemyTexture);
                    float randX = static_cast<float>(std::rand() % 1800 + 50);
                    float randY = static_cast<float>(std::rand() % 1000 + 50);
                    newEnemy.sprite.setPosition(Vector2f(randX, randY));
                    newEnemy.sprite.setScale(Vector2f(1.5f, 1.5f));
                    enemies.push_back(newEnemy);

                    std::cout << "New enemy spawned! Total enemies: " << enemies.size() << std::endl;
                }

                // 敵人追蹤吸血鬼
                for (auto& enemy : enemies) {
                    Vector2f enemyPos = enemy.sprite.getPosition();
                    Vector2f vampirePos = vampireSprite.getPosition();
                    Vector2f dir = vampirePos - enemyPos;
                    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
                    if (length > 0) {
                        dir /= length;
                        enemy.sprite.move(dir * (100.f * deltaTime));
                    }

                    if (const std::optional<sf::FloatRect> intersection = enemy.sprite.getGlobalBounds().findIntersection(vampireSprite.getGlobalBounds())) {
                        if (damageCooldownClock.getElapsedTime().asSeconds() >= DAMAGE_COOLDOWN) {
                            playerHealth -= 10;
                            damageCooldownClock.restart();
                            cout << "Player health: " << playerHealth << endl;
                        }
                    }
                }

                if (playerHealth <= 0) {
                    gameOver();
                    gameWindow.close();
                }

                healthText.setString("Health: " + to_string(playerHealth));

                gameWindow.clear();
                gameWindow.draw(gameBackground);
                gameWindow.draw(vampireSprite);
                for (auto& enemy : enemies) {
                    gameWindow.draw(enemy.sprite);
                }
                for (auto& particle : attackParticles) {
                    gameWindow.draw(particle.shape);
                }
                gameWindow.draw(healthText);
                gameWindow.display();
                
            }
            if (isPaused) {
              
            }
        }

        gameplayMusic.stop();
    }

    void gameOver() {
    gameplayMusic.stop();
    RenderWindow gameOverWindow(sf::VideoMode({ 1920, 1080 }), "Game Over");
    if (!gameOverfont.openFromFile("arial.ttf")) {
        cerr << "Failed to load gameover font!" << endl;
        return;
    }

    Text gameOverText(gameOverfont);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(100);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({ 700, 400 });

    while (gameOverWindow.isOpen()) {

        while (optional event = gameOverWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                gameOverWindow.close();
            }
        }
       
        gameOverWindow.clear();
        gameOverWindow.draw(gameOverText);
        gameOverWindow.display();
    }
}

};

int main() {
    GameMenu gameMenu;
    gameMenu.run();

    return 0;
}
