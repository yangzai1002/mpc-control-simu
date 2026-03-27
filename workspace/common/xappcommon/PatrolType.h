#pragma once

#include <cstdint>

enum class PatrolType : std::int32_t { TurnAround, Reverse, Oneway, GlobalOneway, GlobalRawplan, Return, ClosedLoop };
