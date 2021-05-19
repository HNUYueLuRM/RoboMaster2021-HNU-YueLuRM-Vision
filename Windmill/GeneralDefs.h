#ifndef __GENERAL_DEFS__
#define __GENERAL_DEFS__

namespace hnurm {
	
	// 风车旋转方向
	// CLOCKWISE : 当前相机观察到风车在顺时针旋转
	// COUNTERCLOCKWISE ： 当前相机观察到风车在逆时针旋转
	enum RotateDirection
	{
		CLOCKWISE = 0, COUNTERCLOCKWISE
	};

	struct RotateInfo
	{
		RotateDirection r_direc;
		double angular_speed;
	};

	enum TeamColor
	{
		BLUE = 0, RED
	};

	// (u, v) 是像素坐标系下的坐标
	// u, v 均为 10 进制 整数
	struct PixelPoint2d
	{
		PixelPoint2d(int uu, int vv) :
			u(uu), v(vv)
		{};
		int u, v;
	};

	// focal_length: 焦距
	// resolution_width: 抓取的图像的宽度
	// resolution_height: 抓取的图像的高度
	// fps: 相机拍摄时 | 视频流的帧率
	struct CamParams
	{
		int focal_length;
		int resolution_width;
		int resolution_height;
		int fps;
	};

}
#endif // !GENERAL_DEFS
