# Live2D 桌面宠物

本项目使用 CMake 构建，基于 C++、Qt 以及 Live2D 官方 SDK 实现桌面宠物应用。

[模型规范](modelSpecification.md) 参考自带的 Haru 模型，若有问题请直接联系我。

## 原始项目

相关视频见 BiliBili：[Live2D 桌宠项目](https://www.bilibili.com/video/BV1TtkHYpEDA/)

## 改进内容

在[原项目](https://github.com/Misakiotoha/Live2D_ForC-Qt-OpenGL-_CMake)的代码基础上进行了以下改进：
1. 从 Qt5 移植到 Qt6
2. 添加点击穿透功能
3. 优化模型遮罩生成和检测算法
4. 右键菜单功能
5. 部分设置功能
6. 模型动态切换功能
7. 配置文件

## 部署环境

- **Qt**: Qt 6.8.2
- **C++**: C++17
- **操作系统**: Windows 10/11（暂不支持 Linux，因为点击穿透功能使用了 WinAPI，后期计划添加 Linux 支持，win10以下没试过，说不定可以用）
- **IDE**: 推荐使用 CLion 直接打开项目（这是 CMake 项目，无需使用 Qt Creator）

## 功能特点

- 实时模型动画展示
- 鼠标点击穿透
- 视线追踪
- 相比于Electron实现的桌宠，资源占用仅为1/20
~~用Kanban-Desktop做的对比，CPU占用确实是这么多，显卡大概是2倍，但是Kanban-Desktop用独显，这个用核显，占比也能差这么多~~
- 模型音频支持
- 模型音频字幕支持，字幕内容非动态生成，需模型内定义

***

## TODO
- [x] 添加右键菜单
- [x] 添加模型切换功能
- [x] 添加模型导入/删除菜单
- [x] 添加对某些模型的字幕支持
- [x] 添加模型自带的音频与待机动作支持
- [ ] 模型轮换
- [ ] 事件系统
- [ ] 番茄钟
- [ ] 添加完整设置功能
- [ ] 添加完整菜单
- [ ] 添加对某些模型的特殊支持
- [ ] 添加 Linux 支持
- [ ] 添加LLM支持