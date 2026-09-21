#pragma once

namespace ant::v01 {

struct CellCoord {
  int x;
  int y;

  constexpr bool operator==(const CellCoord&) const = default;
  constexpr bool operator!=(const CellCoord&) const = default;
  // Row-major ordering by y, then x.
  constexpr bool operator<(const CellCoord& other) const {
    return y < other.y || (y == other.y && x < other.x);
  }
};

}  // namespace ant::v01
