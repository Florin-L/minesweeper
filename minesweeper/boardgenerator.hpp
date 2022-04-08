#pragma once

const unsigned int kEmptyTileValue = 0;
const unsigned int kMineTileValue = 9;

/// @brief Generates the configuration of the border.
///
class BoardGenerator {
 public:
  BoardGenerator(int size, int numMines) : _size{size}, _numMines{numMines} {
    std::size_t sz = size;
    _tiles.resize(sz * sz);
  }

  ~BoardGenerator() {}

  BoardGenerator(const BoardGenerator&) = delete;
  BoardGenerator& operator=(const BoardGenerator&) = delete;

  bool check() { return true; }
  void generate();

  std::vector<int>&& getTiles() { return std::move(_tiles); }

 private:
  struct Coord {
    int row;
    int col;

    Coord() : row{0}, col{0} {}
    Coord(int r, int c) : row{r}, col{c} {}
  };

  /// @brief Generates the coordinates of the mines.
  ///
  /// @return A vector holding the coordinates of the mines.
  std::vector<Coord> generateMinesPositions();

  bool isValid(int row, int col) {
    std::size_t i = static_cast<std::size_t>(row) * _size + col;
    return row >= 0 && row < _size && col >= 0 && col < _size &&
           _tiles[i] != kMineTileValue;
  }

 private:
  std::vector<int> _tiles;
  int _size;
  int _numMines;
};