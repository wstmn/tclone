/*This source code copyrighted by Lazy Foo' Productions 2004-2024
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

class Player
{
  public:
    static const int RECT_WIDTH = SCREEN_WIDTH / 16;
    static const int RECT_HEIGHT = SCREEN_HEIGHT / 16;
    static const int DOT_VEL = 1;

    Player();

    void handleEvents( SDL_Event& e);
    void move(SDL_Rect wall);

    void render();

  private:
    int RED_RECT_X;
    int RED_RECT_Y;
    int mRED_RECT_VELX;
    int mRED_RECT_VELY;

    SDL_Rect RED_RECT_COLLIDER;
};

Player::Player()
{
  RED_RECT_X = 4;
  RED_RECT_Y = 4;

  RED_RECT_COLLIDER.w = RECT_WIDTH;
  RED_RECT_COLLIDER.h = RECT_HEIGHT;
  
  mRED_RECT_VELX = 0;
  mRED_RECT_VELY = 0;
}

//Check Collision
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    //The sides of the rectangles
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;

  //Calculate the sides of rect A
  leftA = a.x;
  rightA = a.x + a.w;
  topA = a.y;
  bottomA = a.y + a.h;
  std::cout << "left a: " << leftA << "Right a: " << rightA << "Top A: " << topA << "Bottom A: " << bottomA << std::endl;

  //Calculate the sides of rect B
  leftB = b.x;
  rightB = b.x + b.w;
  topB = b.y;
  bottomB = b.y + b.h;

      //If any of the sides from A are outside of B
  if( bottomA <= topB )
  {
      return false;
  }

  if( topA >= bottomB )
  {
      return false;
  }

  if( rightA <= leftB )
  {
      return false;
  }

  if( leftA >= rightB )
  {
      return false;
  }

  //If none of the sides from A are outside B
  return true;

}

//Frees media and shuts down SDL
void close();

//Loads individual image as texture
SDL_Texture* loadTexture( std::string path );

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* loadTexture( std::string path )
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

void Player::move(SDL_Rect wall)
{
  RED_RECT_X += mRED_RECT_VELX;
  RED_RECT_COLLIDER.x = RED_RECT_X;

  if((RED_RECT_X < 0) || (RED_RECT_X + RECT_WIDTH > SCREEN_WIDTH) || checkCollision(RED_RECT_COLLIDER, wall))
  {
    RED_RECT_X -= mRED_RECT_VELX;
    RED_RECT_COLLIDER.x = RED_RECT_X;
  }  

  RED_RECT_Y += mRED_RECT_VELY;
  RED_RECT_COLLIDER.y = RED_RECT_Y;
  if((RED_RECT_Y < 0) || (RED_RECT_Y + RECT_HEIGHT > SCREEN_HEIGHT) || checkCollision(RED_RECT_COLLIDER, wall))
  {
    RED_RECT_Y -= mRED_RECT_VELY;
    RED_RECT_COLLIDER.y = RED_RECT_Y;
  }
}

void Player::handleEvents(SDL_Event& e)
{
if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
          {
              //Adjust the velocity
              switch( e.key.keysym.sym )
              {
                  case SDLK_UP: mRED_RECT_VELY -= DOT_VEL; break;
                  case SDLK_DOWN: mRED_RECT_VELY += DOT_VEL; break;
                  case SDLK_LEFT: mRED_RECT_VELX -= DOT_VEL; break;
                  case SDLK_RIGHT: mRED_RECT_VELX += DOT_VEL; break;
              }

          }else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
          {
              //Adjust the velocity
              switch( e.key.keysym.sym )
              {
                  case SDLK_UP: mRED_RECT_VELY += DOT_VEL; break;
                  case SDLK_DOWN: mRED_RECT_VELY -= DOT_VEL; break;
                  case SDLK_LEFT: mRED_RECT_VELX += DOT_VEL; break;
                  case SDLK_RIGHT: mRED_RECT_VELX -= DOT_VEL; break;
              }
          }

}

void Player::render()
{
  SDL_Rect fillRect = { RED_RECT_X, RED_RECT_Y, RECT_WIDTH, RECT_HEIGHT};
  SDL_SetRenderDrawColor( gRenderer, 0xFF, 0x00, 0x00, 0xFF );		
  SDL_RenderFillRect( gRenderer, &fillRect );
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

      Player player;

      //Set Collision  
      int GREEN_RECT_WIDTH = SCREEN_WIDTH * 2 / 3;
      int GREEN_RECT_HEIGHT = SCREEN_HEIGHT * 2 / 3;

      int GREEN_RECT_X = SCREEN_WIDTH / 6;
      int GREEN_RECT_Y = SCREEN_HEIGHT / 6;

      SDL_Rect wall;
      wall.x = GREEN_RECT_X;
      wall.y = GREEN_RECT_Y;
      wall.w = GREEN_RECT_WIDTH;
      wall.h = GREEN_RECT_HEIGHT;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
          //handle player movement and collisions
          player.handleEvents(e);
				}

        //Move Rectangle
        player.move(wall);

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render player
        player.render();
			
				//Render green outlined quad
				SDL_Rect outlineRect = { GREEN_RECT_X, GREEN_RECT_Y, GREEN_RECT_WIDTH, GREEN_RECT_HEIGHT };
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0xFF, 0x00, 0xFF );		
				SDL_RenderDrawRect( gRenderer, &outlineRect );
				
				//Draw blue horizontal line
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );		
				SDL_RenderDrawLine( gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2 );

				//Draw vertical line of yellow dots
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0x00, 0xFF );
				for( int i = 0; i < SCREEN_HEIGHT; i += 4 )
				{
					SDL_RenderDrawPoint( gRenderer, SCREEN_WIDTH / 2, i );
				}

				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

