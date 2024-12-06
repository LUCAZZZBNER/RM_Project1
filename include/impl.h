#ifndef IMPL_H
#define IMPL_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;

vector<vector<Point>> get_Contours(const Mat& src);
vector<vector<Point>> get_Triangles(vector<vector<Point>> contours);
void distinguish_Faces(vector<vector<Point>>& faceContours, vector<vector<Point>>& sideContours, vector<vector<Point>>& approxs);
bool compareByY(const cv::Point& a, const cv::Point& b);
void sortByY(vector<cv::Point>& points);
Point findVertice(const vector<Point>& contour, Mat& img);
bool isRedPixel(Point point, Mat image);
void distinguish_Special(vector<vector<Point>>& faceContours, vector<vector<Point>>& specialContours, vector<vector<Point>>& otherContours);
struct PointWithAngle { cv::Point pt;double angle; };
bool comparewithAngle(const PointWithAngle& a, const PointWithAngle& b);
void sortByAngle(vector<Point>& points, Point startPt);
Mat PNP(const vector<Point>& face_Points);

#endif