#include <raylib.h>
#include "ball.h"
#include <iostream>
#include <deque>
#include "C:/raylib/raylib/src/raymath.h"

using namespace std;

Color backColor =GetColor(0x371E30FF); //add 0x to beginning of hex color and FF after (FF means full opacity)
    //so this color is #371E30

Color snakeColor =GetColor(0xF6828CFF); // color is #F6828C

Color foodColor =GetColor(0xA03E99FF);

Color borderColor =GetColor(0xDF57BCFF);

Color borderTrimColor =GetColor(0xF59CA9FF);

int cellSize=25;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i = 0; i<deque.size(); i++)
    {
        if(Vector2Equals(deque[i],element))
        return true;
    }
    return false;
}

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    else {
        return false;
    }
}

class Snake
{
    public : 
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    Vector2 nextDirection = {1,0};
    bool addSegment = false;

    void Draw(bool win)
    {
        
        Color startColor = RED;   // pick any two colors
        Color endColor   = YELLOW;

        if(win == false) {

        for(unsigned int i = 0; i < body.size(); i++)
        {
            float x= body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, snakeColor);
        }
        }
        else{
            for (unsigned int i = 0; i < body.size(); i++) {
        float t = (float)i / (body.size() - 1); // goes from 0 to 1
        Color segColor = {
        (unsigned char)(startColor.r + t * (endColor.r - startColor.r)),
        (unsigned char)(startColor.g + t * (endColor.g - startColor.g)),
        (unsigned char)(startColor.b + t * (endColor.b - startColor.b)),
        255
    };
    DrawRectangleV(
        Vector2{offset+ body[i].x * cellSize,offset +  body[i].y * cellSize},
        Vector2{(float)cellSize, (float)cellSize},
        segColor
    );
        }
    }
    }

    void Update()
    {
        direction = nextDirection;
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {

            
            addSegment = false;

        }
        else{
            body.pop_back();
            
            
        }

        
    }

    void Reset()
    {   
        snakeColor = GetColor(0xF6828CFF);
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
        nextDirection = {1,0};
        
    }

};

class Food {

    public:
    Vector2 position; //hold the position of the food object at {x,y}
    Texture2D texture;

    

    Food(deque<Vector2> snakeBody)
    {
        Image image = LoadImage("graphics/food.png");
        
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    void Draw()
    {
        DrawTexture(texture, offset +  position.x*cellSize, offset +  position.y*cellSize, foodColor);
    }

    Vector2 GenerateRandomCells(){
        float x = GetRandomValue(0,cellCount-1);
        float y = GetRandomValue(0,cellCount-1);
        return Vector2{x,y};

    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody )
    {
        
        Vector2 position = GenerateRandomCells();
        while(ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCells();
        }
        return position;
        
    }
};

class Game{
    public : 
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    int highScore = 0;
    bool win = false;

    Sound eatSound;
    Sound wallSound;

    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("sounds/eat.mp3");
        wallSound = LoadSound("sounds/wall.mp3");
    }

    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw(win);
    }

    void Update(){

        
        if(running){
            
            snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
            

        }
        if (snake.body.size() >= cellCount* cellCount) 
            {
                running = false;
                win = true;
            }
        
    }

    void CheckCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score ++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if(ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }

    }

    void GameOver()
    {
        win = false;
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        if (score > highScore) {
            highScore = score;
        }
        score = 0;
        PlaySound(wallSound);

    }
};

int main() 
{
    cout << "Starting the game..." << endl;
    //make a window with width, height, and title. You must destroy every window you create
    InitWindow(2*offset + cellSize * cellCount,2*offset +  cellSize * cellCount, "Snakey Wakey");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose()==false) // checks for esc or close button
    {
        BeginDrawing();

        if(eventTriggered(0.2))
        {
            game.Update();
        }
        
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.nextDirection = {0, -1};
            
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.nextDirection = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT)&& game.snake.direction.x != -1){
            game.snake.nextDirection = {1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.nextDirection = {-1, 0};
            game.running = true;
        }
        
        
        ClearBackground(backColor);
        
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, borderTrimColor);
        DrawText("Snakey Wakey", offset - 5 , 20, 40 , borderTrimColor);
        DrawText(TextFormat("%i",game.score), offset - 5 , offset + cellSize*cellCount + 10, 40, borderTrimColor);
        DrawText(TextFormat("%i",game.highScore), cellSize*cellCount - (offset - 5) , offset + cellSize*cellCount + 10, 40, borderTrimColor);
        game.Draw();
        EndDrawing();
    }
    
    
    
    
    CloseWindow();

    
}