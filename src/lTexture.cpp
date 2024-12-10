#include "lTexture.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include "globals.h"

LTexture::LTexture()
    : mTexture(nullptr), mWidth(0), mHeight(0) {}

LTexture::~LTexture() {
    free();
}

bool LTexture::loadFromFile(const std::string path, SDL_Renderer* renderer) {
    free();
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!mTexture) {
        std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
    return true;
}

bool LTexture::loadFromRenderedText(const std::string textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer) {
    free();
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
        return false;
    }

    mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!mTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return false;
    }

    mWidth = textSurface->w;
    mHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    return true;
}

void LTexture::free() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip, SDL_Renderer* renderer) {
    SDL_Rect renderQuad = {x, y, mWidth * SCALE_FACTOR, mHeight * SCALE_FACTOR};
    if (clip) {
        renderQuad.w = clip->w * SCALE_FACTOR;
        renderQuad.h = clip->h * SCALE_FACTOR;
    }
    SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
}

void LTexture::renderText(int x, int y, int size, SDL_Renderer* renderer) {
    SDL_Rect renderQuad = {x, y, mWidth * size, mHeight * size};
    SDL_RenderCopy(renderer, mTexture, nullptr, &renderQuad);
}

int LTexture::getWidth() const {
    return mWidth;
}

int LTexture::getHeight() const {
    return mHeight;
}

