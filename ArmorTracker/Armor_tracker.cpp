#include "Armor_tracker.h"
namespace hnurm {
    //std::string kernel_type = "gaussian";	// 核函数的类型
    //std::string feature_type = "hog";	//特征提取的方式
	
	Armor_tracker::Armor_tracker() {
		// 初始化kcf_tracker
        _kcf_tracker = make_unique<KCF>("gaussian", "hog");
        //KCF kcf_tracker("gaussian", "hog");
        //_kcf_tracker = &kcf_tracker;

	}

    void hnurm::Armor_tracker::init_tracker(const Rect& roi, const cv::Mat& image) {
		//	初始化_kcf_tracker
		_kcf_tracker->Init(image, roi);
	}
	// 获取矩阵的中心点
	static Point2d Get_Rect_center(const Rect2d& rect) {
		Point2d center;
		center.x = rect.x + cvRound(rect.width / 2);
		center.y = rect.y + cvRound(rect.height / 2);
		return center;
	}


	//  这个函数返回的是矩阵
	//	在测试中，使用这个函数
	Rect hnurm::Armor_tracker::Update(cv::Mat& image) {
		Rect result_rect;
		result_rect = _kcf_tracker->Update(image);

		return result_rect;
	}
	

	//这个函数返回的是矩阵的中心点
	//在实际应用时，使用这个函数
	Point2d hnurm::Armor_tracker::Update_and_Get_center(cv::Mat& image) {
		return Get_Rect_center(_kcf_tracker->Update(image));
	}




	// 用于测试
	// 获取groundtruth.txt文件中每个矩阵
    /*static std::vector<cv::Rect> GetGroundtruth(std::string txt_file) {
		// 存储文件中的每个矩阵
		std::vector<cv::Rect> rect;
		ifstream gt;
		gt.open(txt_file.c_str());
		if (!gt.is_open())	//如果文件打不开
			std::cout << "Ground truth file " << txt_file
			<< " can not be read" << std::endl;
		std::string line;
		int tmp1, tmp2, tmp3, tmp4;
		// 依次读取文件中的每一行
		while (getline(gt, line)) {
			// 把文件中的逗号替换成空格
			std::replace(line.begin(), line.end(), ',', ' ');
			stringstream ss;
			ss.str(line);
			ss >> tmp1 >> tmp2 >> tmp3 >> tmp4;
			rect.push_back(cv::Rect(--tmp1, --tmp2, tmp3, tmp4));
		}
		gt.close();
		return rect;
    }*/
	/*
		在实际的应用中：
		1：首先进行装甲板检测，当检测成功的时候，返回一个矩形为bounding_box
		2：然后进行目标跟踪只需要定义一个Armor_tracker类对象 my_tracker
		3：然后调用对象中的init_tracker函数
			my_tracker.init_tracker(bounding_box, image)	image为当前帧的画面
		4：然后在之后的每一帧调用Update_and_Get_center函数返回每一帧跟踪目标具体的坐标
			my_tracker.Update_and_Get_center(image)			image为当前帧的画面
	*/


	// 测试案例，这个函数只用于测试，所以就直接采用读取文件夹的方式，之后的开发过程中改为从摄像头读取
    /*void hnurm::Armor_tracker::test_case(string video_path, string groundtruth_path) {


		//	获取路径下所有的jpg文件
		std::string pattern_jpg = video_path + "/img/*.jpg";
		std::vector<cv::String> image_files;
		cv::glob(pattern_jpg, image_files);
		if (image_files.size() == 0) cout << "No images found" << std::endl;



		// 获取groundtruth_rect.txt的文件路径
		std::string txt_path = groundtruth_path + "/groundtruth_rect.txt";
		std::vector<cv::Rect> groundtruth_rect;



		// 获取groundtruth_rect
		groundtruth_rect = GetGroundtruth(txt_path);
		cv::Mat image;
		std::vector<cv::Rect>result_rect;
		int64 tic, toc;
		double time = 0;

		// 遍历每一张图片
		for (unsigned int frame = 0; frame < image_files.size(); ++frame) {
			// 读取图片
			image = cv::imread(image_files[frame]);
			tic = cv::getTickCount();
			if (frame == 0) {
				//	如果是第一帧图像，那么就初始化tracker
				Armor_tracker::init_tracker(groundtruth_rect[0], image);
				result_rect.push_back(groundtruth_rect[0]);
			}
			else {
				//	如果不是第一帧，那么就更新目标框
				result_rect.push_back(Armor_tracker::Update(image));
			}
			toc = cv::getTickCount() - tic;
			time += toc;
			//	画出矩形
			cv::rectangle(image, result_rect[frame], cv::Scalar(0, 255, 255), 2);
			cout << Get_Rect_center(result_rect[frame]) << endl;
			cv::imshow("This is a test", image);
			char key = cv::waitKey(1);
			if (key == 27 || key == 'q' || key == 'Q')
				break;

		}
		time = time / double(getTickFrequency());
		double fps = double(result_rect.size()) / time;
		cout << fps << endl;


    }*/
}
