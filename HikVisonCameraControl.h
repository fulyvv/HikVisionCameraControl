#pragma once

#include "MvCamera.h"

#include <string>
#include <iostream>
#include <vector>

// 枚举设备
MV_CC_DEVICE_INFO_LIST enumDevice();
// 创建相机并打开
std::vector<CMvCamera> createCamera(MV_CC_DEVICE_INFO_LIST& stDeviceList);
// 设置触发模式，默认打开触发模式
bool setTriggerMode(std::vector<CMvCamera>& cameras, bool triggermode_on = true);

// 设置和注销图像回调，reg为true时注册，为false时注销，需要配合回调函数ImageCallBackEx
/*回调函数原型
void __stdcall ImageCallBackEx(unsigned char *pData, MV_FRAME_OUT_INFO_EX *pFrameInfo, void *pUser)
{
    printf("callback\n");
}
*/
bool regImageCallback(std::vector<CMvCamera>& cameras, bool reg,
    void(__stdcall* ImageCallBackEx)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);
// 开始抓图
bool startGrabbing(std::vector<CMvCamera>& cameras);
// 停止抓图
bool stopGrabbing(std::vector<CMvCamera>& cameras);
// 关闭相机
bool closeCamera(std::vector<CMvCamera>& cameras);

void connectCamera(bool triggermode_on, void(__stdcall* ImageCallBackEx)(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);