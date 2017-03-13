#include <gtest/gtest.h>

#include <common/config.h>
#include <server/device/device.h>


//--------------------------------------------------------------------------------------------------

class TestDevice : public ::testing::Test { };

//--------------------------------------------------------------------------------------------------

TEST_F( TestDevice, testIsAlive )
{
    config_t config;

    //device_t device( driver, config );
}

//--------------------------------------------------------------------------------------------------
