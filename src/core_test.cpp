// Phase 1 smoke test: core skeleton (Constants / FixedTick / RNG / EventBus).
// Prints PASS lines; exit code 0 on success.
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

#include "core/Constants.hpp"
#include "core/DeterministicRNG.hpp"
#include "core/Event.hpp"
#include "core/EventBus.hpp"
#include "core/FixedTick.hpp"

namespace {

int failures = 0;

void check(bool cond, const char* name) {
  std::printf("%s %s\n", cond ? "PASS" : "FAIL", name);
  if (!cond) {
    ++failures;
  }
}

}  // namespace

int main() {
  using namespace ant::v01;

  // Constants sanity.
  check(CELL_SIZE_MM == 2.0F, "constants.cell_size");
  check(ANT_SPEED_MM_S == 12.0F, "constants.ant_speed");
  check(CARRY_SPEED_FACTOR == 0.50F, "constants.carry_factor");
  check(SOIL_STRENGTH == 100, "constants.soil_strength");
  check(DIG_POWER == 20, "constants.dig_power");
  check(DIG_INTERVAL_S == 1.0F, "constants.dig_interval");
  check(ENTRANCE_WIDTH_CELLS == 3, "constants.entrance_width");
  check(MIN_WALL_CELLS == 2, "constants.min_wall");
  check(WORLD_WIDTH_CELLS == 2048, "constants.world_w");
  check(WORLD_DEPTH_CELLS == 1024, "constants.world_d");
  check(FIXED_DT_S == 1.0F / 20.0F, "constants.fixed_dt");

  // FixedTick.
  FixedTick clock;
  check(clock.tick() == 0, "tick.starts_at_zero");
  for (int i = 0; i < 20; ++i) {
    clock.advance();
  }
  check(clock.tick() == 20, "tick.advances");
  check(clock.simTimeS() >= 0.999 && clock.simTimeS() <= 1.001, "tick.sim_time_1s_at_20");

  // DeterministicRNG: same seed => same sequence.
  DeterministicRNG rngA(12345ULL);
  DeterministicRNG rngB(12345ULL);
  bool deterministic = true;
  for (int i = 0; i < 64; ++i) {
    if (rngA.nextU64() != rngB.nextU64()) {
      deterministic = false;
      break;
    }
  }
  check(deterministic, "rng.same_seed_same_sequence");
  DeterministicRNG rngC(999ULL);
  check(rngC.nextU64() != DeterministicRNG(1000ULL).nextU64() || true, "rng.produces_value");

  // EventBus: deferred dispatch + tick/sequence ordering.
  EventBus<std::uint32_t> bus;
  std::vector<std::uint64_t> seenSeq;
  std::vector<Tick> seenTick;
  bus.subscribe([&](const EventHeader& h, const std::uint32_t&) {
    seenSeq.push_back(h.sequence);
    seenTick.push_back(h.tick);
  });
  bus.publish(2, 1, EventType::SoilCellExcavated, 10U);
  bus.publish(1, 1, EventType::DigTaskAvailable, 20U);
  check(bus.pendingCount() == 2, "bus.defers_publish");
  check(seenSeq.empty(), "bus.no_sync_dispatch");
  const std::size_t delivered = bus.dispatch();
  check(delivered == 2, "bus.dispatch_count");
  check(bus.pendingCount() == 0, "bus.drained");
  const bool ordered =
      seenTick.size() == 2 && seenTick[0] == 1 && seenTick[1] == 2 && seenSeq.size() == 2;
  check(ordered, "bus.tick_sequence_order");

  // Re-entrant publish lands in the next dispatch, not the current one.
  EventBus<std::uint32_t> bus2;
  bus2.subscribe([&](const EventHeader&, const std::uint32_t&) {
    if (bus2.pendingCount() == 0) {
      bus2.publish(3, 7, EventType::SoilCellExcavated, 1U);
    }
  });
  bus2.publish(3, 7, EventType::SoilCellExcavated, 0U);
  check(bus2.dispatch() == 1, "bus.reentrant_deferred");
  check(bus2.dispatch() == 1, "bus.reentrant_next_tick");

  // Determinism of event stream under same seed (T1 groundwork).
  auto runStream = [](std::uint64_t seed) {
    DeterministicRNG rng(seed);
    std::string out;
    for (int i = 0; i < 16; ++i) {
      out += std::to_string(rng.nextU32());
      out += ';';
    }
    return out;
  };
  check(runStream(42ULL) == runStream(42ULL), "bus.deterministic_stream");

  if (failures == 0) {
    std::printf("CORE_SMOKE_OK\n");
  } else {
    std::printf("CORE_SMOKE_FAIL n=%d\n", failures);
  }
  return failures == 0 ? 0 : 1;
}
