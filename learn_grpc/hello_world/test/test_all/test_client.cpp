#include "gtest/gtest.h"
#include "gtest/gtest.h"

#define private public
#define protected public
//#include <xxx.h>
#undef private
#undef protected

namespace test_client
{

class ClientTest : public ::testing::Test {

protected:
    ClientTest()
    {}

    ~ClientTest()
    {}

    void SetUp()
    {}

    void TearDown()
    {}
};

TEST_F(ClientTest, demo)
{
    ASSERT_TRUE(true);
}

}
