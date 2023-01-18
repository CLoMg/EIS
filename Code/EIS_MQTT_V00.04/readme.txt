EIS_MQTT Project
Software Version：V00.04
Hardware：V0.4
data： 2022/01/18
水口隧道在用程序
Modified：
1.去掉设备自己订阅主题，由中继统一订阅
To do：
1.频繁重启，怀疑是内存泄漏
Profile：
1.疏散指示方向控制
2.scd4x 温度、湿度、co2数据采集
3.uart+lora模块 传输数据至中继
4.mqtt协议数据传输
5.rtc 时间戳毫秒获取
6.DSL_08 固体颗粒物浓度数据采集
7.组设备分时间窗口发送数据
