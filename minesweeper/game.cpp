// clang-format off
#include "pch.h"
#include "assets.hpp"
#include "components.hpp"
#include "boardgenerator.hpp"
#include "game.hpp"

// clang-format on
const int kScreenWidth = 800;
const int kScreenHeight = 600;

const int kTileSizeW = 21;
const int kTileSizeH = 21;

namespace fs = std::filesystem;

Game::Game(const fs::path& assetsDir, std::shared_ptr<spdlog::logger> logger)
    : _window{nullptr},
      _renderer{nullptr},
      _gameLevel{GameLevel::Beginner},
      _boardSize{9},
      _minesCount{10},
      _logger{logger},
      _firstClick{true},
      _gameOver{false},
      _revealedTilesCount{0} {
  fs::path graphicsAssetsDir = assetsDir;
  graphicsAssetsDir /= "graphics/21x21";
  _graphicAssets = std::make_unique<GraphicsAssets>(graphicsAssetsDir);
}

Game::~Game() {}

bool Game::init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    _logger->error("SDL could not initialize");
    return false;
  }

  int width = kTileSizeW * _boardSize;
  int height = kTileSizeH * _boardSize;

  SDL_Window* window = SDL_CreateWindow(
      "Mines", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
      SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
  if (!window) {
    return false;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    return false;
  }

  _window = window;
  _renderer = renderer;

  _graphicAssets->setRenderer(_renderer);

  if (!_graphicAssets->load()) {
    _logger->error("Cannot load graphics assets. Error: {}", SDL_GetError());
    return false;
  }

  initEntities();

  if (SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0) {
    return false;
  }

  return true;
}

void Game::destroy() {
  // SDL_Delay(3000);
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void Game::run() {
  bool quit = false;
  SDL_Event ev;

  while (!quit) {
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT) {
        quit = true;
      } else if (ev.type == SDL_KEYDOWN) {
        switch (ev.key.keysym.sym) {
          case SDLK_n:
            // reset the current game level
            reset();
            break;
          case SDLK_b:
            changeGameLevel(GameLevel::Beginner);
            break;
          case SDLK_i:
            changeGameLevel(GameLevel::Intermediate);
            break;
          case SDLK_a:
            changeGameLevel(GameLevel::Advanced);
            break;
          default:
            break;
        }
      }

      startFrame();

      update(&ev);
      render();

      endFrame();
    }
  }
}

void Game::setGameLevel(GameLevel level) {
  switch (level) {
    case GameLevel::Beginner:
      _boardSize = 9;
      _minesCount = 10;
      break;

    case GameLevel::Intermediate:
      _boardSize = 16;
      _minesCount = 40;
      break;

    case GameLevel::Advanced:
      _boardSize = 24;
      _minesCount = 99;
      break;

    default:
      break;
  }
}

void Game::initEntities() {
  BoardGenerator bg(_boardSize, _minesCount);
  bg.generate();
  std::vector<int> tiles = bg.getTiles();

  for (std::size_t i = 0; i < tiles.size(); i++) {
    std::size_t row = i / _boardSize;
    std::size_t col = i % _boardSize;

    auto ent = _registry.create();
    _registry.emplace<TileComponent>(ent, Position{row, col}, tiles[i], false);

    std::shared_ptr<Texture> texture = _graphicAssets->get(kUnexplored);
    _registry.emplace<GraphicsComponent>(ent, GraphicsComponent{texture});

    _boardState.entities.emplace_back(ent);
  }
}

void Game::reset() {
  _logger->info("Reseting the game");
  _boardState.entities.clear();
  _registry.clear();

  initEntities();
  _gameOver = false;
  _firstClick = true;
  _revealedTilesCount = 0;
}

void Game::changeGameLevel(GameLevel level) {
  _logger->info("Changing the game level to {}", level);
  setGameLevel(level);
  reset();

  SDL_SetWindowSize(_window, kTileSizeW * _boardSize, kTileSizeH * _boardSize);
}

void Game::update(SDL_Event* ev) {
  if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
    if (_gameOver) {
      return;
    }

    int x, y;
    SDL_GetMouseState(&x, &y);

    std::size_t col = static_cast<std::size_t>(x) / kTileSizeW;
    std::size_t row = static_cast<std::size_t>(y) / kTileSizeH;

    std::size_t i = row * _boardSize + col;
    auto& ent = _boardState.entities[i];

    if (!_registry.all_of<TileComponent, GraphicsComponent>(ent)) {
      return;
    }

    TileComponent& tile = _registry.get<TileComponent>(ent);
    GraphicsComponent& graphics = _registry.get<GraphicsComponent>(ent);

    if (tile.zoneValue == kMineTileValue) {
      _logger->info("Clicked on a mine at {} _firstClick={}", tile.position,
                    _firstClick);

      if (!_firstClick) {
        // clicked on a mine: game is over
        graphics.texture = getTextureForZoneValue(tile.zoneValue + 1);
        tile.explored = true;

        _gameOver = true;

        // reveal all mines
        revealMines();

        _logger->info("Game is over");

        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Mines",
                                 "You lost !", _window);

        return;
      }

      // first click on a mine : swap the clicked on "entity" with the first
      // entity which is not a "mine"
      _logger->info(
          "First clicked tile is a mine. Swap it with the first tile which is "
          "not a mine.");

      for (auto& e : _boardState.entities) {
        if (_registry.all_of<TileComponent, GraphicsComponent>(e)) {
          auto& tc = _registry.get<TileComponent>(e);
          if (tc.zoneValue == kMineTileValue) {
            continue;
          }
          std::swap(tile, tc);

          auto& gc = _registry.get<GraphicsComponent>(e);
          std::swap(graphics, gc);

          _logger->info("Swapped the mine from {} with tile at {}",
                        tile.position, tc.position);

          break;
        }
      }
    }

    if (!tile.explored) {
      if (tile.zoneValue != kEmptyTileValue) {
        graphics.texture = getTextureForZoneValue(tile.zoneValue);
        tile.explored = true;
        _revealedTilesCount++;
      } else {
        // try to reveal the nearby tiles
        int r = static_cast<int>(row);
        int c = static_cast<int>(col);
        _logger->debug(
            "Try revealing nearby tiles starting from {}",
            Position{static_cast<std::size_t>(r), static_cast<std::size_t>(c)});
        tryRevealNearbyTiles(r, c);
      }
    }

    _firstClick = false;

    if (won()) {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Mines", "You won !",
                               _window);
      revealMines();
    }
  }
}

void Game::render() {
  auto view = _registry.view<TileComponent, GraphicsComponent>();
  view.each([=](const auto& tile, const auto& graphics) {
    SDL_Rect dstRect{static_cast<int>(tile.position.col) * kTileSizeW,
                     static_cast<int>(tile.position.row) * kTileSizeH,
                     kTileSizeW, kTileSizeH};

    SDL_RenderCopy(_renderer, graphics.texture->raw_ptr(), nullptr, &dstRect);
  });
}

void Game::tryRevealNearbyTiles(int row, int col) {
  std::size_t i = static_cast<std::size_t>(row) * _boardSize +
                  static_cast<std::size_t>(col);

  auto ent = _boardState.entities[i];
  auto& tile = _registry.get<TileComponent>(ent);
  if (tile.explored) {
    return;
  }

  if (tile.zoneValue != kEmptyTileValue && tile.zoneValue != kMineTileValue) {
    // reveal the nearby tiles that touch the empty tiles
    tile.explored = true;
    _revealedTilesCount++;

    auto& graphics = _registry.get<GraphicsComponent>(ent);
    graphics.texture = getTextureForZoneValue(tile.zoneValue);
    return;
  }

  // revel the nearby empty tiles
  tile.explored = true;
  _revealedTilesCount++;

  auto& graphics = _registry.get<GraphicsComponent>(ent);
  graphics.texture = getTextureForZoneValue(kEmptyTileValue);

  // W
  if (isValid(row, col - 1)) {
    tryRevealNearbyTiles(row, col - 1);
  }

  // N-W
  if (isValid(row - 1, col - 1)) {
    tryRevealNearbyTiles(row - 1, col - 1);
  }

  // N
  if (isValid(row - 1, col)) {
    tryRevealNearbyTiles(row - 1, col);
  }

  // N-E
  if (isValid(row - 1, col + 1)) {
    tryRevealNearbyTiles(row - 1, col + 1);
  }

  // E
  if (isValid(row, col + 1)) {
    tryRevealNearbyTiles(row, col + 1);
  }

  // S-E
  if (isValid(row + 1, col + 1)) {
    tryRevealNearbyTiles(row + 1, col + 1);
  }

  // S
  if (isValid(row + 1, col)) {
    tryRevealNearbyTiles(row + 1, col);
  }

  // S-W
  if (isValid(row + 1, col - 1)) {
    tryRevealNearbyTiles(row + 1, col - 1);
  }
}

void Game::revealMines() {
  _logger->debug("Reveal all mines");
  for (auto& e : _boardState.entities) {
    if (_registry.all_of<TileComponent, GraphicsComponent>(e)) {
      auto& tc = _registry.get<TileComponent>(e);
      if (!tc.explored && tc.zoneValue == kMineTileValue) {
        tc.explored = true;

        auto& graphics = _registry.get<GraphicsComponent>(e);
        graphics.texture = getTextureForZoneValue(tc.zoneValue);
      }
    }
  }
}

std::shared_ptr<Texture> Game::getTextureForZoneValue(int value) {
  std::shared_ptr<Texture> texture;
  switch (value) {
    case 0:
      texture = _graphicAssets->get(k0);
      break;
    case 1:
      texture = _graphicAssets->get(k1);
      break;
    case 2:
      texture = _graphicAssets->get(k2);
      break;
    case 3:
      texture = _graphicAssets->get(k3);
      break;
    case 4:
      texture = _graphicAssets->get(k4);
      break;
    case 5:
      texture = _graphicAssets->get(k5);
      break;
    case 6:
      texture = _graphicAssets->get(k6);
      break;
    case 7:
      texture = _graphicAssets->get(k7);
      break;
    case 8:
      texture = _graphicAssets->get(k8);
      break;
    case kMineTileValue:
      texture = _graphicAssets->get(kMine);
      break;
    default:
      texture = _graphicAssets->get(kMineHit);
      break;
  }
  return texture;
}
