#include "gtest/gtest.h"
#include "gtest/gtest.h"

#define private public
#define protected public
//#include <xxx.h>
#undef private
#undef protected

namespace test_server
{

class ServerTest : public ::testing::Test {

protected:
    ServerTest()
    {}

    ~ServerTest()
    {}

    void SetUp()
    {}

    void TearDown()
    {}
};

TEST_F(ServerTest, demo)
{
    ASSERT_TRUE(true);
}

}
