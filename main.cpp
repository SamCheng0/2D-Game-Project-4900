#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint> 
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

using namespace sf;
using namespace std;

const float ENEMY_SPAWN_INTERVAL = 0.5f; // 設定每 0.5 秒生成一個敵人
const int PLAYER_MAX_HEALTH = 100;       // 角色最大血量
const float DAMAGE_COOLDOWN = 1.0f;      // 敵人攻擊間隔
const int HEALTH_PER_KILL = 5;           // 每次擊殺敵人回復的血量
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

    Enemy(const Texture& texture) : sprite(texture), health(20) {}
};

class GameMenu {
private:
    RenderWindow menuWindow;
    Font font;
    vector<Text> menuItems;
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
            cerr << "Failed to load font! Ensure arial.ttf is in the project directory." << endl;
            return;
        }

        if (!backgroundTexture.loadFromFile("castle-1920x1080.png")) {
            cerr << "Failed to load background image! Ensure castle-1920x1080.png is in the project directory." << endl;
            return;
        }
        backgroundTexture.setSmooth(true);

        if (!music.openFromFile("backgroundmusic.wav")) {
            cerr << "Failed to load background music! Ensure backgroundmusic.wav is in the project directory." << endl;
            return;
        }

        music.setLooping(true);
        music.setVolume(volume);
        music.play();

        float yPosition = 500.f;
        for (const auto& text : menuTexts) {
            Text menuItem(font);
            menuItem.setString(text);
            menuItem.setCharacterSize(60);
            menuItem.setFillColor(Color(255, 215, 0));
            menuItem.setOutlineColor(Color::White);
            menuItem.setOutlineThickness(2.0f);

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

    void stopMusic() {
        for (float vol = volume; vol > 0; vol -= 5.0f) {
            music.setVolume(vol);
            sf::sleep(sf::milliseconds(50));
        }
        music.stop();
        music.setVolume(volume);
    }

    void processInput() {
        while (optional event = menuWindow.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                menuWindow.close();
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
            stopMusic();
            // 移除 menuWindow.close()
            runGame();
            break;
        case 1:
            stopMusic();
            // 移除 menuWindow.close()
            openOptionsWindow();
            cout << "Option button is clicked!" << endl;
            break;
        case 2:
            stopMusic();
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
        music.setVolume(volume);
        music.play();
        while (menuWindow.isOpen()) {
            processInput();
            render();
        }
    }

    void openOptionsWindow() {
        sf::RenderWindow optionsWindow(VideoMode({ 1920, 1080 }), "Options");

        if (!font.openFromFile("arial.ttf")) {
            std::cerr << "Error loading font! Ensure arial.ttf is in the project directory.\n";
            return;
        }

        Text volumeText(font);
        volumeText.setCharacterSize(30);
        volumeText.setString("Volume: " + std::to_string(static_cast<int>(volume)));
        volumeText.setFillColor(Color::White);
        volumeText.setPosition(Vector2f(800.f, 500.f));

        RectangleShape volumeBarBackground(Vector2f(300.f, 20.f));
        volumeBarBackground.setFillColor(Color(128, 128, 128)); //gray
        volumeBarBackground.setPosition(Vector2f(800.f, 550.f));

        RectangleShape volumeBar(Vector2f(300.f * (volume / 100.f), 20.f));
        volumeBar.setFillColor(Color::Green);
        volumeBar.setPosition(Vector2f(800.f, 550.f));

        music.setVolume(volume);
        music.play();

        while (optionsWindow.isOpen()) {
            while (optional event = optionsWindow.pollEvent()) {
                // 移除直接關閉窗口的邏輯，避免與 Esc 鍵衝突
                // if (event->is<sf::Event::Closed>())
                //     optionsWindow.close();

                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == Keyboard::Scancode::D && volume < 100) {
                        volume += 5;
                        music.setVolume(volume);
                    }
                    else if (keyPressed->scancode == Keyboard::Scancode::A && volume > 0) {
                        volume -= 5;
                        music.setVolume(volume);
                    }
                    else if (keyPressed->scancode == Keyboard::Scancode::Escape) {
                        optionsWindow.close();
                        // 停止選項畫面的音樂，避免與主選單音樂重疊
                        music.stop();
                        run();
                        return; // 確保在關閉窗口後退出迴圈
                    }
                    volumeText.setString("Volume: " + std::to_string(static_cast<int>(volume)));
                    volumeBar.setSize(Vector2f(300.f * (volume / 100.f), 20.f));
                    cout << "Volume set to: " << volume << "%\n";
                }
            }

            optionsWindow.clear();
            optionsWindow.draw(volumeText);
            optionsWindow.draw(volumeBarBackground);
            optionsWindow.draw(volumeBar);
            optionsWindow.display();
        }
    }

    void runGame() {
        RenderWindow gameWindow(VideoMode({ 1920, 1080 }), "Game CISC 4900");
        gameWindow.setFramerateLimit(60);

        Texture gameBackgroundTexture;
        if (!gameBackgroundTexture.loadFromFile("map.png")) {
            cerr << "Failed to load dungeon background texture! Ensure map.png is in the project directory." << endl;
            return;
        }
        Sprite gameBackground(gameBackgroundTexture);
        gameBackground.setPosition(Vector2f(0.f, 0.f));

        Music gameplayMusic;
        if (!gameplayMusic.openFromFile("Dark Descent.wav")) {
            cerr << "Failed to load gameplay music! Ensure Dark Descent.wav is in the project directory." << endl;
            return;
        }
        gameplayMusic.setLooping(true);
        gameplayMusic.setVolume(volume);
        gameplayMusic.play();

        bool isPaused = false;
        bool isGameOver = false;

        View view;
        view.setSize({ 1920.f, 1080.f });
        view.setCenter({ 800.f, 500.f });

        Texture vampireTexture;
        if (!vampireTexture.loadFromFile("vampire-m-002.png")) {
            cerr << "Failed to load vampire texture! Ensure vampire-m-002.png is in the project directory." << endl;
            return;
        }

        Sprite vampireSprite(vampireTexture);
        vampireSprite.setPosition({ 3500.f, 1500.f });
        vampireSprite.setTextureRect({ {0, 0}, {48, 64} });
        vampireSprite.setScale({ 2.0f, 2.0f });

        float speed = 300.f;
        float animationTimer = 0.f;
        int frameIndex = 0;
        int direction = 2;
        Clock enemySpawnClock;

        Texture enemyTexture;
        if (!enemyTexture.loadFromFile("spellun-sprite.png")) {
            cerr << "Failed to load enemy texture! Ensure spellun-sprite.png is in the project directory." << endl;
            return;
        }

        srand(static_cast<unsigned>(time(nullptr)));

        int playerHealth = PLAYER_MAX_HEALTH;
        Clock damageCooldownClock;

        Font uiFont;
        if (!uiFont.openFromFile("arial.ttf")) {
            cerr << "Failed to load UI font! Ensure arial.ttf is in the project directory." << endl;
            return;
        }

        Text healthText(uiFont);
        healthText.setCharacterSize(50);
        healthText.setFillColor(Color::Red);
        healthText.setOutlineColor(Color::White);
        healthText.setOutlineThickness(2.0f);
        healthText.setPosition({ 40.f, 50.f });

        Text killText(uiFont);
        killText.setCharacterSize(50);
        killText.setFillColor(Color::Green);
        killText.setOutlineColor(Color::White);
        killText.setOutlineThickness(2.0f);
        killText.setPosition({ 10.f, 10.f });
        int killCount = 0;

        Text pauseText(uiFont);
        pauseText.setString("Pause");
        pauseText.setCharacterSize(60);
        pauseText.setFillColor(Color(255, 215, 0));
        pauseText.setOutlineColor(Color::White);
        pauseText.setOutlineThickness(2.0f);
        pauseText.setPosition(Vector2f(800.f, 400.f));

        Text continueText(uiFont);
        continueText.setCharacterSize(40);
        continueText.setString("Continue");
        continueText.setFillColor(Color::Blue);
        continueText.setOutlineColor(Color::White);
        continueText.setOutlineThickness(2.5f);
        continueText.setPosition(Vector2f(800.f, 550.f));

        Text exitToMenuText(uiFont);
        exitToMenuText.setCharacterSize(40);
        exitToMenuText.setString("Exit to Menu");
        exitToMenuText.setFillColor(Color::Red);
        exitToMenuText.setOutlineColor(Color::White);
        exitToMenuText.setOutlineThickness(2.5f);
        exitToMenuText.setPosition(Vector2f(800.f, 600.f));

        RectangleShape overlay(Vector2f(1920.f, 1080.f));
        overlay.setFillColor(Color(0, 0, 0, 150));

        Text gameOverText(uiFont);
        gameOverText.setString("Game Over");
        gameOverText.setCharacterSize(80);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setOutlineColor(Color::White);
        gameOverText.setOutlineThickness(2.5f);
        gameOverText.setPosition(Vector2f(700.f, 500.f));

        Text returnPrompt(uiFont);
        returnPrompt.setString("Press Enter to Return to Menu");
        returnPrompt.setCharacterSize(40);
        returnPrompt.setFillColor(Color::White);
        returnPrompt.setOutlineColor(Color::White);
        returnPrompt.setOutlineThickness(2.0f);
        returnPrompt.setPosition(Vector2f(650.f, 600.f));

        Clock gameOverTimer;

        Clock clock;
        float survivalTimer = 0.f;
        bool enemyKilledRecently = false;
        Clock lastKillClock;
        const float killResetTime = 5.f;

        bool showHealText = false;
        Clock healTextTimer;
        Text healText(uiFont);
        Vector2f healTextPosition;

        while (gameWindow.isOpen()) {
            float deltaTime = clock.restart().asSeconds();

            if (lastKillClock.getElapsedTime().asSeconds() >= killResetTime) {
                enemyKilledRecently = false;
            }

            while (optional event = gameWindow.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    gameWindow.close();
                }
                else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == Keyboard::Scancode::Escape && !isGameOver) {
                        isPaused = !isPaused;
                    }
                    if (isGameOver && keyPressed->scancode == Keyboard::Scancode::Enter) {
                        gameplayMusic.stop();
                        gameWindow.close();
                        run();
                    }
                }
                else if (isPaused) {
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                            Vector2f mousePos = gameWindow.mapPixelToCoords(Mouse::getPosition(gameWindow));
                            if (continueText.getGlobalBounds().contains(mousePos)) {
                                isPaused = false;
                            }
                            if (exitToMenuText.getGlobalBounds().contains(mousePos)) {
                                gameplayMusic.stop();
                                gameWindow.close();
                                run();
                            }
                        }
                    }
                }
                else if (isGameOver) {
                    if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                        if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                            gameplayMusic.stop();
                            gameWindow.close();
                            run();
                        }
                    }
                }
            }

            if (!isPaused && !isGameOver) {
                survivalTimer += deltaTime;
                if (survivalTimer >= 1.f) {
                    survivalTimer = 0.f;
                    if (!enemyKilledRecently) {
                        playerHealth -= 1;
                        cout << "No enemies killed! Player health: " << playerHealth << endl;
                    }
                }

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

                if (Keyboard::isKeyPressed(Keyboard::Key::F) && attackClock.getElapsedTime().asSeconds() >= attackCooldown) {
                    attackClock.restart();
                    cout << "Attack!" << endl;

                    Vector2f spawnOffset(0.f, 0.f);
                    switch (direction) {
                    case 0: spawnOffset = Vector2f(0.f, -50.f); break;
                    case 1: spawnOffset = Vector2f(50.f, 0.f); break;
                    case 2: spawnOffset = Vector2f(0.f, 50.f); break;
                    case 3: spawnOffset = Vector2f(-50.f, 0.f); break;
                    }

                    for (int i = 0; i < 50; i++) {
                        Particle p;
                        p.shape = CircleShape(5.f);
                        p.shape.setFillColor(Color(255, 0, 0, 200));
                        p.shape.setPosition(vampireSprite.getPosition() + spawnOffset);
                        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
                        float speed = static_cast<float>(rand() % 300 + 100);
                        p.velocity = Vector2f(cosf(angle) * speed, sinf(angle) * speed);
                        p.lifetime = 0.5f;
                        attackParticles.push_back(p);
                    }
                }

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

                size_t initialEnemyCount = enemies.size();
                enemies.erase(remove_if(enemies.begin(), enemies.end(), [](const Enemy& enemy) {
                    return enemy.health <= 0;
                    }), enemies.end());
                if (enemies.size() < initialEnemyCount) {
                    int enemiesKilled = initialEnemyCount - enemies.size();
                    killCount += enemiesKilled;
                    int healthBefore = playerHealth;
                    playerHealth += enemiesKilled * HEALTH_PER_KILL;
                    if (playerHealth > PLAYER_MAX_HEALTH) {
                        playerHealth = PLAYER_MAX_HEALTH;
                    }
                    int healthGained = playerHealth - healthBefore;
                    enemyKilledRecently = true;
                    lastKillClock.restart();
                    cout << "Enemy killed! Total kills: " << killCount << ", Player healed! Health: " << playerHealth << endl;

                    if (healthGained > 0) {
                        healText.setString("+" + to_string(healthGained));
                        healText.setCharacterSize(50);
                        healText.setFillColor(Color::Green);
                        healText.setOutlineColor(Color::White);
                        healText.setOutlineThickness(2.5f);
                        healTextPosition = vampireSprite.getPosition();
                        healText.setPosition(healTextPosition);
                        showHealText = true;
                        healTextTimer.restart();
                    }
                }

                if (enemySpawnClock.getElapsedTime().asSeconds() >= ENEMY_SPAWN_INTERVAL) {
                    enemySpawnClock.restart();
                    Enemy newEnemy(enemyTexture);
                    float randX = static_cast<float>(std::rand() % 1800 + 50);
                    float randY = static_cast<float>(std::rand() % 1000 + 50);
                    newEnemy.sprite.setPosition(Vector2f(randX, randY));
                    newEnemy.sprite.setScale(Vector2f(1.5f, 1.5f));
                    enemies.push_back(newEnemy);
                    std::cout << "New enemy spawned! Total enemies: " << enemies.size() << std::endl;
                }

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

                if (showHealText) {
                    if (healTextTimer.getElapsedTime().asSeconds() >= 1.0f) {
                        showHealText = false;
                    }
                    else {
                        healTextPosition.y -= 30.0f * deltaTime;
                        healText.setPosition(healTextPosition);
                    }
                }

                if (playerHealth <= 0) {
                    isGameOver = true;
                    gameOverTimer.restart();
                    cout << "Game Over!" << endl;
                }

                healthText.setString("Health: " + to_string(playerHealth));
                killText.setString("Kills: " + to_string(killCount));
            }

            gameWindow.clear();
            gameWindow.setView(view);
            gameWindow.draw(gameBackground);
            gameWindow.draw(vampireSprite);
            for (auto& enemy : enemies) {
                gameWindow.draw(enemy.sprite);
            }
            for (auto& particle : attackParticles) {
                gameWindow.draw(particle.shape);
            }
            if (showHealText) {
                gameWindow.draw(healText);
            }
            gameWindow.draw(healthText);

            gameWindow.setView(gameWindow.getDefaultView());
            gameWindow.draw(killText);

            if (isPaused) {
                gameWindow.draw(pauseText);
                gameWindow.draw(continueText);
                gameWindow.draw(exitToMenuText);
            }
            else if (isGameOver) {
                static float alpha = 0.f;
                alpha += deltaTime * 255.f;
                if (alpha > 255.f) alpha = 255.f;
                gameOverText.setFillColor(Color(255, 0, 0, static_cast<std::uint8_t>(alpha)));
                returnPrompt.setFillColor(Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));

                gameWindow.setView(gameWindow.getDefaultView());
                gameWindow.draw(overlay);
                gameWindow.draw(gameOverText);
                gameWindow.draw(returnPrompt);

                if (gameOverTimer.getElapsedTime().asSeconds() >= 5.f) {
                    gameplayMusic.stop();
                    gameWindow.close();
                    run();
                }
            }

            gameWindow.display();
        }

        gameplayMusic.stop();
    }
};

int main() {
    GameMenu gameMenu;
    gameMenu.run();

    return 0;
}