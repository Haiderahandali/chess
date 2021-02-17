#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <string>
//------------------------------------------------------ GLOBAL VARIABLE STARTS --------------------------------------------------//
//------------------------------------------------------ GLOBAL VARIABLE STARTS --------------------------------------------------//
std::map<char const*, SDL_Texture*> texturesMap;

const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
SDL_Texture* gTexture      = NULL;
SDL_Renderer* gRenderer    = NULL;
SDL_Window* gWindow        = NULL;

bool quit = false;

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 640;
const int SCREEN_OFFSET = 25;
const int SQUARE_WIDTH  = 75;

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
    int currentSqaure;

    bool selected = false;

    void snapPosition(int const x = SQUARE_WIDTH, int const y = SQUARE_WIDTH, int const offset = 0);
    Vector2d pieceOffset = Vector2d { this->m_width / 2, this->m_height / 2 };
};
Vector2d operator-(Vector2d const&, Vector2d const);
Vector2d operator+(Vector2d const&, Vector2d const);

Vector2d operator+(Vector2d const&, int const);

Vector2d operator-(Vector2d const&, int const);
Vector2d operator/(Vector2d const&, int const);
Vector2d snapPosition(Vector2d vec, int x = SQUARE_WIDTH, int y = SQUARE_WIDTH, int offset = 0);
Vector2d getMousePosition();

bool onKeyDown(int);
bool onKeyUp(int);

bool init();
bool isLegalPosition(Piece p, Vector2d const& pos);

void loadPieces();
void drawBoard();
void drawPiece(Piece, int offset = 25);

void eventLoop(SDL_Event event);
void render();

SDL_Texture* loadTexture(char const*);

int normalizePosition(Vector2d const&);
int abs(int);
int selectedSquare(Vector2d mousePos);

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
    Piece WhiteQueen("Queen_W", { 0, 0 }, QUEEN);
    auto oldPosition = WhiteQueen.position;

    WhiteQueen.currentSqaure = 0;
    while (!quit)
    {

        drawBoard();

        if (onKeyDown(SPACE))
        {
            if (selectedSquare(getMousePosition()) == WhiteQueen.currentSqaure)
            {

                WhiteQueen.selected = true;
            }
            auto tempPosition   = WhiteQueen.position;
            WhiteQueen.position = getMousePosition() - SCREEN_OFFSET;
            drawPiece(WhiteQueen, 0); // no offset, draw at mouse position
            WhiteQueen.position = tempPosition;
        }
        else
        {

            Vector2d mouse = snapPosition(getMousePosition(), SQUARE_WIDTH, SQUARE_WIDTH, -SCREEN_OFFSET);
            printf("mouse hover at %d & Piece square is %d \n", selectedSquare(getMousePosition()), WhiteQueen.currentSqaure);
            if (isLegalPosition(WhiteQueen, mouse) && WhiteQueen.selected)
            {

                WhiteQueen.position = mouse;

                oldPosition = WhiteQueen.position;
                drawPiece(WhiteQueen);
                WhiteQueen.selected      = false;
                WhiteQueen.currentSqaure = selectedSquare(getMousePosition());
            }

            else
            {
                WhiteQueen.position = oldPosition;
                drawPiece(WhiteQueen);
                WhiteQueen.selected = false;
            }
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

void drawPiece(Piece p, int offset)
{

    SDL_Rect piecePos { p.position.x + offset, p.position.y + offset, p.m_width, p.m_height };
    //off setting the screen width and height;

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
    this->position.x += offset;
    this->position.y += offset;

    this->position.x /= x_snap;
    this->position.y /= y_snap;

    this->position.x *= x_snap;

    this->position.y *= y_snap;
}

Vector2d operator-(Vector2d const& vec, int const other)
{
    return { vec.x - other, vec.y - other };
}
Vector2d operator-(Vector2d const& src, Vector2d const other)
{

    return { src.x - other.x, src.y - other.y };
}

Vector2d operator+(Vector2d const& src, Vector2d const other)
{

    return { src.x + other.x, src.y + other.y };
}

Vector2d operator+(Vector2d const& src, int const other)
{

    return { src.x + other, src.y + other };
}
Vector2d operator/(Vector2d const& vec, int const other)
{
    return { vec.x / other, vec.y / other };
}

Vector2d snapPosition(Vector2d vec, int const x_snap, int const y_snap, int const offset)
{

    vec.y += offset;
    vec.x += offset;

    vec.x /= x_snap;
    vec.y /= y_snap;

    vec.x *= x_snap;
    vec.y *= y_snap;

    return vec;
}

int normalizePosition(Vector2d const& pos)
{
    return pos.y * 8 + pos.x;
}
int abs(int num)
{
    if (num > -1)
        return num;
    else
        return (num * -1);
}

bool isLegalPosition(Piece p, Vector2d const& pos)
{
    bool legal = false;

    int x = (p.position.x - pos.x) / SQUARE_WIDTH;
    int y = (p.position.y - pos.y) / SQUARE_WIDTH;

    y = abs(y);
    x = abs(x);

    switch (p.type)
    {
    case KNIGHT:

        if ((y == 2 && x == 1) || (y == 1 && x == 2))
            legal = true;
        break;

    case ROOK:
        if (y == 0 || x == 0)
            legal = true;
        break;

    case BISHOP:
        if (y == x)
            legal = true;
        break;

    case QUEEN:
        if (x == y || x == 0 || y == 0)
            legal = true;
        break;

    case KING:
        if (y <= 1 && x <= 1)
            legal = true;
        break;

    default:
        break;
    }
    return legal;
}

int selectedSquare(Vector2d mousePos)
{
    mousePos = mousePos - SCREEN_OFFSET;
    mousePos = mousePos / SQUARE_WIDTH;
    return normalizePosition(mousePos);
}

/*



















































































*/
