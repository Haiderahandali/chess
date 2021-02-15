#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

struct vector2d
{
    float x = 0;
    float y = 0;
};

bool init();
void drawBoard();
void eventLoop(SDL_Event event);
void loadTexture(char const*);

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 640;

SDL_Texture* gTexture   = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow     = NULL;

void render();
void drawPiece(char const*, vector2d pos);

bool quit = false;

int main(int argc, char** argv)
{
    if (init())
        std::cout << "SDL initialize is done"
                  << "\n"
                  << argc << argv[0];

    SDL_Event event;

    while (!quit)
    {
        drawBoard();

        for (float i = 25; i <= 600; i += 75)
            for (float j = 25; j <= 600; j += 75)
            {
                drawPiece("/Users/aliabdulkareem/dev/chess/res/WhiteNight.png", { i, j });
            }
        render();
        eventLoop(event);
    }

    return 0;
}

void eventLoop(SDL_Event e)
{
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        default:
            break;
        }
    }
}

bool init()
{
    {

        bool success = true;

        //Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Set texture filtering to linear
            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            {
                printf("Warning: Linear texture filtering not enabled!");
            }

            //Create window
            gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            if (gWindow == NULL)
            {
                printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Create gRenderer for window
                gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
                if (gRenderer == NULL)
                {
                    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                    success = false;
                }
                else
                {
                    //Initialize renderer color
                    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

                    //Initialize PNG loading
                    int imgFlags = IMG_INIT_PNG;
                    if (!(IMG_Init(imgFlags) & imgFlags))
                    {
                        printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
                        success = false;
                    }
                }
            }
        }

        gTexture = NULL;
        return success;
    }
}

void drawBoard()
{
    //   SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 255);
    loadTexture("/Users/aliabdulkareem/dev/chess/res/ChessBoard.png");

    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
}

void drawPiece(char const* piece, vector2d pos)
{

    SDL_Rect piecePos { static_cast<int>(pos.x), static_cast<int>(pos.y), 60, 60 };
    loadTexture(piece);
    SDL_RenderCopy(gRenderer, gTexture, NULL, &piecePos);
}

void loadTexture(char const* path)
{
    SDL_Surface* tempSurface = nullptr;
    tempSurface              = IMG_Load(path);
    if (tempSurface == nullptr)
    {
        std::cout << "ERROR LOADING SURFACE " << SDL_GetError() << std::endl;
        std::terminate();
    }
    else
    {
        gTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
        if (gTexture == nullptr)
        {
            std::cout << "ERROR Creating Texture " << SDL_GetError() << std::endl;
        }

        else
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = nullptr;
        }
    }
}

void render()
{
    SDL_RenderPresent(gRenderer);
}

int clamp(float pos)
{
    return 1;
}