# HiKVision相机控制系统

这是一个基于海康威视SDK的相机控制系统，用于控制和管理海康威视工业相机。

## 功能特点

- 相机设备枚举与连接
- 触发模式设置
- 图像回调注册与处理
- 相机抓图控制
- 多相机并行控制

## 系统要求

- Windows操作系统
- 海康威视MV SDK (MvCameraControl.h)
- Visual Studio开发环境

## 使用方法

### 初始化相机

```cpp
// 枚举设备
MV_CC_DEVICE_INFO_LIST stDeviceList = enumDevice();

// 创建相机并打开
std::vector<CMvCamera> cameras = createCamera(stDeviceList);

// 设置触发模式（默认开启）
setTriggerMode(cameras, true);
```

### 注册图像回调

```cpp
// 定义回调函数
void __stdcall ImageCallBackEx(unsigned char *pData, MV_FRAME_OUT_INFO_EX *pFrameInfo, void *pUser)
{
    // 处理图像数据
}

// 注册回调
regImageCallback(cameras, true, ImageCallBackEx, nullptr);
```

### 开始/停止抓图

```cpp
// 开始抓图
startGrabbing(cameras);

// ... 业务逻辑 ...

// 停止抓图
stopGrabbing(cameras);
```

### 关闭相机

```cpp
// 关闭相机
closeCamera(cameras);
```

## 项目结构

- `HikVisonCameraControl.h/cpp`: 相机控制的主要接口
- `MvCamera.h/cpp`: 对海康威视SDK的C++封装
- `main.cpp`: 主程序入口
- `utils.h`: 工具函数

## 注意事项

- 使用前请确保已正确安装海康威视相机驱动
- 请参考海康威视官方文档了解更多SDK功能
- 使用过程中请注意相机连接状态