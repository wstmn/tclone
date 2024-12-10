#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "SDL2/SDL_error.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_timer.h"

#include <iostream>
#include <sstream>
#include <string>

#include "globals.h"
#include "lTexture.h"
#include "player.h"

// The window and renderer
SDL_Window * gWindow = NULL;
SDL_Renderer * gRenderer = NULL;

// Declare the global instance of LTexture
LTexture gPlayerSpriteSheetTextureIdle, gPlayerSpriteSheetTextureRunningRight, gPlayerSpriteSheetTextureRunningLeft, gTextTexture, gTimeTextTexture, gPosTexture;
// Declare global fonts
TTF_Font* gFont = NULL;

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

    //Set text color as black
     SDL_Color textColor = { 0, 0, 0, 255 };

    //Delta Time
    Uint64 startTime = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    double timerStart = 0;

    //In memory text stream
    std::stringstream timeText;
    std::stringstream posText;


    SDL_Rect tiles[40]; // Declare the array before the loop

    // Initialize tile positions
    for (int i = 0; i < 40; i++) {
        tiles[i].x = i * 32;
        tiles[i].y = SCREEN_HEIGHT - SCREEN_HEIGHT / 5;
        tiles[i].w = 32;
        tiles[i].h = 32;
    }    

    while (!quit) {
        LAST = startTime;
        startTime = SDL_GetPerformanceCounter();


        while (SDL_PollEvent( & e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }                    //Reset start time on return keypress
            
            player.handleEvents(e, deltaTime);
        }

        player.move(tiles, sizeof(tiles) / sizeof(tiles[0]), deltaTime);

        //Set text to be rendered
        timeText.str( "" );
        timeText << "Seconds since start: " << (SDL_GetTicks() - timerStart) / 1000;

        player.updatePlayerPosText(&posText);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        //Render text
        if( !gTimeTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor , gFont, gRenderer) )
        {
            std::cout << "couldn't create texture from text";
        }

        if( !gPosTexture.loadFromRenderedText(posText.str().c_str(), textColor, gFont, gRenderer))
        {
            std::cout << "couldn't create texture from text";
        }
        
        //Render current frame
        gTextTexture.renderText( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 6, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2, 3, gRenderer);
        gTimeTextTexture.renderText( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 3, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 3 , 1, gRenderer);

        // Render tiles // WorldGrid.render();
        SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);  // Green color for tiles
        for (int i = 0; i < sizeof(tiles) / sizeof(tiles[0]); i++) {
            SDL_RenderFillRect(gRenderer, &tiles[i]);
        }

        gPosTexture.renderText(10, 10, 1, gRenderer);
        player.render(gPlayerSpriteSheetTextureIdle, gPlayerSpriteSheetTextureRunningRight, gPlayerSpriteSheetTextureRunningLeft, gRenderer);

        // Present the rendered frame
        SDL_RenderPresent(gRenderer);
        deltaTime = ((startTime - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );
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

      //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
        return false;
    }

    return true;
}

// Clean up resources
void close() {
    gPlayerSpriteSheetTextureIdle.free();
    gPlayerSpriteSheetTextureRunningRight.free();
    gPlayerSpriteSheetTextureRunningLeft.free();
    gTextTexture.free();
    gTimeTextTexture.free();
    gPosTexture.free();
    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

// Load media (textures)
bool loadMedia() {
    if (!gPlayerSpriteSheetTextureIdle.loadFromFile("assets/sprites/player/idle/robot-idle.png", gRenderer) ||
        !gPlayerSpriteSheetTextureRunningRight.loadFromFile("assets/sprites/player/idle/robot-running.png", gRenderer  ) ||
        !gPlayerSpriteSheetTextureRunningLeft.loadFromFile("assets/sprites/player/idle/robot-running-left.png", gRenderer)) {

        std::cerr << "Failed to load sprite sheets!" << std::endl;
        return false;
    }
    
    gFont = TTF_OpenFont("fonts/pixel.ttf", 28);
    if (gFont == NULL)
    {
        std::cout << "FAILED TO LOAD FONT! SDL_ttf Error: " << TTF_GetError();
        return false;
    }else
    {
        SDL_Color textColor = {0, 0, 0, 80};

        if( !gTextTexture.loadFromRenderedText("FN + WASD to move", textColor, gFont, gRenderer))
        {
            std::cout << "Could not load from rendered text." << std::endl;
            return false;
        }
    }

    return true;
} 
