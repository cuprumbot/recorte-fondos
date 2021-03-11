// OpenCV Demo.cpp: define el punto de entrada de la aplicación de consola.
//

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay" << endl;
		return -1;
	}
	Mat image;

	image = imread(argv[1], IMREAD_COLOR); // Read the file
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	Mat gray;
	resize(image, image, Size(960, 720));

	cvtColor(image, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(5, 5));
	Canny(gray, gray, 50, 100);
	
	int erosion_size = 20;
	Mat element = getStructuringElement(MORPH_ELLIPSE,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	dilate(gray, gray, element);
	erode(gray, gray, element);

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(gray, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
	cout << contours.size() << endl;

	vector< vector<Point> > hull(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], false);
	}

	vector< vector<Point> > appended(1);
	for (int i = 0; i < hull.size(); i++) {
		for (int j = 0; j < hull[i].size(); j++) {
			appended[0].push_back(hull[i][j]);
		}
	}

	/*
	Mat layer = cv::Mat::zeros(image.size(), CV_8UC3);
	Scalar color = Scalar(0, 0, 255);	// rojo
	for (int i = 0; i < contours.size(); i++) {
		drawContours(layer, hull, i, color, -1, 8, vector<Vec4i>(), 0, Point());
	}
	addWeighted(image, 0.6, layer, 1 - 0.4, 0, image);	// aplicar alpha
	*/

	Mat stencil = cv::Mat::zeros(image.size(), CV_8UC3);
	Scalar white = Scalar(255, 255, 255);

	vector< vector<Point> > appHull(1);
	convexHull(Mat(appended[0]), appHull[0], false);
	drawContours(stencil, appHull, -1, white, -1, 8, vector<Vec4i>(), 0, Point());

	/*
	for (int i = 0; i < appHull.size(); i++) {
		drawContours(stencil, appHull, i, white, -1, 8, vector<Vec4i>(), 0, Point());
	}
	*/
	bitwise_and(image, stencil, image);

	string fullPath = argv[1];
	string filename = fullPath.substr(fullPath.find_last_of("/\\") + 1);

	imwrite(".\\generated\\" + filename, image);

	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", image); // Show our image inside it.
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}