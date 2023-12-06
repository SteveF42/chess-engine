#include <iostream>
#include "catch_amalgamated.hpp"
#include "../AI.hpp"
#include "../Board.hpp"
#include "../ReadFen.hpp"

TEST_CASE("Zobrist key generation", "[Zobrist]")
{
    Board *board = ReadFen::readFenString(ReadFen::startingString);
    // pawn moves
    SECTION("pawn moves")
    {
        // 1 pawn move
        Move move(50, 42, Piece::PAWN);
        board->makeMove(move);
        REQUIRE(board->getZobristKey() == board->zobrist.generateZobristKey(board));
        // 2 move forward
        move = Move(11, 26, Piece::PAWN);
        board->makeMove(move);
        REQUIRE(board->getZobristKey() == board->zobrist.generateZobristKey(board));
    }
    delete board;
}

TEST_CASE("AI move depth", "[AI]")
{
    SECTION("Starting board position")
    {
        Board *board = ReadFen::readFenString(ReadFen::startingString);
        AI ai(board);

        REQUIRE(ai.moveGenerationTest(1) == 20);
        REQUIRE(ai.moveGenerationTest(2) == 400);
        REQUIRE(ai.moveGenerationTest(3) == 8902);
        REQUIRE(ai.moveGenerationTest(4) == 197281);
        REQUIRE(ai.moveGenerationTest(5) == 4865609);
        REQUIRE(ai.moveGenerationTest(6) == 119060324);
        delete board;
    }

    SECTION("Tricky chess position")
    {
        Board *board = ReadFen::readFenString("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
        AI ai(board);
        REQUIRE(ai.moveGenerationTest(1) == 44);
        REQUIRE(ai.moveGenerationTest(2) == 1486);
        REQUIRE(ai.moveGenerationTest(3) == 62379);
        REQUIRE(ai.moveGenerationTest(4) == 2103487);
        REQUIRE(ai.moveGenerationTest(5) == 89941194);
    }
}

// static int Factorial(int number)
// {
//     return number <= 1 ? number : Factorial(number - 1) * number; // fail
//     // return number <= 1 ? 1      : Factorial( number - 1 ) * number;  // pass
// }

// TEST_CASE("Factorial of 0 is 1 (fail)", "[single-file]")
// {
//     REQUIRE(Factorial(0) == 1);
// }

// TEST_CASE("Factorials of 1 and higher are computed (pass)", "[single-file]")
// {
//     REQUIRE(Factorial(1) == 1);
//     REQUIRE(Factorial(2) == 2);
//     REQUIRE(Factorial(3) == 6);
//     REQUIRE(Factorial(10) == 3628800);
// }