#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//include opencv core
#include "opencv2\core\core.hpp"
#include "opencv2\core.hpp"
#include "opencv2\face.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2\opencv.hpp"
#include <direct.h>


cv::CascadeClassifier Cascade;
std::string filename = "./faces/";
std::string face_haarcascade_file = "./haarcascade/haarcascade_frontalface_alt.xml";
std::string name;
int filenumber = 10;


void takeAttendence(int StudentCode) {

	//get current time
	time_t presentTime = time(NULL);
	char timeString [26];

	struct tm ltm;
	localtime_s(&ltm, &presentTime);

	//declare output file stream for excel sheet
	std::ofstream attendanceFile;

	//declare stringStream for file
	std::stringstream afss;

	//convert time_t to ctime
	ctime_s(timeString, sizeof timeString, &presentTime);

	std::cout << "Student code : " << StudentCode << " | Time : " << timeString << std::endl;

	afss << "./attendanceFolder/attendance-" << ltm.tm_mday << "-" << 1 + ltm.tm_mon << "-" << 1900 + ltm.tm_year << ".csv";

	//create excel sheet
	attendanceFile.open(afss.str(), std::ios_base::app);

	attendanceFile << StudentCode << "," << ltm.tm_hour << ":" << ltm.tm_min << ":" << ltm.tm_sec << "\n";

}

void detectAndDisplay(cv::Mat frame)
{
	std::vector<cv::Rect> faces;
	cv::Mat frame_gray;
	cv::Mat crop;
	cv::Mat res;
	cv::Mat gray;
	std::string text;
	std::stringstream sstm;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	Cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

	cv::Rect roi_b;
	cv::Rect roi_c;

	size_t ic = 0;
	int ac = 0;

	size_t ib = 0;
	int ab = 0;

	for (ic = 0; ic < faces.size(); ic++)
	{
		roi_c.x = faces[ic].x;
		roi_c.y = faces[ic].y;
		roi_c.width = (faces[ic].width);
		roi_c.height = (faces[ic].height);

		ac = roi_c.width * roi_c.height;

		roi_b.x = faces[ib].x;
		roi_b.y = faces[ib].y;
		roi_b.width = (faces[ib].width);
		roi_b.height = (faces[ib].height);


		crop = frame(roi_b);
		resize(crop, res, cv::Size(128, 128), 0, 0, cv::INTER_LINEAR);
		cvtColor(crop, gray, cv::COLOR_BGR2GRAY);
		std::stringstream ssfn;
		filename = "./faces/";
		ssfn << filename.c_str() << name << filenumber << ".png";
		filename = ssfn.str();

		imwrite(filename, res);
		filenumber++;


		cv::Point pt1(faces[ic].x, faces[ic].y);
		cv::Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
		rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2, 8, 0);
	}


	sstm << "Crop area size: " << roi_b.width << "x" << roi_b.height << " Filename: " << filename;
	text = sstm.str();

	if (!crop.empty())
	{
		imshow("detected", crop);
	}
	else
		cv::destroyWindow("detected");
}

void addFace()
{
	std::cout << "\nEnter Your Student Code:  ";
	std::cin >> name;

	cv::VideoCapture capture(0);

	if (!capture.isOpened()) {
		std::cout << "Cannot open camera" << std::endl;
		return;
	}

	if (!Cascade.load(face_haarcascade_file))
	{
		std::cout << "error" << std::endl;
		return;
	};

	cv::Mat frame;
	std::cout << "\nCapturing your face 10 times, keep Your head straight\n";
	//char key;
	int i = 0;

	_mkdir("./faces/");

	for (;;)
	{
		capture >> frame;

		//imshow("Camera", frame);
		detectAndDisplay(frame);
		i++;
		if (i == 10)
		{
			std::cout << "Face Added";
			break;
		}

		int c = cv::waitKey(10);

		if (27 == char(c))
		{
			break;
		}
	}

	return;
}

static void dbread(std::vector<cv::Mat>& images, std::vector<int>& labels) {
	std::vector<cv::String> fn;

	cv::glob("./faces/*.png", fn, false);

	size_t count = fn.size();

	for (size_t i = 0; i < count; i++)
	{
		std::string itsname = "";
		char sep = '\\';
		size_t j = fn[i].rfind(sep, fn[i].length() - 1);
		if (j != std::string::npos)
		{
			itsname = (fn[i].substr(j + 1, fn[i].length() - j - 7));
		}
		images.push_back(cv::imread(fn[i], 0));
		labels.push_back(atoi(itsname.c_str()));
	}
}

void eigenFaceTrainer() {
	std::vector<cv::Mat> images;
	std::vector<int> labels;
	dbread(images, labels);
	std::cout << "size of the images is " << images.size() << std::endl;
	std::cout << "size of the labels is " << labels.size() << std::endl;
	std::cout << "Training begins...." << std::endl;

	//create algorithm eigenface recognizer
	cv::Ptr<cv::face::EigenFaceRecognizer> model = cv::face::EigenFaceRecognizer::create();

	//train data
	model->train(images, labels);

	model->save("./eigenface.yaml");
	std::cout << "Training finished...." << std::endl;
	cv::waitKey(10000);
}

void  FaceRecognition() {

	std::cout << "\nstart recognizing...\n" << std::endl;

	//load pre-trained data sets
	cv::Ptr<cv::face::FaceRecognizer>  model = cv::face::FisherFaceRecognizer::create();
	model->read("./eigenface.yaml");

	_mkdir("./attendanceFolder/");

	cv::Mat testSample = cv::imread("./0.png", 0);

	int img_width = testSample.cols;
	int img_height = testSample.rows;
	std::vector<int> person;


	//lbpcascades/lbpcascade_frontalface.xml

	std::string window = "Face Recognition Attendance";

	if (!Cascade.load(face_haarcascade_file)) {
		std::cout << " \nError loading file\n" << std::endl;
		return;
	}

	cv::VideoCapture cap(0);
	//VideoCapture cap("C:/Users/lsf-admin/Pictures/Camera Roll/video000.mp4");

	if (!cap.isOpened())
	{
		std::cout << "\nexit\n" << std::endl;
		return;
	}

	//double fps = cap.get(CV_CAP_PROP_FPS);
	//std::cout << " Frames per seconds " << fps << std::endl;
	cv::namedWindow(window, 1);
	long count = 0;
	std::string Pname = "";

	while (true)
	{
		std::vector<cv::Rect> faces;
		cv::Mat frame;
		cv::Mat graySacleFrame;
		cv::Mat original;

		cap >> frame;
		//cap.read(frame);
		count = count + 1;//count frames;

		if (!frame.empty()) {

			//clone from original frame
			original = frame.clone();
			flip(original, original, 1);

			//convert image to gray scale and equalize
			cvtColor(original, graySacleFrame, cv::COLOR_BGR2GRAY);
			//equalizeHist(graySacleFrame, graySacleFrame);

			//detect face in gray image
			Cascade.detectMultiScale(graySacleFrame, faces, 1.1, 3, 0, cv::Size(90, 90));

			//number of faces detected
			//std::cout << faces.size() << " faces detected" << std::endl;
			std::string frameset = std::to_string(count);
			std::string faceset = std::to_string(faces.size());

			int width = 0, height = 0;

			//region of interest
			//cv::Rect roi;

			for (int i = 0; i < faces.size(); i++)
			{
				//region of interest
				cv::Rect face_i = faces[i];

				//crop the roi from grya image
				cv::Mat face = graySacleFrame(face_i);

				//resizing the cropped image to suit to database image sizes
				cv::Mat face_resized;
				cv::resize(face, face_resized, cv::Size(img_width, img_height), 1.0, 1.0, cv::INTER_CUBIC);

				//recognizing what faces detected
				int label = -1; double confidence = 0.0;
				model->setThreshold(4000.0);
				model->predict(face_resized, label, confidence);
				int label_predict = model->predict(face_resized);

				//std::cout << " confidence " << confidence << " Label: " << label_predict << std::endl;

				if (label_predict != -1) {
					Pname = std::to_string(label);
					if (find(person.begin(), person.end(), label) == person.end()) {
						std::cout << "Label : " << label << std::endl;
						person.push_back(label);
						takeAttendence(label);
					}
				}
				else {
					Pname = "Unknown";
				}

				//drawing green rectagle in recognize face
				rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
				std::string text = Pname;

				int pos_x = std::max(face_i.tl().x - 10, 0);
				int pos_y = std::max(face_i.tl().y - 10, 0);

				//name the person who is in the image
				putText(original, Pname, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(255, 255, 255), 1);

				//cv::imwrite("E:/FDB/"+frameset+".jpg", cropImg);

			}


			putText(original, "Framerate: " + frameset, cv::Point(0, 15), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 0, 0), 1);
			//putText(original, "No. of Persons detected: " + to_std::string(faces.size()), Point(30, 90), FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1);
			//display to the winodw
			imshow(window, original);

			//std::cout << "model infor " << model->getThreshold() << std::endl;

			//std::cout << (cv::getWindowProperty(window, cv::WND_PROP_VISIBLE));
		}

		if (cv::waitKey(30) >= 0){
			cv::destroyWindow(window);
			break;
		}
	}
}