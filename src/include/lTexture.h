#ifndef LTEXURE_H
#define LTEXURE_H

#include "SDL2/SDL_pixels.h"
#include <SDL2/SDL.h>
#include <string>

typedef struct _TTF_Font TTF_Font;

class LTexture {
public:
    // Constructors and Destructor
    LTexture();
    ~LTexture();

    // Load image from file
    bool loadFromFile(std::string path, SDL_Renderer* gRenderer);

    bool loadFromRenderedText(std::string textureText, SDL_Color textColor,TTF_Font *gFont, SDL_Renderer* renderer);

    // Deallocates texture
    void free();

    // Renders texture at given point (x, y)
    void render(int x, int y, SDL_Rect * clip = NULL, SDL_Renderer* renderer = NULL);
    void renderText(int x, int y, int size, SDL_Renderer* renderer);    // Get texture dimensions
    int getWidth() const;
    int getHeight() const;

private:
    // The texture
    SDL_Texture* mTexture;

    // Texture dimensions
    int mWidth;
    int mHeight;
};

#endif // LTEXURE_H
