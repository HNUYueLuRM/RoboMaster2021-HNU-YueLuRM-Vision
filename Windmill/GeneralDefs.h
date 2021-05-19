#ifndef __GENERAL_DEFS__
#define __GENERAL_DEFS__

namespace hnurm {
	
	// �糵��ת����
	// CLOCKWISE : ��ǰ����۲쵽�糵��˳ʱ����ת
	// COUNTERCLOCKWISE �� ��ǰ����۲쵽�糵����ʱ����ת
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

	// (u, v) ����������ϵ�µ�����
	// u, v ��Ϊ 10 ���� ����
	struct PixelPoint2d
	{
		PixelPoint2d(int uu, int vv) :
			u(uu), v(vv)
		{};
		int u, v;
	};

	// focal_length: ����
	// resolution_width: ץȡ��ͼ��Ŀ��
	// resolution_height: ץȡ��ͼ��ĸ߶�
	// fps: �������ʱ | ��Ƶ����֡��
	struct CamParams
	{
		int focal_length;
		int resolution_width;
		int resolution_height;
		int fps;
	};

}
#endif // !GENERAL_DEFS
