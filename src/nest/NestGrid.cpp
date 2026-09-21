#include "NestGrid.hpp"

#include <algorithm>
#include <cstddef>

namespace ant::v01 {
namespace {
std::size_t indexOf(CellCoord c) {
  return static_cast<std::size_t>(c.y) * NestGrid::W + c.x;
}
bool isBoundary(CellCoord c) {
  return c.x == 0 || c.x == NestGrid::W - 1 ||
         c.y == 0 || c.y == NestGrid::D - 1;
}
}  // namespace

NestGrid::NestGrid()
    : material(static_cast<std::size_t>(W) * D,
               static_cast<std::uint8_t>(CellMaterial::Soil)),
      soilStrength(static_cast<std::size_t>(W) * D, 100) {
  for (int y = 0; y < D; ++y) {
    for (int x = 0; x < W; ++x) {
      if (x == 0 || x == W - 1 || y == 0 || y == D - 1) {
        material[indexOf({x, y})] = static_cast<std::uint8_t>(CellMaterial::Bedrock);
        soilStrength[indexOf({x, y})] = 0;
      }
    }
  }
  for (int x = entrance.centerX - 1; x <= entrance.centerX + 1; ++x) {
    material[indexOf({x, 0})] = static_cast<std::uint8_t>(CellMaterial::Air);
  }
}

bool NestGrid::isValid(CellCoord c) const {
  return c.x >= 0 && c.x < W && c.y >= 0 && c.y < D;
}

CellMaterial NestGrid::materialAt(CellCoord c) const {
  return isValid(c) ? static_cast<CellMaterial>(material[indexOf(c)])
                    : CellMaterial::Bedrock;
}

std::uint16_t NestGrid::soilStrengthAt(CellCoord c) const {
  return isValid(c) ? soilStrength[indexOf(c)] : 0;
}

void NestGrid::setMaterial(CellCoord c, CellMaterial m) {
  // Boundary materials are fixed; the predefined entrance is initialized above.
  if (!isValid(c) || isBoundary(c)) return;
  material[indexOf(c)] = static_cast<std::uint8_t>(m);
  if (m != CellMaterial::Soil) soilStrength[indexOf(c)] = 0;
}

void NestGrid::setSoilStrength(CellCoord c, std::uint16_t s) {
  if (!isValid(c) || isBoundary(c)) return;
  soilStrength[indexOf(c)] = materialAt(c) == CellMaterial::Soil
                                 ? std::min<std::uint16_t>(s, 100)
                                 : 0;
}

}  // namespace ant::v01
