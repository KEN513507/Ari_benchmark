#pragma once

namespace ant::v01 {

struct CellCoord {
  int x;
  int y;

  constexpr bool operator==(const CellCoord&) const = default;
  constexpr bool operator!=(const CellCoord&) const = default;
  // Lexicographic ordering by x, then y.
  constexpr bool operator<(const CellCoord& other) const {
    return x < other.x || (x == other.x && y < other.y);
  }
};

}  // namespace ant::v01
