# QtPacker 打包工具

## 介绍

Qt图形化自动化打包工具，针对基于Qt开发的程序进行打包，旨在避免每次打包时，从开始菜单打开Qt命令行使用`windeployqt`命令进行打包。同时，使用ini配置文件记录上一次的操作，更加方便下次打包，避免重复性操作。

仅限于Windows平台，Linux平台机制不同，未自带`windeployqt`工具。

## 原理

通过匹配对应的windeployqt.exe路径，然后调用对应的编译器版本并执行打包命令。


## 使用说明
1. 选择Qt 的安装路径。
  **不同版本路径有区别，未识别到编译器，请多尝试下一级路径或上一级。**
2. 编译器选择对应的工程所使用的
3. 软件路径即生成的可执行文件路径

## TODO
- [ ] 增加VC环境配置功能
- [ ] 查找Qt安装路径时，使用递归

---

待解决的问题（but 忘了解决什么问题😂）

考虑怎样的形式调用更好

1. 修改变量
2. 直接读取bat内容，构造新的bat

```bash
@echo off
::E:/Code/QtProject/qt-packer/build-QtPackTool-Desktop_Qt_5_15_2_MinGW_64_bit-Debug/debug/QtPacker.exe
@REM You can not have spaces on either side of an equal sign
set envPath=D:/Qt/5.15.2/mingw81_64/bin/qtenv2.bat
set APP=E:/Code/QtProject/untitled/a/QtPacker.exe

call %envPath%

@REM echo Setting up environment for Qt usage...
@REM set PATH=D:\Qt\5.15.2\mingw81_64\bin;D:/Qt/Tools/mingw810_64\bin;%PATH%
@REM cd /D D:\Qt\5.15.2\mingw81_64

windeployqt %APP%

pause
```

