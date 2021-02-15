#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <string>
//------------------------------------------------------ GLOBAL VARIABLE STARTS --------------------------------------------------//
//------------------------------------------------------ GLOBAL VARIABLE STARTS --------------------------------------------------//
std::map<char const*, SDL_Texture*> texturesMap;
int arr[8][8];

const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
SDL_Texture* gTexture      = NULL;
SDL_Renderer* gRenderer    = NULL;
SDL_Window* gWindow        = NULL;

bool quit = false;

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 640;

#define SPACE SDL_SCANCODE_SPACE
#define WHITE 1
#define BLACK 0
#define Color int
#define Type int

enum ChessClass
{
    KNIGHT,
    QUEEN,
    KING,
    PAWN,
    BISHOP,
    ROOK

};
//------------------------------------------------------ GLOBAL VARIABLE ENDS --------------------------------------------------//
//------------------------------------------------------ GLOBAL VARIABLE ENDS --------------------------------------------------//

struct Vector2d
{
    int x = 0;
    int y = 0;
};

struct Piece
{
    Piece(char const* name, Vector2d pos, Type type, Color color = 1)
        : name { name }
        , position { pos }
        , type { type }
        , color { color }

    {
    }
    Color color; //white or black
    Type type; // king, knight, queen, etc.
    char const* name;
    SDL_Texture* tex;
    Vector2d position;
    int m_width  = 60;
    int m_height = 60;

    void snapPosition(int const x = 75, int const y = 75, int const offset = 25);
    Vector2d pieceOffset = Vector2d { this->m_width / 2, this->m_height / 2 };
};
Vector2d operator-(Vector2d const&, Vector2d const);
Vector2d snapPosition(Vector2d vec, int x = 75, int y = 75, int offset = 25);
Vector2d getMousePosition();

bool onKeyDown(int);
bool onKeyUp(int);

bool init();

void loadPieces();
void drawBoard();
void drawPiece(Piece);

void eventLoop(SDL_Event event);
void render();

SDL_Texture* loadTexture(char const*);

int normalizePosition(Vector2d const&);

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
    Piece WhiteKnight("Night_W", { 0, 0 }, KNIGHT);

    while (!quit)
    {

        drawBoard();

        if (onKeyDown(SPACE))
        {
            WhiteKnight.position = getMousePosition();
            WhiteKnight.position = WhiteKnight.position - WhiteKnight.pieceOffset;
            drawPiece(WhiteKnight);
            ;
        }
        else
        {
            WhiteKnight.snapPosition();
            drawPiece(WhiteKnight);
            printf("%d\n", normalizePosition(WhiteKnight.position));
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

Vector2d operator-(Vector2d const& src, Vector2d const other)
{

    return { src.x - other.x, src.y - other.y };
}

Vector2d snapPosition(Vector2d vec, int const x_snap, int const y_snap, int const offset)
{

    vec.x /= x_snap;
    vec.y /= y_snap;

    vec.x *= x_snap;
    vec.x += offset;

    vec.y *= y_snap;
    vec.y += offset;
    return vec;
}

int normalizePosition(Vector2d const& pos)
{
    return pos.x * 8 + pos.y;
}
/*















































































*/
