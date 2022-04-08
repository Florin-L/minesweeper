// clang-format off
#include "pch.h"
#include "boardgenerator.hpp"

// clang-format on
static const unsigned int kMaxSize = 25;

void BoardGenerator::generate() {
  std::vector<Coord> minesCoords = generateMinesPositions();

  for (auto& c : minesCoords) {
    std::size_t row = c.row;
    std::size_t col = c.col;

    // W
    if (isValid(c.row, c.col - 1)) {
      _tiles[row * _size + col - 1] += 1;
    }

    // N-W
    if (isValid(c.row - 1, c.col - 1)) {
      _tiles[(row - 1) * _size + col - 1] += 1;
    }

    // N
    if (isValid(c.row - 1, c.col)) {
      _tiles[(row - 1) * _size + col] += 1;
    }

    // N-E
    if (isValid(c.row - 1, c.col + 1)) {
      _tiles[(row - 1) * _size + col + 1] += 1;
    }

    // E
    if (isValid(c.row, c.col + 1)) {
      _tiles[row * _size + col + 1] += 1;
    }

    // S-E
    if (isValid(c.row + 1, c.col + 1)) {
      _tiles[(row + 1) * _size + col + 1] += 1;
    }

    // S
    if (isValid(c.row + 1, c.col)) {
      _tiles[(row + 1) * _size + col] += 1;
    }

    // S-W
    if (isValid(c.row + 1, c.col - 1)) {
      _tiles[(row + 1) * _size + col - 1] += 1;
    }
  }
}

std::vector<BoardGenerator::Coord> BoardGenerator::generateMinesPositions() {
  std::vector<BoardGenerator::Coord> mines(_numMines);

  bool mark[kMaxSize * kMaxSize];
  memset(mark, false, sizeof mark);

  srand(static_cast<unsigned int>(time(nullptr)));

  for (int i = 0; i < _numMines;) {
    int r = rand() % (_size * _size);
    int row = r / _size;
    int col = r % _size;

    if (!mark[r]) {
      mines[i] = Coord{row, col};

      _tiles[static_cast<std::size_t>(row) * _size + col] = kMineTileValue;
      mark[r] = true;
      i++;
    }
  }

  return mines;
}