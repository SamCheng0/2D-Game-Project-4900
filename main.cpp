/*
#include <SFML/Graphics.hpp>
#include "Test.h"
#include <SFML/Window.hpp>

using namespace sf;

void main() {
	
	RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Game Menu", sf::Style::Default);
	Test test(window.getSize().x, window.getSize().y);
	window.setFramerateLimit(60);

	while (window.isOpen()) {
		// Event handling
		while (std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					window.close();
			}
			
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
				if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
					test.MoveUp();
					break;
				}
				if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
					test.MoveDown();
					break;
				}
				if (keyPressed->scancode == sf::Keyboard::Scancode::Enter) {

					RenderWindow Play(VideoMode({ 960, 720 }), "game_name");
					RenderWindow OPTIONS(VideoMode({ 960, 720 }), "OPTIONS");
					RenderWindow ABOUT(VideoMode({ 960, 720 }), "ABOUT");

					int x = test.MainMenuPressed();

					if (x == 0)
					{
						while (Play.isOpen()) {
							while (std::optional aevent = window.pollEvent())
							{
								if (aevent->is<sf::Event::Closed>())
									Play.close();
								else if (const auto* keyPressed = aevent->getIf<sf::Event::KeyPressed>())
								{
									if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
										Play.close();
								}
							}

							OPTIONS.close();
							ABOUT.close();
							Play.clear();
							Play.display();
						}

					}

					if (x == 1)
					{
						while (OPTIONS.isOpen()) {
							while (std::optional aevent = window.pollEvent())
							{
								if (aevent->is<sf::Event::Closed>())
									OPTIONS.close();
								else if (const auto* keyPressed = aevent->getIf<sf::Event::KeyPressed>())
								{
									if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
										OPTIONS.close();
								}
							}

							Play.close();
							ABOUT.close();
							OPTIONS.clear();
							OPTIONS.display();
						}

					}
					if (x == 2)
					{
						while (ABOUT.isOpen()) {
							while (std::optional aevent = window.pollEvent())
							{
								if (aevent->is<sf::Event::Closed>())
									ABOUT.close();
								else if (const auto* keyPressed = aevent->getIf<sf::Event::KeyPressed>())
								{
									if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
										ABOUT.close();
								}
							}

							OPTIONS.close();
							Play.close();
							ABOUT.clear();
							ABOUT.display();

						}

					}

					if (x == 3)
						window.close();
						break;

				}

			}

		}

		window.clear();
		test.draw(window);
		window.display();
	}

}


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <SFML/Audio.hpp>



int main() {
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Game Menu", sf::Style::Default);
    window.setFramerateLimit(60);

    // Load Background Image

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("castle-1920x1080.png")) {
        std::cerr << "Failed to load background image!" << std::endl;
        return -1;
    }
    backgroundTexture.setSmooth(true);


    // Create a sprite to display the texture
    sf::Sprite backgroundSprite(backgroundTexture);

    // Load Font and Buttons 

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    // Play Button setup (unchanged)
    sf::RectangleShape playButton(sf::Vector2f(200, 50));
    playButton.setPosition({ 810.f, 540.f });
    playButton.setFillColor(sf::Color::White);

    // Initialize text with font and string
    sf::Text playText(font, "Start Game");
    playText.setCharacterSize(27);
    playText.setFillColor(sf::Color::Black);
    playText.setPosition({ 810.f, 540.f });


    // Exit Button
    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setPosition({ 810.f, 640.f });
    exitButton.setFillColor(sf::Color::White);

    // Initialize exit text with font and string
    sf::Text exitText(font, "Exit Game");
    exitText.setCharacterSize(27);
    exitText.setFillColor(sf::Color::Black);
    exitText.setPosition({ 810.f, 640.f });

    //Load background music, playing it and looping
    sf::Music music;
    if (!music.openFromFile("backgroundmusic.wav"))
        return -1;
    sf::Music::TimeSpan loopPoints(sf::seconds(0), music.getDuration()); // Loop the entire track
    music.setLoopPoints(loopPoints);
    music.setLooping(true);
    music.setVolume(50);
    music.play();

    while (window.isOpen()) {
        // Event handling
        while (std::optional event = window.pollEvent())
        {
           
        

            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }

            //exitButton interaction
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    // Check if mouse is inside the exitbutton
                    if (exitButton.getGlobalBounds().contains(mousePos)) {
                        window.close();  // Exit the game
                    }
                }
            }
            void handleGameEvents(sf::Event & event, sf::RenderWindow & window); {
                if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                    {
                        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                        // Check if mouse is inside the exitbutton
                        if (playButton.getGlobalBounds().contains(mousePos)) {
                            currentState = GameState::GAME; // Switch to game state
                            music.stop();
                            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
                            {
                                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                                    currentState = GameState::MENU;
                            }

                        }
                    }
                }
            }

        }
        switch (currentState) {
        case GameState::MENU:
            window.clear();
            drawMenu(window, backgroundSprite, playButton, playText, exitButton, exitText);
            window.display();
            break;

        case GameState::GAME:
            window.clear();
            drawGame(window);
            window.display();
            break;
        }


        // Clear and draw
        window.clear(sf::Color::Black);

        // Draw background FIRST
        window.draw(backgroundSprite);

        // Then draw buttons/text
        window.draw(playButton);
        window.draw(playText);
        window.draw(exitButton);
        window.draw(exitText);

        window.display();
    }

    return 0;
}
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;


const float ENEMY_SPAWN_INTERVAL = 1.5f; // 設定每 1.5 秒生成一個敵人
const int PLAYER_MAX_HEALTH = 100;      // 角色最大血量
const float DAMAGE_COOLDOWN = 1.0f;     // 敌人攻击间隔
Clock attackClock;
const float attackCooldown = 0.5f; // 攻擊冷卻時間（0.5秒）
const float attackRange = 200.f; // 攻擊範圍半徑

//主菜单
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
    
    

public:
    float volume = 50.0f;
    GameMenu() : menuWindow(VideoMode({ 1920, 1080 }), "SFML Game Menu") {
        initializeMenu();
    }

    void initializeMenu() {
        //加载字体
        if (!font.openFromFile("arial.ttf")) {
            cerr << "Failed to load font!" << endl;
            return;
        }

        //加载背景图片
        if (!backgroundTexture.loadFromFile("castle-1920x1080.png")) {
            cerr << "Failed to load background image!" << endl;
            return;
        }
        backgroundTexture.setSmooth(true);

        //加载背景音乐
        if (!music.openFromFile("backgroundmusic.wav")) {
            cerr << "Failed to load background music!" << endl;
            return;
        }

        //循环播放背景音乐
        //Music::TimeSpan loopPoints(seconds(0), music.getDuration()); //不用这段也可以，这段用来设置截取音乐其中一段来循环
        //music.setLoopPoints(loopPoints);
        music.setLooping(true);
        music.setVolume(volume);
        music.play();

        //创建选项
        float yPosition = 500.f;
        for (const auto& text : menuTexts) {
            Text menuItem(font);
            menuItem.setString(text);
            menuItem.setCharacterSize(50);
            menuItem.setFillColor(Color::White);

            //选项按键位置
            FloatRect textRect = menuItem.getLocalBounds();
            float textWidth = 100.0f; //宽度
            float textHeight = 50.0f; //高度

            menuItem.setOrigin(Vector2f(textWidth / 2.0f, textHeight / 2.0f));
            menuItem.setPosition(Vector2f(menuWindow.getSize().x / 2.0f, yPosition));

            yPosition += 80.f; // Spacing between items
            menuItems.push_back(menuItem);
        }

        //预设按键颜色（红色）
        menuItems[selectedItemIndex].setFillColor(Color::Red);
    }

    void processInput() {
        while (optional event = menuWindow.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                menuWindow.close();
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == Keyboard::Scancode::Escape)
                    menuWindow.close();
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    //判断鼠标是否在按键上面
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
                //当鼠标移动到上面时，高亮选项按键
                Vector2f mousePos = menuWindow.mapPixelToCoords(Mouse::getPosition(menuWindow));
                for (size_t i = 0; i < menuItems.size(); i++) {
                    if (menuItems[i].getGlobalBounds().contains(mousePos)) {
                        //重置其他选项颜色
                        for (auto& item : menuItems) {
                            item.setFillColor(Color::White);
                        }
                        //高亮此按键
                        menuItems[i].setFillColor(Color::Red);
                        break;
                    }
                }
            }
        }
    }

    void handleSelection() {
        switch (selectedItemIndex) {
        case 0: // Play
            menuWindow.close(); 
            runGame();
            break;
        case 1: // Options
            menuWindow.close();
            openOptionsWindow(volume);
            cout << "Option button is clicked!";
            break;
        case 2: // Exit
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

                // Adjust volume using left and right arrow keys
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
                    cout << "Volume set to: " << volume << "%\n"; // Debugging output
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

        Music gameplayMusic;
        if (!gameplayMusic.openFromFile("Dark Descent.wav")) {
            cerr << "Failed to load gameplay music!" << endl;
            return;
        }
        gameplayMusic.setLooping(true);
        gameplayMusic.setVolume(volume);
        gameplayMusic.play();

        // Pause Menu Variables
        bool isPaused = false;
       

        View view;
        view.setSize({ 1920.f, 1080.f }); // Set the size of the view (same as window size)
        view.setCenter({ 800.f, 500.f }); // Initial center (same as character's initial position)
        //加载角色
        Texture vampireTexture;
        if (!vampireTexture.loadFromFile("vampire-m-002.png")) {
            cerr << "Failed to load vampire texture!" << endl; // 如果圖片載入失敗，則退出
            return;
        }

        //角色设置
        Sprite vampireSprite(vampireTexture);
        vampireSprite.setPosition({ 800.f, 500.f });
        vampireSprite.setTextureRect({ {0, 0}, {48, 64} });
        vampireSprite.setScale({ 2.0f, 2.0f });

        // 角色變數
        float speed = 300.f;
        float animationTimer = 0.f;// 控制動畫速度
        int frameIndex = 0;// 動畫幀索引
        int direction = 2; // 預設向下 (0=上, 1=左, 2=下, 3=右)
        Clock enemySpawnClock; // **敵人生成計時器**


        // **敵人設定**
        Texture enemyTexture;
        if (!enemyTexture.loadFromFile("spellun-sprite.png")) {
            cerr << "Failed to load enemy texture!" << endl;
            return;
        }

        vector<Sprite> enemies;// 儲存所有敵人
        srand(static_cast<unsigned>(time(nullptr)));// 設定隨機數

        // 血量系统
        int playerHealth = PLAYER_MAX_HEALTH;
        Clock damageCooldownClock; // 攻击冷却
        
        if (!healthFont.openFromFile("arial.ttf")) {
            cerr << "Failed to load health font!" << endl;
            return;
        }

        Text healthText(healthFont);
        healthText.setCharacterSize(50);
        healthText.setFillColor(Color::Red);
        healthText.setPosition({ 50.f, 50.f });

        


        //游戏中
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
                        isPaused = !isPaused; // Toggle pause
                    }
                }
            }

            if (!isPaused) {
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

                vampireSprite.move(movement * deltaTime);

                //血条显示在主角头上
                healthText.setPosition(Vector2f(vampireSprite.getPosition().x - 30.f, vampireSprite.getPosition().y - 70.f));

                //更新镜头跟随
                view.setCenter(vampireSprite.getPosition());
                gameWindow.setView(view);

                // **更新動畫**
                if (movement.x != 0 || movement.y != 0) { // 只有移動時才播放動畫
                    animationTimer += deltaTime;
                    if (animationTimer >= 0.15f) { // 每 0.15 秒更新一幀
                        animationTimer = 0.f;
                        frameIndex = (frameIndex + 1) % 3; // 循環 0 → 1 → 2 → 0
                        vampireSprite.setTextureRect(sf::IntRect(sf::Vector2i(frameIndex * 48, direction * 64), sf::Vector2i(48, 64)));
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Key::F) && attackClock.getElapsedTime().asSeconds() >= attackCooldown) {
                    attackClock.restart(); // 重置攻擊冷卻
                    cout << "Attack!" << endl;

                    // 遍歷敵人，判定是否在攻擊範圍內
                    enemies.erase(remove_if(enemies.begin(), enemies.end(), [&](Sprite& enemy) {
                        Vector2f enemyPos = enemy.getPosition();
                        Vector2f vampirePos = vampireSprite.getPosition();
                        float distance = sqrt(pow(enemyPos.x - vampirePos.x, 2) + pow(enemyPos.y - vampirePos.y, 2));

                        if (distance <= attackRange) {
                            cout << "Enemy hit!" << endl;
                            return true; // 刪除敵人
                        }
                        return false;
                        }), enemies.end());
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
                    Vector2f dir = vampirePos - enemyPos;
                    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (length > 0) {
                        dir /= length;
                        enemy.move(dir * (100.f * deltaTime));
                    }

                    // 判断敌人是否碰到吸血鬼
                    if (const std::optional<sf::FloatRect> intersection = enemy.getGlobalBounds().findIntersection(vampireSprite.getGlobalBounds())) {
                        if (damageCooldownClock.getElapsedTime().asSeconds() >= DAMAGE_COOLDOWN) {
                            playerHealth -= 10; //碰到一下减10滴血
                            damageCooldownClock.restart(); //重置冷却
                            cout << "Player health: " << playerHealth << endl;
                        }
                    }
                }

                //判断角色是否死亡
                if (playerHealth <= 0) {
                    cout << "Game Over!" << endl;
                    gameWindow.close();
                }

                //显示血条
                healthText.setString("Health: " + to_string(playerHealth));

                // Render
                gameWindow.clear();
                gameWindow.draw(vampireSprite);
                for (auto& enemy : enemies) {
                    gameWindow.draw(enemy);
                }
                gameWindow.draw(healthText);
                gameWindow.display();


          
            }
            if (isPaused) {

                Text pauseText(pausefont);
                pauseText.setString("Pause");
                pauseText.setCharacterSize(50);
                pauseText.setFillColor(Color::White);
                pauseText.setPosition(Vector2f(800.f, 450.f));

                Text continueText(pausefont);
                continueText.setCharacterSize(40);
                continueText.setString("Continue");
                continueText.setPosition(Vector2f(800.f, 500.f));
                continueText.setFillColor(Color::White);

                Text exitToMenuText(pausefont);
                exitToMenuText.setCharacterSize(40);
                exitToMenuText.setString("Exit to Menu");
                exitToMenuText.setPosition(Vector2f(800.f, 550.f));
                exitToMenuText.setFillColor(Color::White);

                if (!pausefont.openFromFile("arial.ttf")) {
                    std::cerr << "Error: Could not load pausefont!\n";
                    return;
                }

                // Draw Pause Menu
                gameWindow.draw(pauseText);
                gameWindow.draw(continueText);
                gameWindow.draw(exitToMenuText);
            }
        }

        gameplayMusic.stop();
    }
};


int main() {
    GameMenu gameMenu;
    gameMenu.run(); // Start with the game menu

    return 0;
}
