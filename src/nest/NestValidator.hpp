#pragma once

#include "NestGrid.hpp"

namespace ant::v01 {

class NestValidator {
 public:
  static bool validateStructuralInvariants(const NestGrid& g);
  // Completed-nest validation is deferred to Part 2C.
};

}  // namespace ant::v01
