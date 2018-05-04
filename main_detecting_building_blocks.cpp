#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int rectangle_side_epsilon = 10;
int intersection_threshold = 80;
char* window_demo = "Demo";

Mat src;					// input image
Mat watershed_seeds;		// marker seeds used for watershed
int	labels;					// number of regions found
vector<Vec4i> lines;		// houghlines
vector<Vec3b> colorTab;		// list of colors
int x_median, y_median;		// grid median sizes
vector<Vec4i> boundRect;	// output: list of detected building blocks

void demo(int, void*)
{
	// paint the watershed image and compute bounding boxes
	Mat wshed(watershed_seeds.size(), CV_8UC3);
	vector<Vec4i> boundRect(labels, Vec4i(INT_MAX, INT_MAX, 0, 0));
	for (int i = 0; i < watershed_seeds.rows; i++)
		for (int j = 0; j < watershed_seeds.cols; j++)
		{
			int index = watershed_seeds.at<int>(i, j);
			if (index == -1)
				wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			else if (index <= 0 || index > labels)
				wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			else
			{
				wshed.at<Vec3b>(i, j) = colorTab[index - 1];
				boundRect[index - 1](0) = std::min(boundRect[index - 1](0), j); // x min
				boundRect[index - 1](1) = std::min(boundRect[index - 1](1), i); // y min
				boundRect[index - 1](2) = std::max(boundRect[index - 1](2), j); // x max
				boundRect[index - 1](3) = std::max(boundRect[index - 1](3), i); // y max
			}
		}
	//imshow("watershed transform", wshed);

	// show rectangular bounding boxes for all regions
	Mat rectsMat(watershed_seeds.size(), CV_8UC3);
	for (int i = 0; i < boundRect.size(); i++)
		rectangle(rectsMat, Point(boundRect[i](0), boundRect[i](1)), Point(boundRect[i](2), boundRect[i](3)), Scalar(colorTab[i]), 2, 8, 0);
	//imshow("bounding boxes", rectsMat);

	// filter out rectangles with inconsistent area coverage and sides
	Mat rectsMatFiltered(src.size(), CV_8UC3);
	for (int i = 0; i < boundRect.size(); i++)
	{
		const int w = (boundRect[i](2) - boundRect[i](0));
		const int h = (boundRect[i](3) - boundRect[i](1));
		const int area = w * h;

		if (area >(x_median + rectangle_side_epsilon) * (y_median + rectangle_side_epsilon) || h > 1.5 * w || w > 1.5 * h)
		{
			boundRect.erase(boundRect.begin() + i);
			i--;
		}
		else
			rectangle(rectsMatFiltered, Point(boundRect[i](0), boundRect[i](1)), Point(boundRect[i](2), boundRect[i](3)), Scalar(colorTab[i]), 2, 8, 0);
	}
	//imshow("bounding boxes (area/sides filtered)", rectsMatFiltered);

	// filter out rectangles without matching perpendicular fine houghlines,
	// and rectangles with overlapping oblique houghlines
	Mat onlyLinesP(src.size(), CV_8UC3);
	Mat onlyLinesO(src.size(), CV_8UC3);
	for (int i = 0; i < lines.size(); ++i)
	{
		Vec4i l = lines[i];

		if ((l(2) - (2 * rectangle_side_epsilon) <= l(0) && l(0) <= l(2) + (2 * rectangle_side_epsilon)) ||
			(l(3) - (2 * rectangle_side_epsilon) <= l(1) && l(1) <= l(3) + (2 * rectangle_side_epsilon)))
			line(onlyLinesP, Point(l(0), l(1)), Point(l(2), l(3)), Scalar(255, 255, 255), 1, CV_AA); // perpendicular
		else
			line(onlyLinesO, Point(l(0), l(1)), Point(l(2), l(3)), Scalar(255, 255, 255), 1, CV_AA); // oblique
	}

	Mat fatlinesP, fatlinesO;
	Mat element1 = getStructuringElement(0, Size(2 * 2 + 1, 2 * 2 + 1), Point(2, 2));
	morphologyEx(onlyLinesP, fatlinesP, 1, element1);
	morphologyEx(onlyLinesO, fatlinesO, 1, element1);

	//imshow("O", fatlinesO);
	//imshow("P", fatlinesP);
	//Mat toBeRemoved = rectsMatFiltered - 255 * fatlinesP;
	//imshow("to be removed", toBeRemoved);
	//Mat toBeRemoved2 = rectsMatFiltered - (rectsMatFiltered - 255 * fatlinesO);
	//imshow("to be removed2", toBeRemoved2);

	Mat rectsMatFilteredFinal(src.size(), CV_8UC3);
	for (int i = 0; i < boundRect.size(); i++)
	{
		Mat onlyOneRect(src.size(), CV_8UC3);
		rectangle(onlyOneRect, Point(boundRect[i](0), boundRect[i](1)), Point(boundRect[i](2), boundRect[i](3)), Scalar(255, 255, 255), 2, 8, 0);

		Mat diffP = onlyOneRect - 255 * fatlinesP;
		Mat diffO = onlyOneRect - (onlyOneRect - 255 * fatlinesO);

		// manual count of non-negative pixels
		int countP = 0, countO = 0;
		for (int x = 0; x < diffP.rows; x++)
			for (int y = 0; y < diffP.cols; y++)
			{
				if (diffP.at<Vec3b>(x, y)(0) > 0)
					countP++;
				if (diffO.at<Vec3b>(x, y)(0) > 0)
					countO++;
			}

		if (countP + countO > intersection_threshold)
		{
			boundRect.erase(boundRect.begin() + i);
			i--;
		}
		else
		{
			rectangle(rectsMatFilteredFinal, Point(boundRect[i](0), boundRect[i](1)), Point(boundRect[i](2), boundRect[i](3)), Scalar(255,255,255), 2, 8, 0);
		}

	}

	//imshow("final", rectsMatFilteredFinal);
	imshow(window_demo, rectsMatFilteredFinal + src);
}

void analyzeHoughLines(Mat& linesMat, const vector<Vec4i>& lines, std::vector<int>& x_peaks, std::vector<int>& y_peaks, int* x_median, int* y_median)
{
	// separate horizontal and vertical lines, oblique lines are discarded
	vector<Vec4i> linesH, linesV;
	Mat linesVMat, linesHMat; linesMat.copyTo(linesVMat), linesMat.copyTo(linesHMat);
	for (int i = 0; i < lines.size(); ++i)
	{
		Vec4i l = lines[i];

		if (l(2) - (2 * rectangle_side_epsilon) <= l(0) && l(0) <= l(2) + (2 * rectangle_side_epsilon))
		{
			linesV.push_back(l);
			line(linesVMat, Point(l(0), l(1)), Point(l(2), l(3)), Scalar(255, 0, 0), 1, CV_AA);
		}
		else if (l(3) - (2 * rectangle_side_epsilon) <= l(1) && l(1) <= l(3) + (2 * rectangle_side_epsilon))
		{
			linesH.push_back(l);
			line(linesHMat, Point(l(0), l(1)), Point(l(2), l(3)), Scalar(0, 255, 0), 1, CV_AA);
		}
	}

	//imshow("vertical", linesVMat);
	//imshow("horizontal", linesHMat);

	// compute histogram of horizontal and vertical lines on the x and y axis
	Mat histogramX(1, linesMat.cols, DataType<int>::type);
	Mat histogramY(linesMat.rows, 1, DataType<int>::type);

	histogramX.setTo(0);
	histogramY.setTo(0);

	for (int i = 0; i < linesV.size(); ++i)
	{
		Vec4i l = linesV[i];

		for (int x = std::min(l(0), l(2)); x <= std::max(l(0), l(2)); x++)
			histogramX.at<int>(0, x) += std::abs(l(1) - l(3));
	}

	Mat horizontalLines = linesMat;
	for (int i = 0; i < linesH.size(); ++i)
	{
		Vec4i l = linesH[i];

		for (int y = std::min(l(1), l(3)); y <= std::max(l(1), l(3)); y++)
			histogramY.at<int>(y, 0) += std::abs(l(0) - l(2));
	}

	// estimate X peaks
	std::vector<int> x_peaks_dist;
	int prev = 0, current = 0;
	std::vector<std::pair<int,int>> subseq;
	for (int i = 1; i < histogramX.cols - 1; i++)
	{
		current = histogramX.at<int>(0, i);

		if (prev <= 0 && current > 0)
		{
			subseq.clear();
			subseq.push_back(std::pair<int,int>(current,i));
		}
		else if (prev > 0 && current > 0)
			subseq.push_back(std::pair<int, int>(current, i));
		else if (prev > 0 && current <= 0)
		{
			// estimate x peak point
			std::sort(subseq.begin(), subseq.end());
			x_peaks.push_back(subseq[subseq.size() / 2].second);
			if (x_peaks.size() > 2)
				x_peaks_dist.push_back(std::abs(x_peaks[x_peaks.size() - 1] - x_peaks[x_peaks.size() - 2]));
		}

		prev = current;
	}
	
	// estimate median distance between X peaks
	std::sort(x_peaks_dist.begin(), x_peaks_dist.end());
	*x_median = x_peaks_dist[x_peaks_dist.size() / 2];
	int epsilon = *x_median / 3;

	// filter out too close-by peaks
	for (int i = 1; i < x_peaks.size(); i++)
	{
		const int dist = std::abs(x_peaks[i - 1] - x_peaks[i]);
		if (dist < *x_median - epsilon)
		{
			x_peaks.erase(x_peaks.begin() + i);
			i--;
		}
		else
			line(linesMat, Point(x_peaks[i], 0), Point(x_peaks[i], linesMat.rows), Scalar(0, 0, 255), 2, CV_AA);
	}

	// estimate Y peaks
	std::vector<int> y_peaks_dist;
	prev = 0; current = 0;
	for (int i = 1; i < histogramY.rows - 1; i++)
	{
		current = histogramY.at<int>(i, 0);

		if (prev <= 0 && current > 0)
		{
			subseq.clear();
			subseq.push_back(std::pair<int, int>(current, i));
		}
		else if (prev > 0 && current > 0)
			subseq.push_back(std::pair<int, int>(current, i));
		else if (prev > 0 && current <= 0)
		{
			// estimate pick point
			std::sort(subseq.begin(), subseq.end());
			y_peaks.push_back(subseq[subseq.size() / 2].second);
			if (y_peaks.size() > 2)
				y_peaks_dist.push_back(std::abs(y_peaks[y_peaks.size() - 1] - y_peaks[y_peaks.size() - 2]));
		}

		prev = current;
	}

	// estimate median distance between Y peaks
	std::sort(y_peaks_dist.begin(), y_peaks_dist.end());
	*y_median = y_peaks_dist[y_peaks_dist.size() / 2];
	epsilon = *y_median / 3;

	// filter out too close-by peaks
	for (int i = 1; i < y_peaks.size(); i++)
	{
		const int dist = std::abs(y_peaks[i - 1] - y_peaks[i]);
		if (dist < *y_median - epsilon)
		{
			y_peaks.erase(y_peaks.begin() + i);
			i--;
		}
		else
			line(linesMat, Point(0, y_peaks[i]), Point(linesMat.cols, y_peaks[i]), Scalar(255, 0, 0), 2, CV_AA);
	}
	//cv::imshow("updated lines", linesMat);
}

int main(int argc, char** argv)
{
	// --------------------------------------
	// read file
	const char* filename = argc >= 2 ? argv[1] : "D://barcelona.jpg";

	// load the source image
	src = imread(filename, CV_LOAD_IMAGE_COLOR);
	if (src.empty())
	{
		cout << "cannot open " << filename << endl;
		return -1;
	}

	// --------------------------------------
	// pre-processing

	// resizing
	resize(src, src, Size(0, 0), 0.5, 0.5);

	// split channels and work on red channel only
	std::vector<Mat> rgbchannel(3);
	Mat src_1channel;
	split(src, rgbchannel);
	src_1channel = rgbchannel[2];

	// blur
	//Mat blur;
	//GaussianBlur(src, blur, Size(7, 7), 2.0, 2.0);
	//bilateralFilter(src, blur, 30, 30 * 2, 30 / 2);

	// morphology operations
	Mat morph;
	Mat element = getStructuringElement(0, Size(2 * 2 + 1, 2 * 2 + 1), Point(2, 2));
	morphologyEx(src_1channel, morph, 1 + 2, element);
	src_1channel = morph;
	
	// thrsholding
	Mat thresh = src_1channel;
	threshold(src_1channel, thresh, 154, 255.0, THRESH_BINARY_INV);
	src_1channel = thresh;

	// coarse hough lines
	Mat dst = thresh;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 70, 3);
	std::cout << "Detected " << lines.size() << " lines!\n";

	// analyze found coarse lines
	Mat onlyLines(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0));
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(onlyLines, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
	}

	std::vector<int> x_picks, y_picks;
	analyzeHoughLines(onlyLines, lines, x_picks, y_picks, &x_median, &y_median);

	// compute finer hough lines
	HoughLinesP(dst, lines, 1, CV_PI / 180, 70, 20, 4);
	std::cout << "Detected " << lines.size() << " finer lines!\n";

	Mat onlyLines1(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0));
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(onlyLines1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
	}
	//imshow("detected finer lines", onlyLines1);

	// insert marker seeds for watershed for each estimated grid point
	Mat watershed_seeds_colored(src.rows, src.cols, CV_8UC3, Scalar(0, 0, 0));
	watershed_seeds = Mat(src.rows, src.cols, CV_32S); watershed_seeds = Scalar::all(0);
	const int x_half_median = x_median / 2;
	const int y_half_median = y_median / 2;
	const int radius = std::min(x_median / 4, y_median / 2);
	labels = 0; // number of different regions found
	for (int x = 0; x < x_picks.size(); x++)
		for (int y = 0; y < y_picks.size(); y++)
		{
			circle(watershed_seeds, Point(std::max(0, x_picks[x] - x_half_median), std::max(0, y_picks[y] - y_half_median)), radius, labels++, -1);

			colorTab.push_back(Vec3b((uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255)));
			circle(watershed_seeds_colored, Point(std::max(0, x_picks[x] - x_half_median), std::max(0, y_picks[y] - y_half_median)), radius, Scalar(colorTab.back()), -1);

			if (x == x_picks.size() - 1)
			{
				circle(watershed_seeds, Point(std::max(0, x_picks[x] + x_half_median), std::max(0, y_picks[y] - y_half_median)), radius, labels++, -1);

				colorTab.push_back(Vec3b((uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255)));
				circle(watershed_seeds_colored, Point(std::max(0, x_picks[x] + x_half_median), std::max(0, y_picks[y] - y_half_median)), radius, Scalar(colorTab.back()), -1);
			}
			if (y == y_picks.size() - 1)
			{
				circle(watershed_seeds, Point(std::max(0, x_picks[x] - x_half_median), std::max(0, y_picks[y] + y_half_median)), radius, labels++, -1);

				colorTab.push_back(Vec3b((uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255)));
				circle(watershed_seeds_colored, Point(std::max(0, x_picks[x] - x_half_median), std::max(0, y_picks[y] + y_half_median)), radius, Scalar(colorTab.back()), -1);
			}
			if (x == x_picks.size() - 1 && y == y_picks.size() - 1)
			{
				circle(watershed_seeds, Point(std::max(0, x_picks[x] + x_half_median), std::max(0, y_picks[y] + y_half_median)), radius, labels++, -1);

				colorTab.push_back(Vec3b((uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255), (uchar)theRNG().uniform(0, 255)));
				circle(watershed_seeds_colored, Point(std::max(0, x_picks[x] + x_half_median), std::max(0, y_picks[y] + y_half_median)), radius, Scalar(colorTab.back()), -1);
			}
		}
	//imshow("watershed marker seeds", watershed_seeds_colored);

	// apply watershed
	watershed(onlyLines1, watershed_seeds);

	// launch the demo
	namedWindow(window_demo, CV_WINDOW_AUTOSIZE);

	/// Create Trackbar to choose type of Threshold
	createTrackbar("Rectangle side epsilon",
		window_demo, &rectangle_side_epsilon,
		1000, demo);

	createTrackbar("Intersection Threshold",
		window_demo, &intersection_threshold,
		1000, demo);

	demo(0, 0);

	waitKey();

	return 0;
}