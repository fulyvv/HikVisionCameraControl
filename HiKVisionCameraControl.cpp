#include "MvCamera.h"

#include <string>
#include <iostream>
#include <vector>

#include "utils.h"



bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        INFO("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        INFO("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
        INFO("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        INFO("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
        INFO("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
        INFO("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
    }
    else
    {
        INFO("Not support.\n");
    }

    return true;
}

MV_CC_DEVICE_INFO_LIST enumDevice()
{
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        INFO("Enum Devices fail! nRet [0x%x]\n", nRet);
        return {};
    }

    if (stDeviceList.nDeviceNum > 0)
    {
        for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            INFO("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            }
            PrintDeviceInfo(pDeviceInfo);
        }
    }
    else
    {
        INFO("Find No Devices!\n");
        return {};
    }
    return stDeviceList;
}

std::vector<CMvCamera> createCamera(MV_CC_DEVICE_INFO_LIST& stDeviceList)
{
    int nRet = MV_OK;
    std::vector<CMvCamera> cameras;
    cameras.reserve(stDeviceList.nDeviceNum);
    for (int i = 0; i < stDeviceList.nDeviceNum; i++)
    {
        CMvCamera camera;
        cameras.emplace_back(camera);
        nRet = cameras[i].Open(stDeviceList.pDeviceInfo[i]);
        if (MV_OK != nRet)
        {
            INFO("Open Camera %d fail! nRet [0x%x]\n", i, nRet);
            return {};
        }
    }
    INFO("Open Camera success!\n");
    return cameras;
}

bool setTriggerMode(std::vector<CMvCamera>& cameras, bool triggermode_on)
{
    int nRet = MV_OK;
    int triggermode = triggermode_on ? 1 : 0; // 0: off, 1: on
    for (int i = 0; i < cameras.size(); i++)
    {
        nRet = cameras[i].SetEnumValue("TriggerMode", triggermode);
        if (MV_OK != nRet)
        {
            INFO("Set Camera [%d] TriggerMode fail! nRet [0x%x]\n", i, nRet);
            return false;
        }
    }
    INFO("Set TriggerMode success!\n");
    return true;
}

// 注册和注销图像回调函数 bool reg = true 注册，false 注销
bool regImageCallback(std::vector<CMvCamera>& cameras, bool reg,
    void(__stdcall* ImageCallBackEx)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
    int nRet = MV_OK;
    if (reg == true)
    {
        for (int i = 0; i < cameras.size(); i++)
        {
            nRet = cameras[i].RegisterImageCallBack(ImageCallBackEx, pUser);
            if (MV_OK != nRet)
            {
                INFO("Register Camera [%d] ImageCallBackEx fail! nRet [0x%x]\n", i, nRet);
                return false;
            }
        }
        INFO("Register ImageCallBackEx success!\n");
    }

    if (reg == false)
    {
        for (int i = 0; i < cameras.size(); i++)
        {
            nRet = cameras[i].RegisterImageCallBack(NULL, NULL);
            if (MV_OK != nRet)
            {
                INFO("Unregister Camera [%d] ImageCallBackEx fail! nRet [0x%x]\n", i, nRet);
                return false;
            }
        }
        INFO("Unregister ImageCallBackEx success!\n");
    }
    return true;
}

// 开启抓图
bool startGrabbing(std::vector<CMvCamera>& cameras)
{
    int nRet = MV_OK;
    for (int i = 0; i < cameras.size(); i++)
    {
        nRet = cameras[i].StartGrabbing();
        if (MV_OK != nRet)
        {
            INFO("Start Camera [%d] Grabbing fail! nRet [0x%x]\n", i, nRet);
            return false;
        }
    }
    INFO("Start Grabbing success!\n");
    return true;
}

// 停止抓图
bool stopGrabbing(std::vector<CMvCamera>& cameras)
{
    int nRet = MV_OK;
    for (int i = 0; i < cameras.size(); i++)
    {
        nRet = cameras[i].StopGrabbing();
        if (MV_OK != nRet)
        {
            INFO("Stop Camera [%d] Grabbing fail! nRet [0x%x]\n", i, nRet);
            return false;
        }
    }
    INFO("Stop Grabbing success!\n");
    return true;
}

// 关闭相机
bool closeCamera(std::vector<CMvCamera>& cameras)
{
    int nRet = MV_OK;
    for (int i = 0; i < cameras.size(); i++)
    {
        nRet = cameras[i].Close();
        if (MV_OK != nRet)
        {
            printf("Close Camera [%d] fail! nRet [0x%x]\n", i, nRet);
            return false;
        }
    }
    INFO("Close Camera success!\n");
    return true;
}


void connectCamera(bool triggermode_on, void(__stdcall* ImageCallBackEx)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
    MV_CC_DEVICE_INFO_LIST stDeviceList = enumDevice();
    if (stDeviceList.nDeviceNum == 0) return;

    std::vector<CMvCamera> cameras = createCamera(stDeviceList);
    if (cameras.size() == 0) return;

    if (setTriggerMode(cameras, triggermode_on) == false) return;
    if (regImageCallback(cameras, true, ImageCallBackEx, pUser) == false) return;
    if (startGrabbing(cameras) == false) return;
    std::cin.get();

    if (stopGrabbing(cameras) == false) return;
    if (regImageCallback(cameras, false, ImageCallBackEx, nullptr) == false) return;

    if (closeCamera(cameras) == false) return;
}

// g++ cam_test.cpp MvCamera.cpp -o cam -L/opt/MVS/lib/64 -lMvCameraControl -lopencv_core -lopencv_imgcodecs