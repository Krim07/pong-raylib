//Pong game redesign
//samuel.oranu
#include "raylib.h"
#include "raymath.h"
#include <string>
#include <algorithm>
#include <vector>
#include <print>

constexpr auto TEXT_SIZE = 40;
constexpr auto MARGIN = 27.0f;
constexpr auto WINDOW_WIDTH = 1080;
constexpr auto WINDOW_HEIGHT = 720;
constexpr auto PADDLE_SPEED = 400.0f;
constexpr auto PADDLE_WIDTH = 14.0f;
constexpr auto PADDLE_HEIGHT = 80.0f;
constexpr auto LAUNCH_VELOCITY = Vector2(180.0f, -350.0f);
constexpr auto BACKGROUND_COLOR = DARKBROWN;
constexpr auto TEXT_COLOR = DARKGRAY;
const auto textPositionX = (WINDOW_WIDTH / 2) - 50;
const auto winsTextPositionY = 10;
const auto textPositionX2 = (WINDOW_WIDTH / 2) + 50;
const auto winsTextFontSize = 20;
constexpr auto BALL_SPEEDUP = 1.15f;     // +15% per paddle hit
constexpr auto BALL_MAX_SPEED = 900.0f;  // max speed of the ball

enum class Side {
	LEFT, RIGHT, TOP, BOTTOM
};

enum class Axis {
	X, Y
};

struct Ball {
	Vector2 position{ 0.0f, 0.0f };
	Vector2 velocity{ 0.0f, 0.0f };
	float radius{ 8.0f };
	Color color{ RAYWHITE };

	Ball() {
		reset();
	}

	

	void nudge(Axis axis, float distance) {
		if (axis == Axis::X) {
			position.x += distance;
		}
		else {
			position.y += distance;
		}
		
	}

	void bounce(Axis axis) {
		if (axis == Axis::X) {
			velocity.x *= -1.0f;
			
		}
		else
		{
			velocity.y *= -1.0f;
		}
	}

	void update(float dt) {
		position.x += velocity.x * dt;
		position.y += velocity.y * dt;
	}

	void render() const {
		DrawCircleV(position, radius, color);
	}

	void reset() {
		position = Vector2{ WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f };
		velocity = Vector2{ 0.0f, 0.0f };
	}

	Rectangle get_rectangle() const {
		return Rectangle{ position.x - radius, position.y - radius, radius * 2, radius * 2 };
	}

	bool is_still() const {
		return Vector2Length(velocity) < 0.01f;
	}
	void launch() {
		velocity = LAUNCH_VELOCITY;
	}

	void do_collision_with(Rectangle r) {
		if (!CheckCollisionRecs(get_rectangle(), r)) {
	     	return;
		}
		speed_up();
		Rectangle overlap = GetCollisionRec(get_rectangle(), r);
		bool sidehit = overlap.width < overlap.height;
		if (sidehit) {
			float dx = (velocity.x > 0.0f) ? -overlap.width : overlap.width;
			bounce(Axis::X);
			nudge(Axis::X, dx);
		}
		else {
			float dy = (velocity.y > 0.0f) ? -overlap.height : overlap.height;
			nudge(Axis::Y, dy);
			bounce(Axis::Y);
		}
	}

	void speed_up() {
		if (Vector2Length(velocity) < BALL_MAX_SPEED) {
			velocity.x *= BALL_SPEEDUP;
			velocity.y *= BALL_SPEEDUP;
		}
	}
};

class Paddle {
	Rectangle rectangle{ 0.0f, 0.0f, PADDLE_WIDTH, PADDLE_HEIGHT };
	Color color{ RAYWHITE };
	Side side{ Side::LEFT };

public:
	Paddle(Side position) {
		side = position;
		if (side == Side::LEFT) {
			reset_position(MARGIN, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);
		}
		if (side == Side::RIGHT) {
			reset_position(WINDOW_WIDTH - MARGIN - PADDLE_WIDTH, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);
		}
	}

	void update(float dt) {
		if (side == Side::RIGHT) {
			if (IsKeyDown(KEY_UP)) {
				rectangle.y -= PADDLE_SPEED * dt;
			}
			if (IsKeyDown(KEY_DOWN)) {
				rectangle.y += PADDLE_SPEED * dt;
			}
			const auto MIN_Y = 0.0f;
			const auto MAX_Y = WINDOW_HEIGHT - rectangle.height;
			rectangle.y = std::clamp(rectangle.y, MIN_Y, MAX_Y);
		}
		if (side == Side::LEFT)
		{
			if (IsKeyDown(KEY_TAB)) {
				rectangle.y -= PADDLE_SPEED * dt;
			}
			if (IsKeyDown(KEY_LEFT_SHIFT)) {
				rectangle.y += PADDLE_SPEED * dt;
			}
			const auto MIN_Y = 0.0f;
			const auto MAX_Y = WINDOW_HEIGHT - rectangle.height;
			rectangle.y = std::clamp(rectangle.y, MIN_Y, MAX_Y);

		}
			
	}

	void render() const {
		DrawRectangleRec(rectangle, color);
	}

	Rectangle get_rectangle() const {
		return rectangle;
	}

	void reset_position(float x, float y) {
		rectangle.y = y;
		rectangle.x = x;
	}
};

class Game {
	Paddle paddle{ Side::LEFT };
	Paddle paddleB{ Side::RIGHT };
	Ball ball;
	int playerScore = 0;
	int playerBScore = 0;
	

	void update(float dt) {
		bool gameOver = (playerScore >= 5 || playerBScore >= 5);
		if (ball.is_still() && IsKeyPressed(KEY_SPACE) && !gameOver) {
			ball.launch();
		}
		paddle.update(dt);
		paddleB.update(dt);
		ball.update(dt);

		if (ball.position.x - ball.radius <= 0) {
			playerScore++;
			ball.reset();
			
			return;
		}
		if (ball.position.x + ball.radius >= WINDOW_WIDTH) {
			playerBScore++;
			ball.reset();
			
			return;
		}
		
		check_bounds_for(ball);
		ball.do_collision_with(paddle.get_rectangle());
		ball.do_collision_with(paddleB.get_rectangle());	

		if (gameOver) {

			DrawText("Press R to restart!", (int)MARGIN + TEXT_SIZE, WINDOW_HEIGHT - TEXT_SIZE * 2, TEXT_SIZE, RED);
			if (IsKeyPressed(KEY_R)) {
				playerScore = 0;
				playerBScore = 0;
				ball.reset();
				paddle.reset_position(MARGIN, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);
				paddleB.reset_position(WINDOW_WIDTH - MARGIN - PADDLE_WIDTH, (WINDOW_HEIGHT - PADDLE_HEIGHT) / 2.0f);
				
			}

		}

	}

	void render() const {
		BeginDrawing();
		ClearBackground(BACKGROUND_COLOR);
		paddle.render();
		paddleB.render();
		ball.render();
		bool gameOver = (playerScore >= 5 || playerBScore >= 5);
		
		if (ball.is_still() && (!gameOver)) {	
			DrawText("Press SPACE to serve!", (int)MARGIN + TEXT_SIZE, WINDOW_HEIGHT - TEXT_SIZE, TEXT_SIZE, TEXT_COLOR);
			DrawText("First to 5 Wins!", (int)MARGIN + TEXT_SIZE + 650, WINDOW_HEIGHT - TEXT_SIZE, TEXT_SIZE, TEXT_COLOR);
		}
		if (playerScore == 5) {
			DrawText("Player 1 Wins!", (int)MARGIN + TEXT_SIZE + 500, WINDOW_HEIGHT - TEXT_SIZE * 2, TEXT_SIZE, RED);
		}
		if (playerBScore == 5) {
			DrawText("Player 2 Wins!", (int)MARGIN + TEXT_SIZE + 500, WINDOW_HEIGHT - TEXT_SIZE * 2, TEXT_SIZE, GREEN);
		}

		
		DrawText(TextFormat("Wins: %i", playerScore), textPositionX, winsTextPositionY, winsTextFontSize, BLUE);
		DrawText(TextFormat("Wins: %i", playerBScore), textPositionX2, winsTextPositionY, winsTextFontSize, GREEN);

		

		EndDrawing();
	}

	void check_bounds_for(Ball& b) {
		auto& pos = b.position; //create a local reference to the Ball::pos member, for convenience		
		const auto MAX_Y = WINDOW_HEIGHT - b.radius;
		if (pos.y < b.radius) {
			pos.y = b.radius;
			b.bounce(Axis::Y);
		}
		else if (pos.y > MAX_Y) {
			pos.y = MAX_Y;
			b.bounce(Axis::Y);
		}
	}

	
	
public:
	Game(std::string title, int width, int height, int fps = 60) {
		InitWindow(width, height, title.c_str());
		SetTargetFPS(fps);
	}

	~Game() {
		CloseWindow();
	}
	

	void run() {
		while (!WindowShouldClose()) {
			auto dt = GetFrameTime();
			update(dt);
			
			render();
					
		}
	}
};

int main() {
	Game game("Pong redesign!", WINDOW_WIDTH, WINDOW_HEIGHT);
	std::println("Game is running");
	game.run();
	return 0;
}