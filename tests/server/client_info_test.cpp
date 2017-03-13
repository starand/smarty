#include <gtest/gtest.h>

#include <server/client/client_info.h>

#include <time.h>


//--------------------------------------------------------------------------------------------------

class TestClientInfo : public ::testing::Test { };

//--------------------------------------------------------------------------------------------------

TEST_F( TestClientInfo, testIsAlive )
{
    client_info_t info( "endpoint", nullptr );

    ASSERT_TRUE( info.is_alive( 1 ) ) << "Info should be alive w/o any timeout";
    ASSERT_FALSE( info.is_alive( 0 ) ) << "Info should not be alive w/ 0 timeout";
}

//--------------------------------------------------------------------------------------------------

TEST_F( TestClientInfo, testFieldsSet )
{
    client_info_t info( "endpoint", nullptr );
    ASSERT_EQ( time( NULL ), info.last_time ) << "last_time should be equal now";
    ASSERT_EQ( "endpoint", info.endpoint ) << "endpoint field should be equal to \'endpoint\'";
    ASSERT_EQ( nullptr, info.socket ) << "socket field should be nullptr";
}

//--------------------------------------------------------------------------------------------------