#include "NestValidator.hpp"

#include <cstddef>
#include <vector>

namespace ant::v01 {
namespace {
bool allAirReachable(const NestGrid& g, std::size_t airCount) {
  std::vector<bool> visited(g.material.size(), false);
  std::vector<CellCoord> queue;
  queue.push_back({g.entrance.centerX, 0});
  visited[static_cast<std::size_t>(g.entrance.centerX)] = true;
  constexpr CellCoord offsets[] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (std::size_t head = 0; head < queue.size(); ++head) {
    const CellCoord c = queue[head];
    for (const CellCoord offset : offsets) {
      const CellCoord next{c.x + offset.x, c.y + offset.y};
      if (g.materialAt(next) != CellMaterial::Air) continue;
      const auto index = static_cast<std::size_t>(next.y) * NestGrid::W + next.x;
      if (!visited[index]) {
        visited[index] = true;
        queue.push_back(next);
      }
    }
  }
  return queue.size() == airCount;
}
}  // namespace

bool NestValidator::validateStructuralInvariants(const NestGrid& g) {
  constexpr auto cellCount = static_cast<std::size_t>(NestGrid::W) * NestGrid::D;
  if (g.material.size() != cellCount || g.soilStrength.size() != cellCount)
    return false;
  if (g.entrance.widthCells != 3 || g.entrance.centerX != 1024)
    return false;

  std::size_t airCount = 0;
  for (int y = 0; y < NestGrid::D; ++y) {
    for (int x = 0; x < NestGrid::W; ++x) {
      const auto m = g.materialAt({x, y});
      if (y == 0) {
        const bool entranceCell = x >= g.entrance.centerX - 1 &&
                                  x <= g.entrance.centerX + 1;
        if (m != (entranceCell ? CellMaterial::Air : CellMaterial::Bedrock))
          return false;
      }
      if ((x == 0 || x == NestGrid::W - 1 || y == NestGrid::D - 1) &&
          m != CellMaterial::Bedrock)
        return false;
      if (m == CellMaterial::Air) ++airCount;
    }
  }
  return allAirReachable(g, airCount);
}

}  // namespace ant::v01
