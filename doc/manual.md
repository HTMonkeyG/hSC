# hSC插件使用手册
&emsp;该文档用于介绍hSC插件中各个设置选项的作用及使用方式。<br>
&emsp;**由于软件仍处于开发阶段，该文档的内容可能随时变化，请以对应版本的文档为准。**

## 一、主菜单`hSC Main`
&emsp;进入游戏、按下`F1`键后，会出现一个ImGui窗口，标题为`hSC Main`。该窗口即为主菜单。<br>
&emsp;主菜单中第一个复选框`Enable plugin`为插件全局开关，只有在该复选框被勾选时插件才会工作。下方的下拉菜单`Overriden mode`为待接管的视角模式选择，将相机切换至对应模式，插件便会接管该视角下的相机运算。<br>
&emsp;复选框`No UI`用于隐藏相机的原生操作界面。隐藏后相机参数将无法直接通过鼠标滚轮调节，同时无法按下`X`键进行快捷拍照。<br>
&emsp;标签栏为操作模式，`Static`为静态模式、`Dynamic`为动态模式，`Animation`为关键帧模式。<br>
&emsp;再次按下`F1`可隐藏全部菜单。

## 二、静态模式`Static`
### 2.1 直接设置参数`Set Camera Params`
&emsp;处在静态模式时，相机的各式参数均可直接输入。若需要使修改生效，需勾选参数前的复选框。<br>
&emsp;`Pos`项的三个输入框为相机坐标输入，`Rotation`项三个输入框分别为相机偏航、俯仰、滚转角的输入，单位为度。<br>
&emsp;`Scale`、`Focus`与`Brightness`为相机的缩放、景深与亮度三个可调参数。输入范围均为0~1，若超出范围可能会导致错误渲染。

### 2.2 自由相机参数`Freecam params`
&emsp;静态模式中可使用WASD、Shift、空格等按键自由移动相机。默认情况下，WS为前后移动，AD为左右移动，Shift及空格为上下移动。移动方向均相对于相机当前朝向。<br>
&emsp;使用QE可左右滚转相机。<br>
&emsp;通过`Disable camera movement`与`Disable camera rotation`复选框可禁用自由相机的移动或旋转，此时键盘与鼠标输入不会被拦截，而是直接传递给游戏。<br>
&emsp;勾选`Check collision`复选框将会使自由相机尝试检测与场景的碰撞。**该项为实验性功能，可能导致意料之外的结果**。<br>
&emsp;`Rotate speed`可调节按下QE时的相机旋转速度，单位为rad/s，`Movement speed`可调节相机的移动速度。

## 三、动态模式`Dynamic`
&emsp;该模式正在开发中，选择后可能导致意料之外的后果。

## 四、关键帧模式`Animation`
&emsp;该模式正在开发中，选择后无效。

## 五、设置菜单`Settings`
### 5.1 控制设置`Control settings`
- `Mouse sensitivity`：用于调节全局鼠标灵敏度。只有完全被接管的操作模式该项才有效（如动态模式或勾选了Rot的静态模式），否则将使用游戏内灵敏度。
- `Vertical sensitivity scale`：垂直灵敏度倍率，限制与鼠标灵敏度相同。<br>
- `Swap yaw and roll`：交换滚转与偏航，勾选后滚转与偏航的操作模式将被交换。对于一部分人而言勾选该项会使操作更简单。
