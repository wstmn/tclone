#ifndef PLAYER_H
#define PLAYER_H
#include "SDL2/SDL_stdinc.h"
#include "lTexture.h"
#include <SDL2/SDL.h>
#include <sstream>

enum playerAnimationState {
    RUNRIGHT,
    IDLE,
    RUNLEFT,
    JUMP,
    FALLING
};

class Player {

public: 
    static const int SPRITE_WIDTH = 32; // Player sprite width
    static const int SPRITE_HEIGHT = 32; // Player sprite height
    static const int PLAYER_VELOCITY = 300; // Player movement speed
    const float maxJump;

    int animationState = 1;
    int pJumpSpeed;
    bool isJumping;    
    Player();
    ~Player();

    bool loadTextures();
    void handleEvents(SDL_Event & e, Uint64 deltaTime);
    void move(SDL_Rect tiles[], int numTiles, Uint64 deltaTime);
    void updateAnimationState();
    void render(LTexture& gPlayerSpriteSheetTextureIdle, LTexture& gPlayerSpriteSheetTextureRunningRight, LTexture& gPlayerSpriteSheetTextureRunningLeft, SDL_Renderer* gRenderer);
    void updatePlayerPosText(std::stringstream * posText);
    bool checkCollision(SDL_Rect a, SDL_Rect b);

private:
    float playerPosX; // Player X position
    float playerPosY; // Player Y position
    float playerVelocityX; // Player X velocity
    float playerVelocityY; // Player Y velocity
    bool isGrounded;
    SDL_Rect playerCollider; // Player collision box

    SDL_Rect SpriteIdleClips[12];
    SDL_Rect SpriteRunningRightClips[12];
    SDL_Rect SpriteRunningLeftClips[12];
};
#endif  // PLAYER_H
