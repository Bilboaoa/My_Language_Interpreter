#pragma once

class Position
{
   public:
    Position() : column(0), line(1) {}
    Position(const int lin, const int col) : column(col), line(lin) {}
    Position(const Position& p) : column(p.column), line(p.line) {}

    bool operator==(const Position& other) const
    {
        return other.column == column && other.line == line;
    }

    Position& operator=(Position&& other) noexcept
    {
        if (this != &other)
        {
            column = other.column;
            line = other.line;
        }
        return *this;
    }

    int column, line;
};
