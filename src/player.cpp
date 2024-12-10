#include "player.h"
#include "globals.h"
#include "lTexture.h"
#include <iostream>

Player::Player(): playerPosX(4), playerPosY(4), playerVelocityX(0), playerVelocityY(0), maxJump(330), isJumping(false) {

        playerCollider.w = 13 * SCALE_FACTOR;
        playerCollider.h = 21 * SCALE_FACTOR;
        playerCollider.x = playerPosX; // Adjusted to center the collision box
        playerCollider.y = playerPosY; // Adjusted based on sprite positioning
        
        //load idle clips
        for (int i = 0; i < 12; i++) {
            SpriteIdleClips[i].x = 9 + (32 * i);
            SpriteIdleClips[i].y = 11;
            SpriteIdleClips[i].w = 23;
            SpriteIdleClips[i].h = 21;
        }

        for (int i = 0; i < 12; i++) {
            SpriteRunningRightClips[i].x = 9 + (32 * i);
            SpriteRunningRightClips[i].y = 11;
            SpriteRunningRightClips[i].w = 23;
            SpriteRunningRightClips[i].h = 21;
        }

        for (int i = 0; i < 12; i++) {
            SpriteRunningLeftClips[i].x = 9 + (32 * i);
            SpriteRunningLeftClips[i].y = 11;
            SpriteRunningLeftClips[i].w = 23;
            SpriteRunningLeftClips[i].h = 21;
        }
    }

Player::~Player(){
    
}

// Inside the Player class, update the handleEvents method
void Player::handleEvents(SDL_Event & e, Uint64 deltaTime) {
if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
    switch (e.key.keysym.sym) {
        case SDLK_UP:
            if (isGrounded) {
                isJumping = true;
                playerVelocityY = -maxJump;  // Apply jump speed
                isGrounded = false;
            }
            break;
        case SDLK_LEFT:
            playerVelocityX = -PLAYER_VELOCITY; // Move left
            break;
        case SDLK_RIGHT:
            playerVelocityX = PLAYER_VELOCITY; // Move right
            break;
        case SDLK_SPACE:
    const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );

    // Check if the player is jumping and determine the direction of the dash
    if (isJumping) {
        if (currentKeyStates[SDL_SCANCODE_RIGHT] && currentKeyStates[SDL_SCANCODE_UP]) {
            playerVelocityY = -maxJump;   // Dash upwards and right
            playerVelocityX = maxJump;    // Move right
        } else if (currentKeyStates[SDL_SCANCODE_LEFT] && currentKeyStates[SDL_SCANCODE_UP]) {
            playerVelocityY = -maxJump;   // Dash upwards and left
            playerVelocityX = -maxJump;   // Move left
        } else if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
            playerVelocityY = -maxJump / 2;  // Dash while jumping right
            playerVelocityX = maxJump * 1.5; // Move right
        } else if (currentKeyStates[SDL_SCANCODE_LEFT]) {
            playerVelocityY = -maxJump / 2;  // Dash while jumping left
            playerVelocityX = -maxJump * 1.5; // Move left
        } else {
            playerVelocityY = -maxJump * 1.5;  // Normal jump (upwards only)
        }
        
        isJumping = false;  // End jump action after space is pressed
        isGrounded = false; // Ensure player isn't considered grounded
    }
    break; 
    }

} else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
    switch (e.key.keysym.sym) {
        case SDLK_UP:
            // If jump velocity is less than the max, adjust to simulate jump cap
            if (playerVelocityY < JUMP_VELOCITY) {
                playerVelocityY /= 4;  // Cap jump height
            }
            break;
        case SDLK_LEFT:
        case SDLK_RIGHT:
            playerVelocityX = 0; // Stop horizontal movement if key is released
            break;
        case SDLK_SPACE:
            // Stop jump early if needed (already handled in jump)
            if (playerVelocityY < JUMP_VELOCITY) {
                playerVelocityY /= 4;  // Cap jump speed
            }
            break;
    }
}
}

void Player::move(SDL_Rect tiles[], int numTiles, Uint64 deltaTime) {
    // Convert deltaTime to seconds
    float deltaTimeSeconds = deltaTime * 0.001f;

    // ---- HORIZONTAL MOVEMENT ----
    playerPosX += playerVelocityX * deltaTimeSeconds;
    playerCollider.x = playerPosX; // Update collision box

    // Handle horizontal collisions with boundaries and the tiles
    bool collidingWithTile = false;
    for (int i = 0; i < numTiles; i++) {
        if (checkCollision(playerCollider, tiles[i])) {
            collidingWithTile = true;
            break; // If colliding with any tile, stop checking further tiles
        }
    }

    if (playerCollider.x < 0 || playerCollider.x + playerCollider.w > SCREEN_WIDTH || collidingWithTile) {
        playerPosX -= playerVelocityX * deltaTimeSeconds; // Undo movement
        playerCollider.x = playerPosX;
        playerVelocityX = 0; // Stop horizontal velocity after collision
    }

    // ---- VERTICAL MOVEMENT ----
    if (!isGrounded) {
        // Apply gravity when airborne
        playerVelocityY += GRAVITY * deltaTimeSeconds;

        // Cap falling speed to prevent the player from falling too fast
        if (playerVelocityY > 1500) {
            playerVelocityY = 1500;
        }
    }

    playerPosY += playerVelocityY * deltaTimeSeconds;
    playerCollider.y = playerPosY; // Update collision box

    // ---- COLLISION DETECTION ----
    bool isCollidingWithFloor = false;
    bool isFallingOffEdge = false;

    // Check if the player is near the edge of the ground or platform (simulate edge detection)
    if (playerCollider.y + playerCollider.h > SCREEN_HEIGHT) {
        isCollidingWithFloor = true;
        playerPosY = SCREEN_HEIGHT - playerCollider.h; // Align to the bottom of the screen
        playerCollider.y = playerPosY;
        playerVelocityY = 0; // Stop vertical velocity when touching the floor
    }

    // Check if the player is falling off the edge (detect no floor beneath them)
    SDL_Rect edgeCheck = playerCollider;
    edgeCheck.y += playerCollider.h + 1;  // Check just below the player's current position

    bool collidingWithGround = false;
    for (int i = 0; i < numTiles; i++) {
        if (checkCollision(edgeCheck, tiles[i])) {
            collidingWithGround = true;
            break;
        }
    }

    if (!collidingWithGround && playerVelocityY >= 0) {
        isFallingOffEdge = true;
        isGrounded = false;
    }

    // If colliding with the tile, stop vertical movement
    bool isCollidingWithWall = false;
    for (int i = 0; i < numTiles; i++) {
        if (checkCollision(playerCollider, tiles[i])) {
            isCollidingWithWall = true;
            if (playerVelocityY > 0) {
                playerPosY = tiles[i].y - playerCollider.h;
            } else if (playerVelocityY < 0) {
                playerPosY = tiles[i].y + tiles[i].h;
            }
            break;
        }
    }

    if (isCollidingWithWall) {
        playerCollider.y = playerPosY;
        playerVelocityY = 0; // Reset vertical velocity if colliding with a tile
    }

    // Update grounded state based on collisions
    if (playerCollider.y + playerCollider.h == SCREEN_HEIGHT || isCollidingWithWall) {
        isGrounded = true; // Player is grounded if they touch the floor or a platform
    } else if (isFallingOffEdge || playerVelocityY > 0) {
        isGrounded = false; // Player is falling off the edge or is in the air
    }

    // ---- DEBUGGING ----
    std::cerr << "Position: (" << playerCollider.x << ", " << playerCollider.y << ") "
              << "Grounded: " << std::boolalpha << isGrounded << std::endl;

    // ---- ANIMATION UPDATE ----
    updateAnimationState();
}

void Player::updateAnimationState()

    {
        if (playerVelocityY == 0 && playerVelocityX == 0) {
            animationState = IDLE; // Idle animation
        } else if (playerVelocityX > 0) {
            animationState = RUNRIGHT; // Running right animation
        } else if (playerVelocityX < 0) {
            animationState = RUNLEFT; // Running left animation
        }   
    }
   
void Player::render(LTexture& gPlayerSpriteSheetTextureIdle, LTexture& gPlayerSpriteSheetTextureRunningRight, LTexture& gPlayerSpriteSheetTextureRunningLeft, SDL_Renderer* gRenderer) {
        switch (animationState) {
        case (RUNRIGHT):
            gPlayerSpriteSheetTextureRunningRight.render(playerPosX, playerPosY, & SpriteRunningRightClips[(SDL_GetTicks() / 100) % 12], gRenderer );
            break;
        case (IDLE):
            std::cout << "IDLE" << std::endl;
            gPlayerSpriteSheetTextureIdle.render(playerPosX, playerPosY,& SpriteIdleClips[(SDL_GetTicks() / 100) % 12], gRenderer);
            break;
        case (RUNLEFT):
            gPlayerSpriteSheetTextureRunningLeft.render(playerPosX, playerPosY, & SpriteRunningLeftClips[(SDL_GetTicks() / 100) % 12], gRenderer);
            break;
        }
        // Render the collision box (for debugging purposes)
        SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255); // Red color for the hitbox
        SDL_RenderDrawRect(gRenderer, &playerCollider); // Draw the collision box
    }

void Player::updatePlayerPosText(std::stringstream* posText) {
        if (posText) {
            posText->str("");
            posText->clear();
            *posText << "x: " << playerPosX << " y: " << playerPosY << " g? " << isGrounded;
        }
    }


// Check for collisions between two rectangles
bool Player::checkCollision(SDL_Rect a, SDL_Rect b) {
    return !(a.x + a.w <= b.x || a.x >= b.x + b.w || a.y + a.h <= b.y || a.y >= b.y + b.h);
}
