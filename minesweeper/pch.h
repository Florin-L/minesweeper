// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for
// future builds. This also affects IntelliSense performance, including code
// completion and many code browsing features. However, files listed here are
// ALL re-compiled if any one of them is updated between builds. Do not add
// files here that you will be updating frequently as this negates the
// performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

// clang-format off
#include <random>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_image.h>

#include <entt/entt.hpp>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include "cxxopts.hpp"

// clang-format on

#endif  // PCH_H
