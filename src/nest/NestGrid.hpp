#pragma once

#include <cstdint>
#include <vector>

#include "CellCoord.hpp"
#include "CellMaterial.hpp"
#include "NestEntrance.hpp"

namespace ant::v01 {

class NestGrid {
 public:
  static constexpr int W = 2048;
  static constexpr int D = 1024;

  std::vector<std::uint8_t> material;
  std::vector<std::uint16_t> soilStrength;
  NestEntrance entrance;

  NestGrid();

  bool isValid(CellCoord c) const;
  CellMaterial materialAt(CellCoord c) const;
  std::uint16_t soilStrengthAt(CellCoord c) const;
  // Boundary cells (including the fixed Air entrance) cannot be changed.
  // Non-Soil materials have zero strength; Soil strength is clamped to [0, 100].
  void setMaterial(CellCoord c, CellMaterial m);
  void setSoilStrength(CellCoord c, std::uint16_t s);
};

}  // namespace ant::v01
