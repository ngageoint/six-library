#include "TestCase.h"
#include <six/sicd/Utilities.h>

TEST_CASE(testInvalidVersions)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.1.0");
    logging::NullLogger log;

    // Invalid SICD version should throw
    TEST_EXCEPTION(six::sicd::Utilities::updateVersion(complexData, "1.1.7", log));

    // No-op should throw
    TEST_EXCEPTION(six::sicd::Utilities::updateVersion(complexData, "1.1.0", log));

    // Downgrade should throw
    TEST_EXCEPTION(six::sicd::Utilities::updateVersion(complexData, "1.0.0", log));

    // Don't support 0.3.1 (yet?)
    TEST_EXCEPTION(six::sicd::Utilities::updateVersion(complexData, "0.3.1", log));
}

TEST_CASE(test110To120)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.1.0");
    logging::NullLogger log;
    six::sicd::Utilities::updateVersion(complexData, "1.2.0", log);

    // AFAIK, this is the only difference...
    TEST_ASSERT_EQ(complexData.getVersion(), "1.2.0");
}

TEST_CASE(testTransitiveUpdate)
{
    six::sicd::ComplexData complexData;
    complexData.setVersion("1.0.0");
    logging::NullLogger log;
    six::sicd::Utilities::updateVersion(complexData, "1.1.0", log);

    // This is still the only notable difference
    TEST_ASSERT_EQ(complexData.getVersion(), "1.1.0");
}

TEST_CASE(testUpdateDistRefLinePoly)
{
    logging::NullLogger log;
    six::sicd::ComplexData complexData;

    // 0.4.0 -> 0.4.1 or 0.5.0 should initialize the polynomial
    complexData.setVersion("0.4.0");
    complexData.rma.reset(new six::sicd::RMA());
    complexData.rma->rmat.reset(new six::sicd::RMAT());

    six::sicd::Utilities::updateVersion(complexData, "0.5.0", log);
    TEST_ASSERT_FALSE(six::Init::isUndefined(complexData.rma->rmat->distRefLinePoly));

    // Jumping from 0.4.0 to 1.0.0 should leave it uninitialized
    complexData.setVersion("0.4.0");
    complexData.rma.reset(new six::sicd::RMA());
    complexData.rma->rmat.reset(new six::sicd::RMAT());

    six::sicd::Utilities::updateVersion(complexData, "1.0.0", log);
    TEST_ASSERT_TRUE(six::Init::isUndefined(complexData.rma->rmat->distRefLinePoly));
}

int main(int, char**)
{
    // These tests should suffice to cover all of the "tricky"
    // logic in the implementation.  Any remaining bugs would
    // most likely be due to misreading the standards.
    TEST_CHECK(testInvalidVersions);
    TEST_CHECK(test110To120);
    TEST_CHECK(testTransitiveUpdate);
    TEST_CHECK(testUpdateDistRefLinePoly);
    return 0;
}