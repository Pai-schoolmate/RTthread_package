# ADS1146
这是一个可以驱动Ti的ADS1146芯片的软件包，具有PGA设置和PGA自动模式，能实现对外部电压的微伏测量。
这里是部分信息，完整信息可以看我的RTthread论坛文章https://club.rt-thread.org/ask/article/05496099372f275d.html
## 1、ads1146.c
**第一个函数`void SPI_device_creation(void)`**
用来对SPI设备初始化，因为ADS1146是通过SPI通讯的，所以需要完成对应的SPI初始化，这里通过预编译指令类比写了SPI1、SPI2、SPI3三个外设初始化，基本能囊括所有SPI设备了，如实际中超出，例如使用的是SPI4，可按模板自行添加。
注意`board.h`要开启对应SPI的宏定义。
代码主要工作完成了ADS1146的SPI片选引脚确认、通信模式、通信字节、通信速度的设置。其中片选引脚需要更改为自己实际使用的引脚，通信速度不建议过高，其他内容不建议更改。

**第二个函数`void ADS1146_init(void)`**
完成ADS1146的初始化，主要是向ADS1146发送了三个命令。
`0x01`：将ADS1146从睡眠中唤醒，
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/025c0c46e9334ec1b997297611170aab.png)
`0x06`：将ADS1146复位，准备下面的工作。（注：因为ADS1146复位需要0.6ms时间，这个时间段不能继续发送命令，我这给了10ms）
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/6a37a9b800a54019807a4784d9a26205.png)
`0x16`：停止数据读取，当前配置未完成，防止错误数据输出（保险，可加可不加）
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/208880f1de914c5fb0e5f32d1050cf53.png)

**第三个函数`void ADS1146_setPGA(struct rt_spi_device *device,short ADS_setPGA)`**
该函数有两个形参，device：传入的设备名；ADS_setPGA：需要设置的PGA十进制值
该函数能够将设置的PGA值写入ADS1146，主要命令是`0x43`。
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7e46706dc0934a60a9cdd15fc4d81ff3.png)
`0x43`表示写入的寄存器如下
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/34ba714e1053408c913fdff898ce7384.png)
`0x04 0x04`表示开始转换
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/087e6f10b1e144f18975db4f9ae25915.png)

**第四个函数`short ADS1146_readPGA(struct rt_spi_device *device)`**
device：传入的设备名
该函数能够将传入设备读取的PGA值以十进制返回，主要命令是`0x23`
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d977f581558741a9b09c782f4a724aa5.png)

**第五个函数`short ADS1146_PGA_AUTO(struct rt_spi_device *device)`**
evice：传入的设备名
该函数属于最核心函数，能够根据测量的数值自动调节该设备PGA，从而达到自动调节量程，实现最精准测量。并且我还特别优化，能实现快速响应。这部分代码不讲解，能理解就是你的，理解不了直接用就行。

**第六个函数`void ADS1146_output(struct rt_spi_device *device,float calibration_k,float calibration_b,short PGA_flag);`**
实现ADS1146采集的电压值输出，单位mV
device：传入的设备名;
 calibration_k, calibration_b: 校准参数
 校准函数：ADS_CCD_mV = calibration_k*ADS_mV + calibration_b
 PGA_flag：量程标志，0：正常，1：超量程。搭配ADS1146_PGA_AUTO使用可实现自动判断

这一个函数就是将读取的电压值输出出来，我这里直接rt_printf，如果需要返回值，可自行拓展。其中 calibration_k, calibration_b是校准参数，我们知道实际值和测量值之间是会存在误差的，这些误差并非都是随机误差，其中很大一部分是具有线性特征的系统误差，这部分误差可以通过一个线性函数来弥补，下图是我通过采集的实际值和测量值计算出来的一次函数，带入函数也就是calibration_k = 0.9994，calibration_b = -0.0045
不过这写数据实在PGA = 8恒定下采集的值，为了省事，我直接运用到了全局PGA中，所以在PGA = 8时校准效果最好，不过应用到其他PGA上也有不错的效果，如果大家有需要，可以将各个PGA下的校准函数求出，搭配ADS1146_PGA_AUTO函数实现更加精准测量。
## 2、ads1146.h
这里面就是各种宏定义，函数声明。因为我做了大量SPI适配的预编写语句，所以在board.h开启SPI宏定义后，这边再开启对应的ADS1146宏定义就能直接使用，开启了SPI宏定义不开启ADS1146宏定义不会对该SPI有任何影响，其他设备正常使用。
```c
//需要在board.h中开启对于的SPI预编译
#define BSP_USING_SPI1_ADS1146
#define BSP_USING_SPI2_ADS1146
//#define BSP_USING_SPI3_ADS1146
```
## 3、应用实例
```c
#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "ads1146.h"
#include "ads_can.h"

int main(void)
{
    short PGA_flag1;
    SPI_device_creation();
    ADS1146_init();
    while(1){
        PGA_flag1 = ADS1146_PGA_AUTO(ads1146_handle1);
        ADS1146_output(ads1146_handle1, 0.9994, -0.0045, PGA_flag1);
        rt_thread_mdelay(800);
    }
    return RT_EOK;
}

```
只需要以上代码就能实现最简单的使用了。
