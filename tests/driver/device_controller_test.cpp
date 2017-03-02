#include <gtest/gtest.h>

#include <driver/device_controller.h>

#include <driver/driver_mock.h>
#include <driver/device_observer_mock.h>

#include <iostream>
#include <memory>


class TestDeviceController : public ::testing::Test
{
protected:
    void SetUp( );
    void TearDown( );
};

//--------------------------------------------------------------------------------------------------

void TestDeviceController::SetUp( )
{
}

//--------------------------------------------------------------------------------------------------

void TestDeviceController::TearDown( )
{
}

//--------------------------------------------------------------------------------------------------

TEST_F( TestDeviceController, testExecuteCommandReturnsTrue )
{
    std::unique_ptr< driver_mock_t > driver( new driver_mock_t( ) );
    device_controller_t controller( *driver );

    ASSERT_EQ( ErrorCode::OK, controller.execute_command( { 0, 0 } ) );
}

//--------------------------------------------------------------------------------------------------

TEST_F( TestDeviceController, testAddObserver )
{
    driver_mock_t driver;
    device_controller_t controller( driver );

    device_observer_mock_t observer;
    ASSERT_EQ( ErrorCode::OK, controller.add_observer( observer ) );
}

//--------------------------------------------------------------------------------------------------

TEST_F( TestDeviceController, testRemoveObserver )
{
    driver_mock_t driver;
    device_controller_t controller( driver );

    device_observer_mock_t observer;
    controller.add_observer( observer );
    ASSERT_EQ( ErrorCode::OK, controller.remove_observer( observer ) );
}

//--------------------------------------------------------------------------------------------------

TEST_F( TestDeviceController, testOnLightChangedCalled )
{
    driver_mock_t driver;
    device_controller_t controller( driver );

    device_observer_mock_t observer;
    controller.add_observer( observer );

    controller.execute_command( { 1, 1 } );
    ASSERT_EQ( 1, observer.light_changed_count ) << "incorrect count of lights changing";
}

//--------------------------------------------------------------------------------------------------
