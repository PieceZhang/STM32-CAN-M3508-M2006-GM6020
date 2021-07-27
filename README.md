# STM32-CAN-M3508-M2006-GM6020

使用robomaster官方C板基于CAN总线驱动M2006电机（M3508、M2006、GM6020均可使用），程序使用CubeMX基于1.25.0版固件库生成，大部分核心程序参考自官方例程。  

主要工作为将官方使用的1.18.0版CubeMX固件使用1.25.0版固件改写，改写了已经弃用的一些函数和宏，仍使用中断接收。

# 参考资料
Robomaster C板： https://www.robomaster.com/zh-CN/products/components/general/development-board-type-c#downloads  

M2006：https://www.robomaster.com/zh-CN/products/components/general/M2006
