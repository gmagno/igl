// This is work in progress

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "igl.h"
#include <string>

TEST_CASE("Img")
{
  INFO("Some info here");

  // If assertion fails test execution is stopped
  REQUIRE(36 == 36);

  // If assertion fails test execution continues
  CHECK(1 > 0);
  CHECK(1 > 0);
}

// BDD style

SCENARIO("Img creation")
{

  GIVEN("A png image")
  {

    REQUIRE(36 == 36);

    WHEN("something happens")
    {
      THEN("something else should be more than 0")
      {
        CHECK(1 > 0);
        CHECK(1 > 0);
      }
    }
  }
}
