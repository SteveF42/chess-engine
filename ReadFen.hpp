#ifndef ReadFen_H
#define ReadFen_H
#include <string>
#include <map>

class ReadFen
{
public:
    static const std::string startingString;
    static struct Position readFenString(const std::string fen);
};
#endif