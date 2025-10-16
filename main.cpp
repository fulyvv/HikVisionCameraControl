#include "HikVisonCameraControl.h"
#include "utils.h"

// 回调函数示例
void __stdcall ImageCallBackEx(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
    if (pFrameInfo)
    {
        INFO("Frame received: Width=%d, Height=%d, FrameNum=%d", pFrameInfo->nWidth, pFrameInfo->nHeight, pFrameInfo->nFrameNum);
    }
    else
    {
        INFO("Frame received with no frame info.");
    }
}

void testCameraFunctionality()
{
    INFO("Starting camera functionality test...");

    // 枚举设备
    MV_CC_DEVICE_INFO_LIST deviceList = enumDevice();
    if (deviceList.nDeviceNum == 0)
    {
        INFO("No devices found. Test failed.");
        return;
    }
    INFO("Devices enumerated successfully. Device count: %d", deviceList.nDeviceNum);

    // 创建相机
    std::vector<CMvCamera> cameras = createCamera(deviceList);
    if (cameras.empty())
    {
        INFO("Failed to create cameras. Test failed.");
        return;
    }
    INFO("Cameras created successfully.");

    // 设置触发模式
    if (!setTriggerMode(cameras, true))
    {
        INFO("Failed to set trigger mode. Test failed.");
        closeCamera(cameras);
        return;
    }
    INFO("Trigger mode set successfully.");

    // 注册图像回调
    if (!regImageCallback(cameras, true, ImageCallBackEx, nullptr))
    {
        INFO("Failed to register image callback. Test failed.");
        closeCamera(cameras);
        return;
    }
    INFO("Image callback registered successfully.");

    // 开始抓图
    if (!startGrabbing(cameras))
    {
        INFO("Failed to start grabbing. Test failed.");
        regImageCallback(cameras, false, nullptr, nullptr);
        closeCamera(cameras);
        return;
    }
    INFO("Grabbing started successfully. Press Enter to stop...");

    // 等待用户输入停止抓图
    std::cin.get();

    // 停止抓图
    if (!stopGrabbing(cameras))
    {
        INFO("Failed to stop grabbing. Test failed.");
    }
    else
    {
        INFO("Grabbing stopped successfully.");
    }

    // 注销图像回调
    if (!regImageCallback(cameras, false, nullptr, nullptr))
    {
        INFO("Failed to unregister image callback.");
    }
    else
    {
        INFO("Image callback unregistered successfully.");
    }

    // 关闭相机
    if (!closeCamera(cameras))
    {
        INFO("Failed to close cameras. Test failed.");
    }
    else
    {
        INFO("Cameras closed successfully.");
    }

    INFO("Camera functionality test completed.");
}

int main()
{
    testCameraFunctionality();
    return 0;
}