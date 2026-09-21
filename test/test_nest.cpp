#include <gtest/gtest.h>

#include "nest/NestGrid.hpp"
#include "nest/NestValidator.hpp"

using namespace ant::v01;

TEST(NestGrid, InitDimensions) {
  NestGrid g;
  ASSERT_EQ(g.material.size(), 2048u * 1024u);
}

TEST(NestGrid, InitEntranceAir) {
  NestGrid g;
  ASSERT_EQ(g.materialAt({1023, 0}), CellMaterial::Air);
  ASSERT_EQ(g.materialAt({1024, 0}), CellMaterial::Air);
  ASSERT_EQ(g.materialAt({1025, 0}), CellMaterial::Air);
}

TEST(NestGrid, InitBoundaryBedrock) {
  NestGrid g;
  ASSERT_EQ(g.materialAt({0, 0}), CellMaterial::Bedrock);
  ASSERT_EQ(g.materialAt({2047, 0}), CellMaterial::Bedrock);
  ASSERT_EQ(g.materialAt({0, 1023}), CellMaterial::Bedrock);
}

TEST(NestGrid, InitSoilBelow) {
  NestGrid g;
  ASSERT_EQ(g.materialAt({1024, 1}), CellMaterial::Soil);
  ASSERT_EQ(g.soilStrengthAt({1024, 1}), 100);
}

TEST(NestGrid, OutOfBoundsReturnsBedrock) {
  NestGrid g;
  ASSERT_EQ(g.materialAt({-1, 0}), CellMaterial::Bedrock);
  ASSERT_EQ(g.materialAt({2048, 0}), CellMaterial::Bedrock);
  ASSERT_EQ(g.materialAt({1024, -1}), CellMaterial::Bedrock);
  ASSERT_EQ(g.materialAt({1024, 1024}), CellMaterial::Bedrock);
}

TEST(NestValidator, StructuralPassOnInit) {
  NestGrid g;
  ASSERT_TRUE(NestValidator::validateStructuralInvariants(g));
}

TEST(NestGrid, BoundaryMaterialsAndStrengthRemainFixed) {
  NestGrid g;
  for (int y = 0; y < NestGrid::D; ++y) {
    for (int x = 0; x < NestGrid::W; ++x) {
      if (x != 0 && x != NestGrid::W - 1 && y != 0 && y != NestGrid::D - 1)
        continue;
      const CellCoord c{x, y};
      const auto expected = y == 0 && x >= 1023 && x <= 1025
                                ? CellMaterial::Air : CellMaterial::Bedrock;
      for (const auto m : {CellMaterial::Soil, CellMaterial::Air,
                           CellMaterial::Rock, CellMaterial::Bedrock}) {
        g.setMaterial(c, m);
        g.setSoilStrength(c, 100);
        ASSERT_EQ(g.materialAt(c), expected);
        ASSERT_EQ(g.soilStrengthAt(c), 0);
      }
    }
  }
  ASSERT_TRUE(NestValidator::validateStructuralInvariants(g));
}

TEST(NestGrid, MaterialStrengthContract) {
  NestGrid g;
  const CellCoord c{1024, 1};
  ASSERT_EQ(g.soilStrengthAt(c), 100);
  for (const std::uint16_t strength : {100, 80, 60, 40, 20, 0}) {
    g.setSoilStrength(c, strength);
    EXPECT_EQ(g.soilStrengthAt(c), strength);
    EXPECT_EQ(g.materialAt(c), CellMaterial::Soil);
  }
  g.setSoilStrength(c, 65535);
  EXPECT_EQ(g.soilStrengthAt(c), 100);
  g.setSoilStrength(c, 42);
  EXPECT_EQ(g.soilStrengthAt(c), 42);
  g.setSoilStrength(c, 0);
  EXPECT_EQ(g.soilStrengthAt(c), 0);
  for (const auto m : {CellMaterial::Air, CellMaterial::Rock, CellMaterial::Bedrock}) {
    g.setMaterial(c, CellMaterial::Soil);
    g.setSoilStrength(c, 100);
    g.setMaterial(c, m);
    EXPECT_EQ(g.soilStrengthAt(c), 0);
    g.setSoilStrength(c, 100);
    EXPECT_EQ(g.soilStrengthAt(c), 0);
  }
}

TEST(CellCoord, RowMajorOrder) {
  EXPECT_LT((CellCoord{0, 0}), (CellCoord{1, 0}));
  EXPECT_LT((CellCoord{2047, 0}), (CellCoord{0, 1}));
  EXPECT_FALSE((CellCoord{0, 1} < CellCoord{2047, 0}));
  EXPECT_FALSE((CellCoord{1, 0} < CellCoord{0, 0}));
  EXPECT_FALSE((CellCoord{1, 1} < CellCoord{1, 1}));
  EXPECT_EQ((CellCoord{1, 1}), (CellCoord{1, 1}));
  EXPECT_NE((CellCoord{1, 1}), (CellCoord{1, 2}));
}

template <typename Validator>
concept HasCompletedNestValidation = requires(const NestGrid& g) {
  Validator::validateCompletedNest(g);
};

TEST(NestValidator, NoCompletedNestValidationInPart2A) {
  static_assert(!HasCompletedNestValidation<NestValidator>);
  EXPECT_FALSE(HasCompletedNestValidation<NestValidator>);
}

TEST(NestValidator, RejectsDisconnectedAir) {
  NestGrid g;
  g.setMaterial({1024, 2}, CellMaterial::Air);
  EXPECT_FALSE(NestValidator::validateStructuralInvariants(g));
  g.setMaterial({1024, 1}, CellMaterial::Air);
  EXPECT_TRUE(NestValidator::validateStructuralInvariants(g));
}

TEST(NestValidator, RejectsDirectBoundaryAndEntranceCorruption) {
  NestGrid g;
  // The public SoA can bypass setters; validation must still detect corruption.
  g.material[100] = static_cast<std::uint8_t>(CellMaterial::Air);
  EXPECT_FALSE(NestValidator::validateStructuralInvariants(g));
  g.material[100] = static_cast<std::uint8_t>(CellMaterial::Bedrock);
  g.material[static_cast<std::size_t>(500) * NestGrid::W] =
      static_cast<std::uint8_t>(CellMaterial::Air);
  EXPECT_FALSE(NestValidator::validateStructuralInvariants(g));
  g.material[static_cast<std::size_t>(500) * NestGrid::W] =
      static_cast<std::uint8_t>(CellMaterial::Bedrock);
  g.entrance.centerX = 1023;
  EXPECT_FALSE(NestValidator::validateStructuralInvariants(g));
}
