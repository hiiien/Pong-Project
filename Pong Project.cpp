#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

class Paddle {
private:
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::RectangleShape rect;
    bool moveUpFlag;
    bool moveDownFlag;
    bool withinBounds;
    int paddleHeight;
    int score;

public:
    Paddle(float pos_x, float pos_y) : moveUpFlag(false), moveDownFlag(false), paddleHeight(80), score(0)
    {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = 0.0f;
        vel.y = 500.0f; // Increased paddle speed

        rect.setPosition(pos);
        rect.setFillColor(sf::Color::White);
        rect.setSize(sf::Vector2f(10, paddleHeight));
    }

    void render(sf::RenderWindow& wind) {
        rect.setPosition(pos);
        wind.draw(rect);
    }

    void moveUp() {
        moveUpFlag = true;
        moveDownFlag = false;
    }

    void moveDown() {
        moveUpFlag = false;
        moveDownFlag = true;
    }

    void stop() {
        moveUpFlag = false;
        moveDownFlag = false;
    }

    float getYPosition() const {
        return pos.y;
    }

    float getXPosition() const {
        return pos.x;
    }

    bool isWithinBounds() const {
        return withinBounds;
    }

    int getScore() {
        return score;
    }

    void increaseScore() {
        score++;
    }

    void update(float deltaTime) {
        if (moveUpFlag && pos.y > 0) {
            pos.y -= vel.y * deltaTime;
        }
        else if (moveDownFlag && pos.y < 720 - paddleHeight) {
            pos.y += vel.y * deltaTime;
        }

        withinBounds = (pos.y >= 0 && pos.y <= 720 - paddleHeight);
    }

    int getPaddleHeight() const {
        return paddleHeight;
    }

    void setPaddleHeight(int height) {
        paddleHeight = height;
        rect.setSize(sf::Vector2f(10, paddleHeight));
    }
    void resetPaddlePosition(float initialX, float initialY) {
        pos.x = initialX;
        pos.y = initialY;
    }
};

class Ball {
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;
    bool collidedThisFrame;

public:
    Ball(float pos_x, float pos_y, float vel_x, float vel_y) {
        pos.x = pos_x;
        pos.y = pos_y;

        vel.x = vel_x * 10.0f; // Increased initial ball speed
        vel.y = vel_y * 10.0f;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(5);

        collidedThisFrame = false;
    }

    void render(sf::RenderWindow& wind) {
        s.setPosition(pos);
        wind.draw(s);
    }

    float getXposition() {
        return pos.x;
    }

    float getYposition() {
        return pos.y;
    }

    void resetBallPosition() {
        pos.x = 640;
        pos.y = 360;
        vel.x = 500.0f; // Increased ball reset speed
        vel.y = 0.0f;
    }

    void updateVelocityY(float newVelY) {
        vel.y *= newVelY;
    }

    void updatePosition(const Paddle& paddle1, const Paddle& paddle2, int halfPaddleHeight, float invHalfPaddleHeight, float bounceAngleFactor, float speedFactor, float maxSpeedFactor, float deltaTime) {
        if (collidedThisFrame) {
            collidedThisFrame = false;
            return;
        }

        pos.x += vel.x * deltaTime;
        pos.y += vel.y * deltaTime;

        if ((pos.x - 5) <= (paddle1.getXPosition() + 10) && (pos.y + 5 >= paddle1.getYPosition() && pos.y - 5 <= paddle1.getYPosition() + paddle1.getPaddleHeight())) {
            float relativeIntersectY = (paddle1.getYPosition() + halfPaddleHeight) - pos.y;
            float normalizedRelativeIntersectY = relativeIntersectY * invHalfPaddleHeight;
            float bounceAngle = normalizedRelativeIntersectY * (bounceAngleFactor);
            vel.x = std::cos(bounceAngle) * speedFactor;
            vel.y = std::sin(bounceAngle) * -speedFactor;
            collidedThisFrame = true;

            speedFactor += 1500.0f * deltaTime; // Increased speed factor increment
            if (speedFactor > maxSpeedFactor) {
                speedFactor = maxSpeedFactor;
            }
        }

        if ((pos.x + 5) >= (paddle2.getXPosition()) && (pos.y + 5 >= paddle2.getYPosition() && pos.y - 5 <= paddle2.getYPosition() + paddle2.getPaddleHeight())) {
            float relativeIntersectY = (paddle2.getYPosition() + halfPaddleHeight) - pos.y;
            float normalizedRelativeIntersectY = relativeIntersectY * invHalfPaddleHeight;
            float bounceAngle = normalizedRelativeIntersectY * (bounceAngleFactor);
            vel.x = std::cos(bounceAngle) * -speedFactor;
            vel.y = std::sin(bounceAngle) * -speedFactor;
            collidedThisFrame = true;

            speedFactor += 1500.0f * deltaTime; // Increased speed factor increment
            if (speedFactor > maxSpeedFactor) {
                speedFactor = maxSpeedFactor;
            }
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "My Program");
    window.setFramerateLimit(60);

    std::vector<Paddle> paddles;
    std::vector<Ball> balls;

    paddles.push_back(Paddle(50, 360));
    paddles.push_back(Paddle(1230, 360));

    Ball ball(640, 360, 250.0f, 0.0f); // Adjusted initial ball speed values
    balls.push_back(ball);

    int halfPaddleHeight = paddles.at(0).getPaddleHeight() / 2;
    float invHalfPaddleHeight = 1.0f / halfPaddleHeight;
    float bounceAngleFactor = 3.14159 / 4;

    float maxSpeedFactor = 2500.0f; // Increased max speed factor
    float speedFactor = 1000.0f;    // Increased initial speed factor

    sf::Font oswald;
    oswald.loadFromFile("C:\\Users\\gavin\\OneDrive\\Desktop\\C++ L\\SFML Fonts\\Oswald-Bold.ttf");
    sf::Text scoreText;
    scoreText.setFont(oswald);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(540, 10);

    bool playerOneWon = false;
    bool playerTwoWon = false;
    sf::Clock clock;
    float deltaTime = 0.0f;

    while (window.isOpen() && (!playerOneWon && !playerTwoWon)) {
        sf::Time elapsed = clock.restart();
        deltaTime = elapsed.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W) paddles.at(0).moveUp();
                if (event.key.code == sf::Keyboard::S) paddles.at(0).moveDown();

                if (event.key.code == sf::Keyboard::Up) paddles.at(1).moveUp();
                if (event.key.code == sf::Keyboard::Down) paddles.at(1).moveDown();
            }

            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S) paddles.at(0).stop();
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down) paddles.at(1).stop();
            }
        }

        if (paddles.at(0).getScore() >= 11) {
            playerOneWon = true;
            window.close();
        }
        else if (paddles.at(1).getScore() >= 11) {
            playerTwoWon = true;
            window.close();
        }

        for (int i = 0; i < paddles.size(); i++) {
            paddles[i].update(deltaTime);
        }

        for (int i = 0; i < balls.size(); i++) {
            balls[i].updatePosition(paddles[0], paddles[1], halfPaddleHeight, invHalfPaddleHeight, bounceAngleFactor, speedFactor, maxSpeedFactor, deltaTime);
            if (balls[i].getYposition() > 720 - 5) {
                balls[i].updateVelocityY(-1.0f);
            }
            if (balls[i].getYposition() < 0) {
                balls[i].updateVelocityY(-1.0f);
            }

            if (balls[i].getXposition() <= 0) {
                paddles.at(0).increaseScore();
                balls[i].resetBallPosition();
                paddles.at(0).resetPaddlePosition(50.0f, 360.0f);
                paddles.at(1).resetPaddlePosition(1230.0f, 360.0f);
            }
            if (balls[i].getXposition() >= 1280) {
                paddles.at(1).increaseScore();
                balls[i].resetBallPosition();
                paddles.at(0).resetPaddlePosition(50.0f, 360.0f);
                paddles.at(1).resetPaddlePosition(1230.0f, 360.0f);
            }
        }
        scoreText.setString("Player 1: " + std::to_string(paddles.at(1).getScore()) +
            "   Player 2: " + std::to_string(paddles.at(0).getScore()));

        window.clear();

        for (int i = 0; i < paddles.size(); i++) {
            paddles[i].render(window);
        }

        for (int i = 0; i < balls.size(); i++) {
            balls[i].render(window);
        }
        window.draw(scoreText);

        window.display();
    }

    scoreText.setPosition(220, 250);
    scoreText.setCharacterSize(50);
    sf::RenderWindow winScreen(sf::VideoMode(800, 600), "Win Screen");
    winScreen.setFramerateLimit(60);
    while (winScreen.isOpen()) {
        sf::Event event;
        while (winScreen.pollEvent(event)) {
            if (event.type == sf::Event::Closed) winScreen.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) winScreen.close();

            if (playerOneWon) {
                scoreText.setString("Player Two Wins!!");
            }
            if (playerTwoWon) {
                scoreText.setString("Player One Wins!!");
            }
        }
        winScreen.clear();
        winScreen.draw(scoreText);
        winScreen.display();
    }

    return 0;
}
