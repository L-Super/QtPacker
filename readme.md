# QtPacker 打包工具

## 介绍

Qt图形化自动化打包工具，针对基于Qt开发的程序进行打包。同时，使用ini配置文件记录上一次的操作，更加方便下次打包，避免重复性操作。

## 原理

通过匹配对应的windeployqt.exe路径，然后调用对应的编译器版本并执行打包命令。


## 使用说明
1. 选择Qt 的安装路径。
**不同版本路径有区别，未识别到编译器，请多尝试下一级路径或上一级。**
2. 编译器选择对应的工程所使用的
3. 软件路径即生成的可执行文件路径
4. 切勿保存到软件路径！选择新的存放路径

## TODO

- [x] 添加加载动画或进度条显示（目前有问题，未实现）
- [x] 打包完成后添加是否打开文件管理器查看已打包好的文件功能
- [x] 增加压缩包功能，自动将打包好的文件压缩
- [ ] 增加VC环境配置功能
- [ ] 压缩包第二次压缩时，提示是否删除原压缩文件
