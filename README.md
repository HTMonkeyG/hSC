# hSC Plugin
&emsp;**简体中文** | [English](https://github.com/HTMonkeyG/hSC/blob/main/README-en_US.md)

&emsp;hSC插件是为光遇国服PC开发的摄像机修改插件。
该插件集成坐标修改、灵魂出窍、航拍运镜于一体，为景点拍摄、视频录制等提供辅助。

**删除线内功能为开发中功能。本行粗体字被移除前该仓库所有特性均不稳定。**

## 一、插件功能
&emsp;注入后按下键盘上`~`键即可显示ImGui内置界面。
若在游戏中需要按住Alt显示鼠标指针后点击操作。<br>
&emsp;界面最上方复选框Take over为全局开关，勾选即接管相机。
下方下拉框选择要接管的相机模式：First person为第一人称手持模式，Front为前置模式，Placed为三脚架模式。
再下方的选择栏为运算类型：Set为直接设置模式，Freecam为灵魂出窍模式，FPV为无人机模式。

### 1.1 直接设置模式
&emsp;选择该选项后下方会出现5个输入框，其中包含坐标、朝向、对焦、缩放、亮度设置，直接输入数字即可。
勾选对应操作的复选框后输入才有效。<br>
&emsp;该模式适用于静态拍摄等情况。

### 1.2 灵魂出窍模式
&emsp;该模式可以使用wasd、空格、shift自由移动镜头，使用Speed输入框调整移动速度。<br>
&emsp;勾选Axial项时，摄像机的水平移动将平行于xOz面。

### 1.3 无人机模式
**该模式开发中，所有描述无法保证稳定性与兼容性**

&emsp;该模式中可以使用wasd与鼠标模拟操作无人机进行拍摄。

&emsp;完整使用手册请[点击此处](https://github.com/HTMonkeyG/hSC/blob/main/doc/manual.md)

## 二、使用方式
1. 下载仓库release界面下的最新版dll文件。
2. 使用任一dll注入工具在等待网易游戏与TGC图标闪过、弹出登录确认弹窗后，将dll文件注入进游戏进程内。注意dll注入工具需使用管理员权限开启。

## 三、编译方式
&emsp;本软件使用mingw64-15.1.0编译。下载仓库release界面下对应版本的Source Code (.zip)，解压后在Makefile同目录下新建名为dist的文件夹，运行`mingw32-make.exe`即可。首次编译需运行`mingw32-make.exe all`编译库文件。<br>
&emsp;本软件不保证可在其他版本的MinGW上正常编，或在低于Win10 22H2、高于Win11 24H2的操作系统上正常运行。如有兼容性问题请通过邮件联系。<br>
&emsp;本软件使用的MinHook经过修改以解决可能由光遇国服的VMP保护导致的hook失败，ImGui经过修改以允许在更新的MinGW SDK下编译。<br>
&emsp;**请勿使用非仓库内提供的第三方库版本。**

## 四、额外许可声明
&emsp;本软件在除[LICENSE](https://github.com/HTMonkeyG/hSC/blob/main/LICENSE)中声明内容之外，还遵循以下声明：<br>
&emsp;《光·遇》、《Sky光·遇》的一切合法权益（包括但不限于使用本软件产生的游戏内图片、音视频）归属于网易与ThatGameCompany，且适用于其对应用户条约与服务协议。<br>
&emsp;本软件和相关文档文件不是网易及ThatGameCompany的官方产品，未经上述二者批准或与上述二者无关。<br>
