#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
using namespace sf;
const float ENEMY_SPAWN_INTERVAL = 1.5f; // 設定每 1.5 秒生成一個敵人

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Game CISC 4900"); // srceen siz
    window.setFramerateLimit(60); //FPS set to 60   

    Texture vampireTexture;
    if (!vampireTexture.loadFromFile("C:/Users/sam/source/repos/Project4/Project4/x64/Release/vampire-m-002.png")) {
        return -1; // 如果圖片載入失敗，則退出
    }
    // main character 
    Sprite vampireSprite(vampireTexture);
    vampireSprite.setPosition(Vector2f(800.f, 500.f)); // 設定初始位置
    vampireSprite.setTextureRect({ {0,0},{48,64} }); // 圖片大小
    vampireSprite.setScale({2.0f, 2.0f});//角色大小

    // 角色變數
    float speed = 300.f;
    float animationTimer = 0.f; // 控制動畫速度
    int frameIndex = 0; // 動畫幀索引
    int direction = 2; // 預設向下 (0=上, 1=左, 2=下, 3=右)
    Clock enemySpawnClock; // **敵人生成計時器**

    Clock clock;


    // **敵人設定**
    Texture enemyTexture;
    if (!enemyTexture.loadFromFile("C:/Users/sam/source/repos/Project4/Project4/x64/Release/enemy.png")) {
        std::cerr << "Error: Failed to load enemy texture!" << std::endl;
        return -1;
    }

    std::vector<Sprite> enemies; // 儲存所有敵人
    std::srand(static_cast<unsigned>(std::time(nullptr))); // 設定隨機數



    while (window.isOpen()) // check the window still open
    {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        //Update
        Vector2f movement(0.f, 0.f); // 初始化移動向量

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

        // 角色移動
        vampireSprite.move(movement * deltaTime);

        // **更新動畫**
        if (movement.x != 0 || movement.y != 0) { // 只有移動時才播放動畫
            animationTimer += deltaTime;
            if (animationTimer >= 0.15f) { // 每 0.15 秒更新一幀
                animationTimer = 0.f;
                frameIndex = (frameIndex + 1) % 3; // 循環 0 → 1 → 2 → 0
                vampireSprite.setTextureRect(sf::IntRect(sf::Vector2i(frameIndex * 48, direction * 64), sf::Vector2i(48, 64)));
            }
        }
        // **敵人生成機制**
        if (enemySpawnClock.getElapsedTime().asSeconds() >= ENEMY_SPAWN_INTERVAL) {
            enemySpawnClock.restart(); // 重置計時器

            Sprite enemySprite(enemyTexture);
            float randX = static_cast<float>(std::rand() % 1800 + 50); // 隨機 X 位置
            float randY = static_cast<float>(std::rand() % 1000 + 50); // 隨機 Y 位置
            enemySprite.setPosition(Vector2f(randX, randY));
            enemySprite.setScale(Vector2f(1.5f, 1.5f));
            enemies.push_back(enemySprite);

            std::cout << "New enemy spawned! Total enemies: " << enemies.size() << std::endl;
        }

        // **敵人追蹤吸血鬼**
        for (auto& enemy : enemies) {
            Vector2f enemyPos = enemy.getPosition();
            Vector2f vampirePos = vampireSprite.getPosition();
            Vector2f direction = vampirePos - enemyPos; // 計算方向向量

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); // 計算距離
            if (length > 0) {
                direction /= length; // 正規化向量
                enemy.move(direction * (100.f * deltaTime)); // 控制敵人速度
            }
        }
        //Draw
        window.clear();
        window.draw(vampireSprite);
        for (auto& enemy : enemies) {
            window.draw(enemy);
        }
        window.display();

    }
}