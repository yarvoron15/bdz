#include <catch2/catch_shim.hpp>

#include "rational.h"


TEST_CASE("Simple")
{
   Rational r(2, 4);
   REQUIRE(r.getNumerator() == 1);
   REQUIRE(r.getDenominator() == 2);

   Rational q(2, 3);

   auto s = r + q;

   REQUIRE(s > 1);
   REQUIRE(s.getDenominator() == 6);
}
