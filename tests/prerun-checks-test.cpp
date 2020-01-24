#include <catch2/catch.hpp>
#include "../src/misc/prerun-checks.hpp"
TEST_CASE("prerun checks"){
    const bool canRun = CanRun();
    REQUIRE(canRun);
}
