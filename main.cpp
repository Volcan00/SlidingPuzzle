#include <iostream>
#include <limits>
#include<cassert>
#include "Random.h"

// Increase amount of new lines if your board isn't
// at the very bottom of the console
constexpr int g_consoleLines { 25 };

class Direction
{
public:
    enum Type
    {
        up,
        down,
        left,
        right,
        max_directions,
    };

    Direction (Type type)
        : m_type { type }
    {
    }

    Type getType() const
    {
        return m_type;
    }

    Direction operator-() const
    {
        switch (m_type)
        {
        case up    : return Direction{ down };
        case down  : return Direction{ up };
        case left  : return Direction{ right };
        case right : return Direction{ left };
        
        default:
            assert(0 && "Uknown direction was passed!");
            return Direction { up };
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Direction& dir)
    {
        switch (dir.getType())
        {
        case Direction::up    : return (out << "up");
        case Direction::down  : return (out << "down");
        case Direction::left  : return (out << "left");
        case Direction::right : return (out << "right");
        default               : return (out << "unknown direction");
        }
    }

    static Direction getRandomDirection()
    {
        Type random{ static_cast<Type>(Random::get(0, Type::max_directions - 1)) };
        return Direction{ random };
    }

private:
    Type m_type {};
};

struct Point
{
    int x{};
    int y{};

    friend bool operator==(Point p1, Point p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }

    friend bool operator!=(Point p1, Point p2)
    {
        return !(p1 == p2);
    }

    Point getAdjacentPoint(Direction dir) const
    {
        switch (dir.getType())
        {
        case Direction::up:     return Point{ x,     y - 1 };
        case Direction::down:   return Point{ x,     y + 1 };
        case Direction::left:   return Point{ x - 1, y };
        case Direction::right:  return Point{ x + 1, y };
        }

        return *this;
    }
};

class Tile
{
public:
    Tile() = default; // Default constructor

    //Constructor with a parameter to initialize the tile number 
    explicit Tile(int num)
        : m_tileNum { num }
    {

    } 

    //Getter for tile value
    int getNum() const
    {
        return m_tileNum;
    }

    void setNum(int value)
    {
        m_tileNum = value;
    }

    //Check if tile is empty
    bool isEmpty() const
    {
        return m_tileNum == 0;
    }

    //Overloaded output operator
    friend std::ostream& operator<<(std::ostream& out, const Tile& tile)
    {
        if(tile.m_tileNum == 0) // if empty spot
            out << "    ";
        else if(tile.m_tileNum > 9) // if two digit number
            out << " " << tile.m_tileNum << " ";
        else// if one digit number
            out << "  " << tile.m_tileNum << " ";

        return out;
    }

private:
    int m_tileNum {};
    Point m_point {};
};

class Board
{
public:
    //Constructor to initialize the board
    Board()
    {
        int defaultValue = 1;

        //Initialize a grid of tiles from 1 to 15 and an empty tile
        for(int i = 0; i < SIZE; ++i)
        {
            for(int j = 0; j < SIZE; ++j)
            {
                if(i == SIZE - 1 && j == SIZE - 1)
                {
                    m_grid[i][j].setNum(0);
                }
                else
                {
                    m_grid[i][j].setNum(defaultValue++);
                }
            }
        }
    }

    static bool isValidPoint(const Point& pt)
    {
        return (pt.x >= 0 && pt.x < SIZE)
            && (pt.y >= 0 && pt.y < SIZE);
    }

    Point findEmptyTile() const
    {
        for(int i = 0; i < SIZE; ++i)
            for(int j = 0; j < SIZE; ++j)
                if(m_grid[i][j].isEmpty())
                    return { j, i };

        assert(0 && "There is no empty tile in the board!!!");
        return { -1,-1 };
    }

    void swapTiles(Point pt1, Point pt2)
    {
        std::swap(m_grid[pt1.y][pt1.x], m_grid[pt2.y][pt2.x]);
    }

    bool moveTile(Direction dir)
    {
        Point emptyTile{ findEmptyTile() };
        Point adj{ emptyTile.getAdjacentPoint(-dir) };

        if(!isValidPoint(adj))
            return false;

        swapTiles(emptyTile, adj);
        return true;
    }

    static void printEmptyLines(int count)
    {
        for(int i = 0; i < count; ++i)
        {
            std::cout << '\n';
        }
    }
    
    void randomize()
    {
        // just move empty tile randomly 1000 times
        // (just like you would do in real life)
        for (int i = 0; i < 1000; ++i)
        {
            bool success = moveTile(Direction::getRandomDirection());
            // If we tried to move in invalid direction, don't count this iteration
            if (!success)
                --i;
        }
    }

    //Overloaded output operator for the board
    friend std::ostream& operator<<(std::ostream& out, Board& board)
    {
        // Before drawing always print some empty lines
        // so that only one board appears at a time
        // and it's always shown at the bottom of the window
        // because console window scrolls automatically when there is no
        // enough space.
        printEmptyLines(g_consoleLines);

        for(int i = 0; i < SIZE; ++i)
        {
            for(int j = 0; j < SIZE; ++j)
            {
                out << board.m_grid[i][j];
            }

            out << '\n';
        }

        return out;
    }

    friend bool operator==(const Board& board1, const Board& board2)
    {
        for(int i = 0; i < SIZE; ++i)
        {
            for(int j = 0; j < SIZE; ++j)
            {
                if(board1.m_grid[i][j].getNum() != board2.m_grid[i][j].getNum())
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool playerWon() const
    {
        Board s_solved {}; // generate a solved board
        return s_solved == *this; // player wins if current board == solved board
    }

private:
    static constexpr int SIZE { 4 };
    Tile m_grid[SIZE][SIZE] {};
};

namespace UserInput
{
    bool isValidCommand(char ch)
    {
        return ch == 'w'
            || ch == 'a'
            || ch == 's'
            || ch == 'd'
            || ch == 'q';
    }

    void ignoreLine()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    char getCharacter()
    {
        char operation {};
        std::cin >> operation;
        ignoreLine(); // remove any extraneous input
        return operation;
    }

    char getCommandFromUser()
    {
        char ch {};
        while (!isValidCommand(ch))
            ch = getCharacter();
        
        return ch;
    }

    Direction charToDirection(char ch)
    {
        switch (ch)
        {
        case 'w' : return Direction{ Direction::up };
        case 's' : return Direction{ Direction::down };
        case 'a' : return Direction{ Direction::left };
        case 'd' : return Direction{ Direction::right };
        default:
            assert(0 && "Unsupported direction was passed!");
            return Direction{ Direction::up };
        }
    }
};

int main()
{
    Board board {};
    board.randomize();
    std::cout << board;

    std::cout << "Enter a command: ";
    while (!board.playerWon())
    {
        char ch { UserInput::getCommandFromUser() };

        if(ch == 'q')
        {
            std::cout << "\n\nBye!\n\n";
            break;
        }

        Direction dir { UserInput::charToDirection(ch) };

        bool userMoved { board.moveTile(dir) };
        if(userMoved)
            std::cout << board;
    }

    std::cout << "\n\nYou won\n\n";

    return 0;
}