#pragma once

#include "structs.hpp"

/// @brief Graphics component
struct GraphicsComponent {
  std::shared_ptr<Texture> texture;
};

/// @brief Tile component
struct TileComponent {
  Position position;
  int zoneValue;
  bool explored;
};
