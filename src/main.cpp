#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <string>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 640;

//------- Aliasing SDL_SCANCODE -----------//
#define SPACE SDL_SCANCODE_SPACE

std::map<char const*, SDL_Texture*> texturesMap;

struct Vector2d
{
    int x = 0;
    int y = 0;
};

struct Piece
{
    Piece(char const* name, Vector2d pos)
        : name { name }
        , position { pos }
    {
    }
    char const* name;
    SDL_Texture* tex;
    Vector2d position;
    int m_width  = 60;
    int m_height = 60;

    void snapPosition(int x = 75, int y = 75, int offset = 25);
};

bool onKeyDown(int);
bool onKeyUp(int);

bool init();

void loadPieces();
void drawBoard();
void drawPiece(Piece);

void eventLoop(SDL_Event event);
void render();

Vector2d getMousePosition();

SDL_Texture* loadTexture(char const*);

const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
SDL_Texture* gTexture      = NULL;
SDL_Renderer* gRenderer    = NULL;
SDL_Window* gWindow        = NULL;

bool quit = false;

int main(int argc, char** argv)
{
    if (init())
    {
        std::cout << "SDL initialize is done"
                  << "\n"
                  << argc << argv[0];
        loadPieces();
    }

    SDL_Event event;
    Piece WhiteKnight("Night_W", { 0, 0 });

    while (!quit)
    {

        drawBoard();

        if (onKeyDown(SPACE))
        {
            WhiteKnight.position = getMousePosition();
            drawPiece(WhiteKnight);
        }
        else
        {
            WhiteKnight.snapPosition();
            drawPiece(WhiteKnight);
        }

        render();
        eventLoop(event);
    }

    IMG_Quit();
    SDL_Quit();

    printf("\n%s\n", SDL_GetError());
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
    gTexture = loadTexture("/Users/aliabdulkareem/dev/chess/res/ChessBoard.png");

    SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
}

void drawPiece(Piece p)
{

    SDL_Rect piecePos { p.position.x, p.position.y, p.m_width, p.m_height };

    auto pieceTexture = texturesMap[p.name];
    SDL_RenderCopy(gRenderer, pieceTexture, NULL, &piecePos);
}

SDL_Texture* loadTexture(char const* path)
{
    SDL_Texture* someTexture;

    SDL_Surface* tempSurface = nullptr;
    tempSurface              = IMG_Load(path);
    if (tempSurface == nullptr)
    {
        std::cout << "ERROR LOADING SURFACE " << SDL_GetError() << std::endl;
        std::terminate();
    }
    else
    {
        someTexture = SDL_CreateTextureFromSurface(gRenderer, tempSurface);
        if (someTexture == nullptr)
        {
            std::cout << "ERROR Creating Texture " << SDL_GetError() << std::endl;
        }

        else
        {
            SDL_FreeSurface(tempSurface);
            tempSurface = nullptr;
        }
    }

    return someTexture;
}

void render()
{
    SDL_RenderPresent(gRenderer);
}

int clamp(float pos)
{
    return 1;
}
void loadPieces()
{
    SDL_Texture* temp = nullptr;

    //------------------------ Black  Loading--------------------------------//
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackBishop.png");
    texturesMap.insert({ "Bishop_B", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackNight.png");
    texturesMap.insert({ "Night_B", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackQueen.png");
    texturesMap.insert({ "Queen_B", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackKing.png");
    texturesMap.insert({ "King_B", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackRook.png");
    texturesMap.insert({ "Rook_B", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/BlackPawn.png");
    texturesMap.insert({ "Pawn_B", temp });

    //------------------------ White  Loading--------------------------------//
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhiteBishop.png");
    texturesMap.insert({ "Bishop_W", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhiteNight.png");
    texturesMap.insert({ "Night_W", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhiteQueen.png");
    texturesMap.insert({ "Queen_W", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhiteKing.png");
    texturesMap.insert({ "King_W", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhiteRook.png");
    texturesMap.insert({ "Rook_W", temp });
    temp = loadTexture("/Users/aliabdulkareem/dev/chess/res/WhitePawn.png");
    texturesMap.insert({ "Pawn_W", temp });

    temp = nullptr;
}

bool onKeyDown(int key)
{
    return keyboardState[key];
}

bool onKeyUp(int key)
{
    return !keyboardState[key];
}

Vector2d getMousePosition()
{
    int x, y;
    SDL_GetMouseState(&x, &y);
    return { x, y };
}

void Piece::snapPosition(int x_snap, int y_snap, int offset)
{
    this->position.x /= x_snap;
    this->position.y /= y_snap;

    this->position.x *= x_snap;
    this->position.x += offset;

    this->position.y *= y_snap;
    this->position.y += offset;
}

/*















































































*/
