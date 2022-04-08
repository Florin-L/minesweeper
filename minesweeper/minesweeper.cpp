// minesweeper.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

// clang-format off
#include "pch.h"
#include "assets.hpp"
#include "game.hpp"


#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")

// clang-format on

//
template <typename R, typename T>
R lambda_ptr_exec(T* v) {
  return (R)(*v)();
}

template <typename R = void, typename Tfp = R (*)(void), typename T>
Tfp lambda_ptr(T& v) {
  return (Tfp)lambda_ptr_exec<R, T>;
}

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  fs::path assetsDir = fs::current_path().append("..").append("assets");
  GameLevel level = GameLevel::Beginner;

  cxxopts::Options options("minesweeper", "minesweeper");

  // clang-format off
  options.add_options()
      ("l,level", "Difficulty level", cxxopts::value<std::string>()->default_value("b"))
      ("assets_dir", "Assest directory", cxxopts::value<std::string>());
  // clang-format on

  auto result = options.parse(argc, argv);

  if (result.count("assets_dir")) {
    assetsDir = result["assets_dir"].as<std::string>();
  }

  std::string levelValue = result["level"].as<std::string>();

  if (_strcmpi(levelValue.c_str(), "b") == 0) {
    level = GameLevel::Beginner;
  } else if (_strcmpi(levelValue.c_str(), "i") == 0) {
    level = GameLevel::Intermediate;
  } else if (_strcmpi(levelValue.c_str(), "a") == 0) {
    level = GameLevel::Advanced;
  }

  auto max_size = 1048576 * 5;
  auto max_files = 3;
  auto logger = spdlog::rotating_logger_mt(
      "basic_logger", "logs/minesweeper.log", max_size, max_files);

  // auto terminate = [&logger]() {
  //   logger->info("Exit the application");
  //   logger->flush();
  // };
  // void (*fp)(void) = lambda_ptr(terminate);
  // std::atexit(fp);

  spdlog::flush_every(std::chrono::seconds(3));
  logger->set_level(spdlog::level::debug);

  logger->info("*********************************");
  logger->info("****** Starting a new game ******");
  logger->info("*********************************");

  logger->info("Assets directory: {}, Difficulty: {}", assetsDir.string(),
               level);

  std::unique_ptr<Game> game = std::make_unique<Game>(assetsDir, logger);
  game->setGameLevel(level);

  if (!game->init()) {
    logger->error("Failed to initialize the game. Error: {}", SDL_GetError());
    exit(1);
  }

  game->run();
  game->destroy();

  logger->info("Exit the application");
  logger->flush();

  return 0;
}
