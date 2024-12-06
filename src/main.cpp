#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <SDL2/SDL_ttf.h>

#include <iostream>

#include <string>

#include "SDL2/SDL_error.h"
#include "SDL2/SDL_pixels.h"
#include "globals.h"

enum playerAnimationState {
    RUNRIGHT,
    IDLE,
    RUNLEFT,
    JUMP,
    FALLING
};

// The window and renderer
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;

//global sprite clips
SDL_Rect gSpriteIdleClips[12];
SDL_Rect gSpriteRunningRightClips[12];
SDL_Rect gSpriteRunningLeftClips[12];

//global fonts
TTF_Font* gFont = NULL;

// Utility Class to manage textures
class LTexture {
    public: LTexture(): mTexture(NULL),
    mWidth(0),
    mHeight(0) {}

    ~LTexture() {
        free();
    }

    bool loadFromFile(std::string path) {
        free();
        SDL_Surface * loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface) {
            std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface -> format, 0, 0xFF, 0xFF));
        mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (!mTexture) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
            return false;
        }

        mWidth = loadedSurface -> w;
        mHeight = loadedSurface -> h;
        SDL_FreeSurface(loadedSurface);
        return true;
    }

    bool loadFromRenderedText(std::string textureText, SDL_Color textColor)
    {
        free();

        SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
        if(textSurface != NULL)
        {
            mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            if(mTexture == NULL){
                std::cout << "unable to create texture from rendered text! SDL ERROR: " << SDL_GetError() << std::endl;
            }else {
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            }
        SDL_FreeSurface(textSurface);

        }
        else
        {
            std::cout << "unable to render text surface: " << SDL_GetError() << std::endl;
        }

        return mTexture != NULL;
    }

    void free() {
        if (mTexture) {
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
    }

    void render(int x, int y, SDL_Rect * clip = NULL) {
        SDL_Rect renderQuad = {
            x,
            y,
            mWidth * SCALE_FACTOR,
            mHeight * SCALE_FACTOR
        };
        if (clip) {
            renderQuad.w = clip -> w * SCALE_FACTOR;
            renderQuad.h = clip -> h * SCALE_FACTOR;
        }
        SDL_RenderCopy(gRenderer, mTexture, clip, & renderQuad);
    }

    int getWidth() {
        return mWidth;
    }
    int getHeight() {
        return mHeight;
    }

    private: SDL_Texture * mTexture;
    int mWidth;
    int mHeight;
    bool isGrounded = false;
};

// Declare the global instance of LTexture
LTexture gPlayerSpriteSheetTextureIdle;
LTexture gPlayerSpriteSheetTextureRunningRight;
LTexture gPlayerSpriteSheetTextureRunningLeft;

// Player class to handle movement and rendering
class Player {
    public: static
    const int SPRITE_WIDTH = 32 * SCALE_FACTOR; // Player sprite width
    static
    const int SPRITE_HEIGHT = 32 * SCALE_FACTOR; // Player sprite height
    static
    const int PLAYER_VELOCITY = 2; // Player movement speed
    int animationState = 1;

    Player(): playerPosX(4),
    playerPosY(4),
    playerVelocityX(0),
    playerVelocityY(0) {
        playerCollider.w = 13 * SCALE_FACTOR;
        playerCollider.h = 21 * SCALE_FACTOR;
        playerCollider.x = playerPosX; // Adjusted to center the collision box
        playerCollider.y = playerPosY; // Adjusted based on sprite positioning
    }

    const int JUMP_VELOCITY = -10; // The initial velocity when jumping

    // Inside the Player class, update the handleEvents method
    void handleEvents(SDL_Event & e) {
        if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (isGrounded) {
                    playerVelocityY = JUMP_VELOCITY; // Apply upward velocity for jump
                    isGrounded = false;
                }
                break;
            case SDLK_LEFT:
                playerVelocityX -= PLAYER_VELOCITY;
                break;
            case SDLK_RIGHT:
                playerVelocityX += PLAYER_VELOCITY;
                break;
            }
        } else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
            switch (e.key.keysym.sym) {
            case SDLK_UP:
                // Stop the jump when the key is released
                if (playerVelocityY < JUMP_VELOCITY) {
                    playerVelocityY = JUMP_VELOCITY;  // Cap the jump velocity
                }
                break;
            case SDLK_LEFT:
                playerVelocityX += PLAYER_VELOCITY;
                break;
            case SDLK_RIGHT:
                playerVelocityX -= PLAYER_VELOCITY;
                break;
            }
        }
    }

    // In the move method, adjust gravity and add the ability to apply different jump heights
    void move(SDL_Rect wall) {
        playerPosX += playerVelocityX;
        playerCollider.x = playerPosX; // Adjust for collision box alignment

        std::cerr << "Collider Position: " << playerCollider.x << ", " << playerCollider.y << std::endl;

        // Collision detection with boundaries and wall
        if (playerCollider.x < 0 || playerCollider.x + playerCollider.w > SCREEN_WIDTH || checkCollision(playerCollider, wall)) {
            playerPosX -= playerVelocityX;
            playerCollider.x = playerPosX;
        }

        // Handle GRAVITY and falling mechanics
        if (!isGrounded) {
            playerVelocityY += GRAVITY; // Apply gravity, causing the player to fall faster
            if (playerVelocityY > 3) { // Cap the maximum falling speed
                playerVelocityY = 3;
            }
        } else {
            playerVelocityY = 0; // Reset vertical speed when isGrounded
        }
        
        updateAnimationState();

        // Update the Y position based on velocity
        playerPosY += playerVelocityY;
        playerCollider.y = playerPosY; // Adjust for collision box alignment

        // Collision detection for vertical movement
        if (checkCollision(playerCollider, wall)) {
            isGrounded = true;
            playerPosY = wall.y - playerCollider.h; // Adjust position to sit on top of the wall
            playerCollider.y = playerPosY;
            playerVelocityY = 0; // Stop downward movement when grounded
        }

        // Prevent the player from going off the bottom of the screen
        if (playerCollider.y < 0 || playerCollider.y + playerCollider.h > SCREEN_HEIGHT) {
            playerPosY -= playerVelocityY;
            playerCollider.y = playerPosY;
        }
    }

    void updateAnimationState()
    {
        if (playerVelocityY == 0 && playerVelocityX == 0) {
            animationState = IDLE; // Idle animation
        } else if (playerVelocityX > 0) {
            animationState = RUNRIGHT; // Running right animation
        } else if (playerVelocityX < 0) {
            animationState = RUNLEFT; // Running left animation
        }   
    }
   
    void render() {
        switch (animationState) {
        case (RUNRIGHT):
            gPlayerSpriteSheetTextureRunningRight.render(playerPosX, playerPosY, & gSpriteRunningRightClips[(SDL_GetTicks() / 100) % 12] );
            break;
        case (IDLE):
            gPlayerSpriteSheetTextureIdle.render(playerPosX, playerPosY,& gSpriteIdleClips[(SDL_GetTicks() / 100) % 12] );
            break;
        case (RUNLEFT):
            gPlayerSpriteSheetTextureRunningLeft.render(playerPosX, playerPosY, & gSpriteRunningLeftClips[(SDL_GetTicks() / 100) % 12]);
            break;
        }
        // Render the collision box (for debugging purposes)
        //SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255); // Red color for the hitbox
        //SDL_RenderDrawRect(gRenderer, &playerCollider); // Draw the collision box
    }

    private: int playerPosX; // Player X position
    int playerPosY; // Player Y position
    int playerVelocityX; // Player X velocity
    int playerVelocityY; // Player Y velocity
    bool isGrounded;
    SDL_Rect playerCollider; // Player collision box
};

// Check for collisions between two rectangles
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w || a.y + a.h <= b.y || a.y >= b.y + b.h);
}

// Main function //
int main(int argc, char * args[]) {
    if (!init()) {
        return 1;
    }

    if (!loadMedia()) {
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    Player player;

    SDL_Rect wall = {
        0,
        SCREEN_HEIGHT - SCREEN_HEIGHT / 3,
        SCREEN_WIDTH,
        SCREEN_HEIGHT / 3
    };

    while (!quit) {
        while (SDL_PollEvent( & e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            player.handleEvents(e);
        }

        player.move(wall);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        player.render();

        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderDrawRect(gRenderer, & wall);

        SDL_RenderPresent(gRenderer);
    }

    close();
    return 0;
}

// Initialize SDL, create window, and renderer
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Clean up resources
void close() {
    gPlayerSpriteSheetTextureIdle.free();
    gPlayerSpriteSheetTextureRunningRight.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();
}

// Load media (textures)
bool loadMedia() {

    if (!gPlayerSpriteSheetTextureIdle.loadFromFile("assets/sprites/player/idle/robot-idle.png") ||
        !gPlayerSpriteSheetTextureRunningRight.loadFromFile("assets/sprites/player/idle/robot-running.png") ||
        !gPlayerSpriteSheetTextureRunningLeft.loadFromFile("assets/sprites/player/idle/robot-running-left.png")) {

        std::cerr << "Failed to load sprite sheets!" << std::endl;
        return false;
    }

    gFont = TTF_OpenFont("fonts/lazy.ttf", 28);
    if (gFont == NULL)
    {
        std::cout << "FAILED TO LOAD FONT! SDL_ttf Error: " << TTF_GetError();
    }
    //load idle clips
    for (int i = 0; i < 12; i++) {
        gSpriteIdleClips[i].x = 9 + (32 * i);
        gSpriteIdleClips[i].y = 11;
        gSpriteIdleClips[i].w = 23;
        gSpriteIdleClips[i].h = 21;
    }

    for (int i = 0; i < 12; i++) {
        gSpriteRunningRightClips[i].x = 9 + (32 * i);
        gSpriteRunningRightClips[i].y = 11;
        gSpriteRunningRightClips[i].w = 23;
        gSpriteRunningRightClips[i].h = 21;
    }

    for (int i = 0; i < 12; i++) {
        gSpriteRunningLeftClips[i].x = 9 + (32 * i);
        gSpriteRunningLeftClips[i].y = 11;
        gSpriteRunningLeftClips[i].w = 23;
        gSpriteRunningLeftClips[i].h = 21;
    }
    return true;
}
