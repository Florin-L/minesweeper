#pragma once

#include "structs.hpp"

class GraphicsAssets;
class Renderer;

/// @brief Game levels.
enum class GameLevel { Beginner, Intermediate, Advanced };

/// @brief Custom formatter for GameLevel
template <>
struct fmt::formatter<GameLevel> : formatter<string_view> {
  template <typename FormatContext>
  auto format(GameLevel level, FormatContext& ctx) {
    string_view name = "unknown";
    switch (level) {
      case GameLevel::Beginner:
        name = "Beginner";
        break;
      case GameLevel::Intermediate:
        name = "Intermediate";
        break;
      case GameLevel::Advanced:
        name = "Advanced";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

/// @brief Custom formtter for Position
template <>
struct fmt::formatter<Position> : formatter<std::string> {
  template <typename FormatContext>
  auto format(Position pos, FormatContext& ctx) {
    std::string s = fmt::format("(row={}, col={})", pos.row, pos.col);
    return formatter<std::string>::format(s, ctx);
  }
};

/// @brief Stores the state of the board.
struct BoardState {
  std::vector<entt::entity> entities;  /// the entities
};

/// @brief Game class
class Game {
 public:
  /// @brief The constructor.
  /// @param assetsDir The directory the assets are loaded from.
  /// @param logger The logger.
  Game(const std::filesystem::path& assetsDir,
       std::shared_ptr<spdlog::logger> logger);

  /// @brief The destructor.
  ~Game();

  /// @brief Runs the game logic.
  void run();

  /// @brief Sets the difficulty level.
  /// @param level The difficulty level.
  void setGameLevel(GameLevel level);

  /// @brief Initializes the game.
  /// @return Returns true if the initialization succeedes; false otherwise.
  bool init();

  /// @brief Releases the renderer and the game window and then quits the
  /// application.
  void destroy();

 private:
  /// @brief Creates the games entities.
  void initEntities();

  /// @brief Starts rendering the current frame.
  void startFrame();

  /// @brief Ends rendering the current frame.
  void endFrame();

  /// @brief Recreates the components of a terminated game.
  void reset();

  /// @brief Changes the difficulty level.
  /// @param level The difficulty level.
  void changeGameLevel(GameLevel level);

  /// @brief Checks the input (keyboard and mouse) events and updates the game
  /// components.
  /// @param ev The input level.
  void update(SDL_Event* ev);

  /// @brief Renders the graphics elements.
  void render();

  /// @brief Returns a texture (corresponding to a tile zone value) to be
  /// rendered in a tile component.
  /// @param value The "zone" value of a tile (0: empty space; 1-8: the number
  /// of neighbours; 9: mine).
  /// @return The texture to be rendered in a tile.
  std::shared_ptr<Texture> getTextureForZoneValue(int value);

  /// @brief Reveals (recursively) all the touching tiles of an empty tile.
  /// @param row The row coordinate of the empty tile.
  /// @param col The column coordinate of the empty tile.
  void tryRevealNearbyTiles(int row, int col);

  /// @brief Show all the mines.
  void revealMines();

  /// @brief Checks if a tile coordinate is a valid board coordinate.
  /// @param row The row.
  /// @param col The column.
  /// @return true if the given coordinate is a valid board coordinate; false
  /// otherwise.
  bool isValid(int row, int col) {
    return row >= 0 && row < _boardSize && col >= 0 && col < _boardSize;
  }

  /// @brief Checks if the player won the current game.
  /// @return true if the player won; false otherwise.
  bool won() {
    return _revealedTilesCount ==
           (static_cast<std::size_t>(_boardSize) * _boardSize - _minesCount);
  }

 private:
  std::filesystem::path _assetsDir;  //!< The game assets directory.
  std::unique_ptr<GraphicsAssets>
      _graphicAssets;  //!< The graphics assets (textures).

  SDL_Window* _window;  //!< The application/game main window.
  std::shared_ptr<Renderer> _renderer{
      nullptr};  //!< The renderer (used to render the graphics
                 //!< objetcs in the game window).

  GameLevel _gameLevel;  //!< The current difficulty level.
  int _boardSize;        //!< The dimension of the board.
  int _minesCount;       //!< The number of mines.

  BoardState _boardState;  //!< The state of the board.
  bool _firstClick;
  bool _gameOver;
  std::size_t _revealedTilesCount;  //!< The number of tiles which are currently
                                    //!< revealed.

  std::shared_ptr<spdlog::logger> _logger;  //!< The logger.
  entt::registry _registry;                 //!< The entities register.
};
