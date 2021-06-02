#include "Armor_tracker.h"
namespace hnurm {
    //std::string kernel_type = "gaussian";	// �˺���������
    //std::string feature_type = "hog";	//������ȡ�ķ�ʽ
	
	Armor_tracker::Armor_tracker() {
		// ��ʼ��kcf_tracker
        _kcf_tracker = make_unique<KCF>("gaussian", "hog");
        //KCF kcf_tracker("gaussian", "hog");
        //_kcf_tracker = &kcf_tracker;

	}

    void hnurm::Armor_tracker::init_tracker(const Rect& roi, const cv::Mat& image) {
		//	��ʼ��_kcf_tracker
		_kcf_tracker->Init(image, roi);
	}
	// ��ȡ��������ĵ�
	static Point2d Get_Rect_center(const Rect2d& rect) {
		Point2d center;
		center.x = rect.x + cvRound(rect.width / 2);
		center.y = rect.y + cvRound(rect.height / 2);
		return center;
	}


	//  ����������ص��Ǿ���
	//	�ڲ����У�ʹ���������
	Rect hnurm::Armor_tracker::Update(cv::Mat& image) {
		Rect result_rect;
		result_rect = _kcf_tracker->Update(image);

		return result_rect;
	}
	

	//����������ص��Ǿ�������ĵ�
	//��ʵ��Ӧ��ʱ��ʹ���������
	Point2d hnurm::Armor_tracker::Update_and_Get_center(cv::Mat& image) {
		return Get_Rect_center(_kcf_tracker->Update(image));
	}




	// ���ڲ���
	// ��ȡgroundtruth.txt�ļ���ÿ������
    /*static std::vector<cv::Rect> GetGroundtruth(std::string txt_file) {
		// �洢�ļ��е�ÿ������
		std::vector<cv::Rect> rect;
		ifstream gt;
		gt.open(txt_file.c_str());
		if (!gt.is_open())	//����ļ��򲻿�
			std::cout << "Ground truth file " << txt_file
			<< " can not be read" << std::endl;
		std::string line;
		int tmp1, tmp2, tmp3, tmp4;
		// ���ζ�ȡ�ļ��е�ÿһ��
		while (getline(gt, line)) {
			// ���ļ��еĶ����滻�ɿո�
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
		��ʵ�ʵ�Ӧ���У�
		1�����Ƚ���װ�װ��⣬�����ɹ���ʱ�򣬷���һ������Ϊbounding_box
		2��Ȼ�����Ŀ�����ֻ��Ҫ����һ��Armor_tracker����� my_tracker
		3��Ȼ����ö����е�init_tracker����
			my_tracker.init_tracker(bounding_box, image)	imageΪ��ǰ֡�Ļ���
		4��Ȼ����֮���ÿһ֡����Update_and_Get_center��������ÿһ֡����Ŀ����������
			my_tracker.Update_and_Get_center(image)			imageΪ��ǰ֡�Ļ���
	*/


	// ���԰������������ֻ���ڲ��ԣ����Ծ�ֱ�Ӳ��ö�ȡ�ļ��еķ�ʽ��֮��Ŀ��������и�Ϊ������ͷ��ȡ
    /*void hnurm::Armor_tracker::test_case(string video_path, string groundtruth_path) {


		//	��ȡ·�������е�jpg�ļ�
		std::string pattern_jpg = video_path + "/img/*.jpg";
		std::vector<cv::String> image_files;
		cv::glob(pattern_jpg, image_files);
		if (image_files.size() == 0) cout << "No images found" << std::endl;



		// ��ȡgroundtruth_rect.txt���ļ�·��
		std::string txt_path = groundtruth_path + "/groundtruth_rect.txt";
		std::vector<cv::Rect> groundtruth_rect;



		// ��ȡgroundtruth_rect
		groundtruth_rect = GetGroundtruth(txt_path);
		cv::Mat image;
		std::vector<cv::Rect>result_rect;
		int64 tic, toc;
		double time = 0;

		// ����ÿһ��ͼƬ
		for (unsigned int frame = 0; frame < image_files.size(); ++frame) {
			// ��ȡͼƬ
			image = cv::imread(image_files[frame]);
			tic = cv::getTickCount();
			if (frame == 0) {
				//	����ǵ�һ֡ͼ����ô�ͳ�ʼ��tracker
				Armor_tracker::init_tracker(groundtruth_rect[0], image);
				result_rect.push_back(groundtruth_rect[0]);
			}
			else {
				//	������ǵ�һ֡����ô�͸���Ŀ���
				result_rect.push_back(Armor_tracker::Update(image));
			}
			toc = cv::getTickCount() - tic;
			time += toc;
			//	��������
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
