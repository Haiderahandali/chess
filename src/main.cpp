#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
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

enum SELECTED
{
    EMPTY,
    SAME_COLOR,
    OPPOSITE_COLOR
};

enum COLOR
{
    BLACK,
    WHITE

};
enum PIECE_TYPE
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
    Piece(char const* name, Vector2d pos, PIECE_TYPE type, COLOR color)
        : color { color }
        , type { type }
        , name { name }
        , position { pos }

    {
    }
    COLOR color; //white or black
    PIECE_TYPE type; // king, knight, queen, etc.
    char const* name;
    SDL_Texture* tex;
    Vector2d position;
    int m_width  = 60;
    int m_height = 60;
    int currentSqaure;

    bool selected = false;

    void snapPosition(int const x = SQUARE_WIDTH, int const y = SQUARE_WIDTH, int const offset = 0);
    Vector2d pieceOffset = Vector2d { this->m_width / 2, this->m_height / 2 };

    SDL_RendererFlip flip()
    {
        SDL_RendererFlip f = SDL_FLIP_NONE;

        if (this->color == WHITE)
            f = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        else
            f = SDL_FLIP_NONE;

        return f;
    }
};
Vector2d operator-(Vector2d const&, Vector2d const);
Vector2d operator+(Vector2d const&, Vector2d const);

Vector2d operator+(Vector2d const&, int const);

Vector2d operator-(Vector2d const&, int const);
Vector2d operator/(Vector2d const&, int const);
Vector2d operator*(Vector2d const&, int const);
Vector2d snapPosition(Vector2d vec, int x = SQUARE_WIDTH, int y = SQUARE_WIDTH, int offset = 0);
Vector2d getMousePosition();
Vector2d getPositionFromSquare(int square);

bool onKeyDown(int);
bool onKeyUp(int);

bool init();
bool isLegalPosition(Piece p, Vector2d const& pos);
bool islegalMove(std::unique_ptr<Piece>& src, Vector2d dest, std::vector<std::unique_ptr<Piece>>& v);
void DrawPieces(std::vector<std::unique_ptr<Piece>>&);
void loadPieces();
void drawBoard();
void drawPiece(Piece, int offset = 25);

void eventLoop(SDL_Event event);
void render();

void LoadStartPosition(std::vector<std::unique_ptr<Piece>>&);

SDL_Texture* loadTexture(char const*);

int normalizePosition(Vector2d const&);
int abs(int);
inline int max(int x, int y)
{
    return x > y ? x : y;
}
int selectedSquare(Vector2d mousePos);

int main(int argc, char** argv)
{
    // bool selected  = false;
    int destSquare = 0;
    int srcSquare  = 63;

    //    auto selectedPiece = EMPTY;

    bool WhiteTurn = true;
    std::vector<std::unique_ptr<Piece>> ChessPieces;
    ChessPieces.resize(64);

    for (int i = 0; i < 64; ++i)
    {
        ChessPieces[i] = nullptr;
    }
    ChessPieces[0]  = std::make_unique<Piece>(Piece { "Queen_B", { 0, 0 }, QUEEN, BLACK });
    ChessPieces[63] = std::make_unique<Piece>(Piece { "Night_W", { 525, 525 }, KNIGHT, WHITE });

    ChessPieces[63]->currentSqaure = 63;
    ChessPieces[0]->currentSqaure  = 0;

    if (init())
    {
        std::cout << "SDL initialize is done"
                  << "\n"
                  << argc << argv[0];
        loadPieces();
    }

    SDL_Event event;

    while (!quit)
    {
        drawBoard();

        if (onKeyDown(SDL_SCANCODE_UP))
        {
            LoadStartPosition(ChessPieces);
            WhiteTurn  = false;
            srcSquare  = 56;
            destSquare = 56;
        }
        if (onKeyDown(SPACE))
        {
            DrawPieces(ChessPieces);
            if (ChessPieces[destSquare] != nullptr)
            {
                //clicked the same color piece
                if (ChessPieces[destSquare]->color == WhiteTurn)
                {
                    ChessPieces[srcSquare]->selected  = false;
                    ChessPieces[destSquare]->selected = true;

                    srcSquare = destSquare;
                }
                else if (ChessPieces[destSquare]->color != WhiteTurn) //clicked an enemy piece

                {
                    if (ChessPieces[srcSquare]->selected
                        && ChessPieces[srcSquare]->color == WhiteTurn
                        && isLegalPosition(*ChessPieces[srcSquare], getPositionFromSquare(destSquare))
                        && islegalMove(ChessPieces[srcSquare], getPositionFromSquare(destSquare), ChessPieces))
                    {

                        WhiteTurn                        = !WhiteTurn;
                        ChessPieces[srcSquare]->position = getPositionFromSquare(destSquare);

                        ChessPieces[srcSquare]->currentSqaure = selectedSquare(getMousePosition());
                        ChessPieces[destSquare]               = std::move(ChessPieces[srcSquare]);
                        ChessPieces[srcSquare]                = nullptr;
                        srcSquare                             = destSquare;
                        ChessPieces[srcSquare]->selected      = false;
                    }
                }
            }
            else if (ChessPieces[destSquare] == nullptr) //clicked an empty square
            {
                if (ChessPieces[srcSquare]->selected
                    && ChessPieces[srcSquare]->color == WhiteTurn
                    && isLegalPosition(*ChessPieces[srcSquare], getPositionFromSquare(destSquare))
                    && islegalMove(ChessPieces[srcSquare], getPositionFromSquare(destSquare), ChessPieces))
                {

                    WhiteTurn                        = !WhiteTurn;
                    ChessPieces[srcSquare]->position = getPositionFromSquare(destSquare);

                    ChessPieces[srcSquare]->currentSqaure = selectedSquare(getMousePosition());
                    ChessPieces[destSquare]               = std::move(ChessPieces[srcSquare]);
                    ChessPieces[srcSquare]                = nullptr;
                    srcSquare                             = destSquare;
                    ChessPieces[srcSquare]->selected      = false;
                }
            }
        }
        else
        {
            destSquare = selectedSquare(getMousePosition());
            DrawPieces(ChessPieces);
            for (auto& x : ChessPieces)
            {
                if (x != nullptr)
                {
                    printf(" piece at %d %d , Selected =%d , src =%d , dest =%d & currentSqaure =%d \n", x->position.x, x->position.y, x->selected, srcSquare, destSquare, x->currentSqaure);
                }
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

    // int onSquare = p.currentSqaure;

    // auto pos = getPositionFromSquare(onSquare);
    //printf("Location = %d %d & Position %d , %d =  \n", pos.x, pos.y, p.position.x, p.position.y);
    SDL_Rect piecePos { p.position.x + offset, p.position.y + offset, p.m_width, p.m_height };

    //off setting the screen width and height;

    auto pieceTexture = texturesMap[p.name];
    SDL_RenderCopyEx(gRenderer, pieceTexture, NULL, &piecePos, 0, 0, p.flip());
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
    return static_cast<int>(pos);
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

Vector2d operator*(Vector2d const& vec, int const other)
{
    return { vec.x * other, vec.y * other };
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
    case PAWN:
        if (y == 1 && x == 0)
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

Vector2d getPositionFromSquare(int square)
{
    return { square % 8 * SQUARE_WIDTH, square / 8 * SQUARE_WIDTH };
}
void DrawPieces(std::vector<std::unique_ptr<Piece>>& pieces)
{
    for (auto& x : pieces)
    {
        if (x == nullptr)
            continue;
        else
        {
            drawPiece(*x);
        }
    }
}

void LoadStartPosition(std::vector<std::unique_ptr<Piece>>& v)
{
    v.clear();
    v.resize(64);
    for (auto& x : v)
        x = nullptr;

    //--------------------- LOAD BLACK PIECES------------------

    for (int i = 0; i < 8; ++i)
    {
        v[48 + i] = std::make_unique<Piece>(Piece { "Pawn_B", { i * SQUARE_WIDTH, 450 }, PAWN, BLACK });

        v[48 + i]->currentSqaure = 48 + i;
    }

    v[56] = std::make_unique<Piece>(Piece { "Rook_B", { 0, 525 }, ROOK, BLACK });
    v[57] = std::make_unique<Piece>(Piece { "Night_B", { 75, 525 }, KNIGHT, BLACK });
    v[58] = std::make_unique<Piece>(Piece { "Bishop_B", { 150, 525 }, BISHOP, BLACK });

    v[59] = std::make_unique<Piece>(Piece { "King_B", { 225, 525 }, KING, BLACK });
    v[60] = std::make_unique<Piece>(Piece { "Queen_B", { 300, 525 }, QUEEN, BLACK });

    v[61] = std::make_unique<Piece>(Piece { "Bishop_B", { 375, 525 }, BISHOP, BLACK });
    v[62] = std::make_unique<Piece>(Piece { "Night_B", { 450, 525 }, KNIGHT, BLACK });
    v[63] = std::make_unique<Piece>(Piece { "Rook_B", { 525, 525 }, ROOK, BLACK });

    v[56]->currentSqaure = 56;
    v[57]->currentSqaure = 57;
    v[58]->currentSqaure = 58;
    v[59]->currentSqaure = 59;
    v[60]->currentSqaure = 60;
    v[61]->currentSqaure = 61;
    v[62]->currentSqaure = 62;
    v[63]->currentSqaure = 63;

    //------------------------- LOAD WHITE PIECES --------------------------//

    for (int i = 0; i < 8; ++i)
    {
        v[8 + i] = std::make_unique<Piece>(Piece { "Pawn_W", { i * SQUARE_WIDTH, 75 }, PAWN, WHITE });

        v[8 + i]->currentSqaure = 8 + i;
    }

    v[0] = std::make_unique<Piece>(Piece { "Rook_W", { 0, 0 }, ROOK, WHITE });
    v[1] = std::make_unique<Piece>(Piece { "Night_W", { 75, 0 }, KNIGHT, WHITE });
    v[2] = std::make_unique<Piece>(Piece { "Bishop_W", { 150, 0 }, BISHOP, WHITE });

    v[3] = std::make_unique<Piece>(Piece { "King_W", { 225, 0 }, KING, WHITE });
    v[4] = std::make_unique<Piece>(Piece { "Queen_W", { 300, 0 }, QUEEN, WHITE });

    v[5] = std::make_unique<Piece>(Piece { "Bishop_W", { 375, 0 }, BISHOP, WHITE });
    v[6] = std::make_unique<Piece>(Piece { "Night_W", { 450, 0 }, KNIGHT, WHITE });
    v[7] = std::make_unique<Piece>(Piece { "Rook_W", { 525, 0 }, ROOK, WHITE });

    v[0]->currentSqaure = 0;
    v[1]->currentSqaure = 1;
    v[2]->currentSqaure = 2;
    v[3]->currentSqaure = 3;
    v[4]->currentSqaure = 4;
    v[5]->currentSqaure = 5;
    v[6]->currentSqaure = 6;
    v[7]->currentSqaure = 7;
}

bool islegalMove(std::unique_ptr<Piece>& src, Vector2d dest, std::vector<std::unique_ptr<Piece>>& v)
{
    bool legal = false;

    switch (src->type)
    {
    case KNIGHT: {
        legal = true;
        break;
    }

    case BISHOP:
    case ROOK:
    case QUEEN: {
        int x = dest.x - src->position.x;
        int y = dest.y - src->position.y;

        auto numSqaures = max(abs(x / SQUARE_WIDTH), abs(y / SQUARE_WIDTH));
        for (int i = 1; i < numSqaures; ++i)
        {
            Vector2d nextSquare = { src->position.x + x * i / (numSqaures), src->position.y + y * i / (numSqaures) };
            nextSquare          = nextSquare + SCREEN_OFFSET;
            auto square         = selectedSquare(nextSquare);
            if (v[square] != nullptr)
            {
                return false;
                break;
            }
        }
        legal = true;
        break;
    }

    default:
        legal = true;
        break;
    }
    return legal;
}
/*





  if (ChessPieces[srcSquare]->selected && ChessPieces[srcSquare]->color == WhiteTurn && isLegalPosition(*ChessPieces[srcSquare], getPositionFromSquare(destSquare)))
                {

                    WhiteTurn                        = !WhiteTurn;
                    ChessPieces[srcSquare]->position = getPositionFromSquare(destSquare);

                    ChessPieces[srcSquare]->currentSqaure = selectedSquare(getMousePosition());
                    ChessPieces[destSquare]               = std::move(ChessPieces[srcSquare]);
                    ChessPieces[srcSquare]                = nullptr;
                    srcSquare                             = destSquare;
                    ChessPieces[srcSquare]->selected      = false;
                }
            }













































































*/
