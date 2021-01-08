#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <deque>
#include <algorithm>

#define SWIDTH 800
#define SHEIGHT 800

#define BWIDTH 6
#define BHEIGHT 4

#define CLOSETM .5f

#define MATCHCNT 2

using namespace std;
using namespace sf;

const int GWIDTH = SWIDTH / BWIDTH;
const int GHEIGHT = SHEIGHT / BHEIGHT;

typedef enum class Elements { Red, Green, Blue, Pink, Yellow, Cyan, Purple, White, Skyblue, Orange, Brown, Grass, Hidden, Removed } elements;
Color color[BWIDTH * BHEIGHT / MATCHCNT + 2] = { Color::Red, Color::Green, Color::Blue, Color(0xffc0cbff), Color::Yellow, Color::Cyan, Color::Magenta, 
Color::White, Color(0x87ceebff), Color(0xffa500ff), Color(0xa52a2aff), Color(0xadff2fff), Color(0x000000ff), Color(0xffffff7f) };

class Board
{
private:
    vector<vector<Elements>> board;
    vector<vector<bool>> opened;
    int openedCnt;
    Clock closeClock;
    deque<pair<int, int>> openedStack;
    bool won;

    void update()
    {
        if (closeClock.getElapsedTime().asSeconds() > CLOSETM && openedCnt == MATCHCNT)
        {
            if (openedCnt == MATCHCNT) // detect if all of selected squares are same
            {
                bool result = true;
                auto lastElem = board[openedStack.back().first][openedStack.back().second];
                for (auto& elem : openedStack)
                {
                    result = result && board[elem.first][elem.second] == lastElem;
                }
                if (result)
                {
                    while (!openedStack.empty())
                    {
                        board[openedStack.back().first][openedStack.back().second] = Elements::Removed;
                        openedStack.pop_back();
                    }
                }

                won = true;
                for (int X = 0;X < BWIDTH;X++)
                {
                    for (int Y = 0;Y < BHEIGHT;Y++)
                    {
                        won = won && board[X][Y] == Elements::Removed;
                    }
                }
            }

            opened = vector<vector<bool>>(BWIDTH, vector<bool>(BHEIGHT, false));
            openedStack = deque<pair<int, int>>();
            openedCnt = 0;
        }
    }

public:
    Board() : board(vector<vector<Elements>>(BWIDTH, vector<Elements>(BHEIGHT, Elements::Red))), 
        opened(vector<vector<bool>>(BWIDTH, vector<bool>(BHEIGHT, false))), openedCnt(0), won(false)
    {
        vector<Elements> elements = {Elements::Red, Elements::Green, Elements::Blue, Elements::Pink, Elements::Yellow, Elements::Cyan, Elements::Purple, 
        Elements::White, Elements::Skyblue, Elements::Orange, Elements::Brown, Elements::Grass};
        for (int i = 1;i < MATCHCNT;i++)
        {
            elements.insert(elements.end(), elements.begin(), elements.end());
        }

        random_shuffle(elements.begin(), elements.end());

        for (int X = 0;X < BWIDTH;X++)
        {
            for (int Y = 0;Y < BHEIGHT; Y++)
            {
                board[X][Y] = elements[X * BHEIGHT + Y];
            }
        }
    }

    void open(int X, int Y)
    {
        update();

        if (openedCnt < MATCHCNT)
        {
            if (!opened[X][Y])
            {
                opened[X][Y] = true;
                openedStack.push_back(make_pair(X, Y));
                openedCnt++;
                closeClock.restart();
            }
        }
    }
    vector<vector<Elements>> getBoard()
    {
        update();
        vector<vector<Elements>> result = vector<vector<Elements>>(BWIDTH, vector<Elements>(BHEIGHT, Elements::Hidden));
        for (int X = 0;X < BWIDTH;X++)
        {
            for (int Y = 0;Y < BHEIGHT; Y++)
            {
                if (opened[X][Y] || board[X][Y] == Elements::Removed)
                {
                    result[X][Y] = board[X][Y];
                }
            }
        }
        return result;
    }
    FloatRect getGlobalBounds(int X, int Y)
    {
        return FloatRect(GWIDTH * X, GHEIGHT * Y, GWIDTH, GHEIGHT);
    }

    bool win()
    {
        return won;
    }
};

int main()
{
    unsigned int t = GetTickCount64();
    srand(t);

    RenderWindow window(VideoMode(SWIDTH, SHEIGHT), L"크고♂아름다운♂게이ㅁ");

    Event event;

    Clock clock;

    Board board;
    vector<vector<Elements>> toDraw;
    RectangleShape rect(Vector2f(GWIDTH, GHEIGHT));
    rect.setOutlineColor(Color(0x054159ff));
    rect.setOutlineThickness(5);

    Vector2f mousePos;

    auto reset = [&]()
    {
        board = Board();
    };

    while (window.isOpen()) 
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case Event::Closed:
                window.close();
                break;
            case Event::MouseMoved:
                mousePos = static_cast<Vector2f>(Mouse::getPosition(window));
                break;
            case Event::MouseButtonPressed:
                switch (event.mouseButton.button)
                {
                case Mouse::Button::Left:
                    for (int X = 0;X < BWIDTH;X++)
                    {
                        for (int Y = 0;Y < BHEIGHT;Y++)
                        {
                            if (board.getGlobalBounds(X, Y).contains(mousePos))
                            {
                                board.open(X, Y);
                            }
                        }
                    }
                    break;
                }
                break;
            }
        }

        toDraw = board.getBoard();

        if (board.win())
        {
            reset();
        }

        window.clear();

        for (int X = 0; X < BWIDTH; X++)
        {
            for (int Y = 0;Y < BHEIGHT; Y++)
            {
                rect.setPosition(X * GWIDTH, Y * GHEIGHT);
                rect.setFillColor(color[static_cast<int>(toDraw[X][Y])]);
                window.draw(rect);
            }
        }

        window.display();
    }
    return 0;
}