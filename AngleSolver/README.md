# 位姿计算

## 1、文件依赖

```c++
General.h
PoseCalculation.h
PoseCalculation.cpp
ArmorParams.xml
Camera_data.xml
```

## 2、接口

##### 2.1  void  solve(Armors);

说明：计算yaw、pitch和distance

参数：经过图像检测得到的装甲板识别结果（颜色、大小、四个角点的像素坐标），是一个ArmorInfo类(定义在General.h中)

注意：ArmorInfo类中，std::vector<cv::Point2d>  object_points定义的四个角点的顺序依次如下：

```
 【1】—————————【2】
 |         		|
 |         		|
 |         		|
 【4】—————————【3】
```



##### 2.2  double  getYaw();

说明：获取x轴方向转角yaw，角度制单位

##### 2.3  double  getPitch();

说明：获取y轴方向转角pitch，角度值单位

##### 2.4  cv::Mat  getTvec();

说明：获取旋转矩阵Tvec 

##### 2.5 double getDistance();

 说明：获取摄像头与装甲板中心的距离，单位：mm^3



## 3、示例代码

见文件“test_PoseCalculation.cpp”

```c++
#include "PoseCalculation.h"
#include "General.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
using namespace hnurm;

int main() {

	//初始化参数
	ArmorInfo Armors;
	Armors.color = BLUE;
	Armors.size = BIG;
	Armors.object_points.push_back(cv::Point2f(62.097, 206.647));
	Armors.object_points.push_back(cv::Point2f(297.774, 200.077));
	Armors.object_points.push_back(cv::Point2f(59.3742, 108.977));
	Armors.object_points.push_back(cv::Point2f(295.051, 102.407));


	//位姿计算
	AngleSolver ANGLESOLVER1;			//建立一个位姿计算器
	ANGLESOLVER1.init();				//读入相机参数和装甲板参数，初始化位姿计算器
	ANGLESOLVER1.solve(Armors);			//传输pixel坐标,计算
	

	//输出
    std::cout << "yaw =  " << ANGLESOLVER1.getYaw() << std::endl << std::endl;
    std::cout << "pictch =  " << ANGLESOLVER1.getPitch() << std::endl<<std::endl;
    std::cout << "Tvec : \n" << ANGLESOLVER1.getTvec() << std::endl<<std::endl;
    std::cout << "distance:" << ANGLESOLVER1.getDistance() <<"( mm^3 )"<< std::endl;


	return 0;
}

```

