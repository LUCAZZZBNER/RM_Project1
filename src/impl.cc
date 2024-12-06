#include "impl.h"

// 获取轮廓
vector<vector<Point>> get_Contours(const Mat& src) {
    Mat dst= src.clone();

    //resize(dst, dst, Size(), 4, 4);

    // 转换颜色空间
    Mat hsv;
    cvtColor(dst, hsv, COLOR_BGR2HSV);

    //TODO: 把范围调小
    // 设定红色的HSV阈值范围
    Mat mask1, mask2, mask;
    inRange(hsv, Scalar(0, 43, 20), Scalar(10, 255, 255), mask1);
    inRange(hsv, Scalar(156, 43, 20), Scalar(180, 255, 255), mask2);

    // 合并两个阈值范围的二值图像
    //add(mask1, mask2, mask);
    mask = mask1 | mask2;

    // 形态学操作去除噪点
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat kerne2 = getStructuringElement(MORPH_RECT, Size(7, 7));
    morphologyEx(mask, mask, MORPH_OPEN, kernel);
    morphologyEx(mask, mask, MORPH_CLOSE, kerne2);

    // 高斯模糊
    GaussianBlur(mask, mask, Size(5, 5), 0);

    // 边缘检测
    Canny(mask, mask, 100, 200);

    // 查找轮廓
    vector<vector<Point>> contours0;
    vector<vector<Point>> contours;
    findContours(mask, contours0, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    float allarea = 0;
    for (int i = 0; i < contours0.size(); i++) {
        allarea += contourArea(contours0[i]);
    }
    for (int i = 0; i < contours0.size(); i++) {
        //cout << contourArea(contours0[i]) << endl;
        //TODO:  contourArea(contours0[i]) > (里面得调)
        if (contourArea(contours0[i]) / allarea > 0.014){
            contours.push_back(contours0[i]);
        }
    }
    //cout << allarea << endl;

    return contours;
}

//用三角形近似
vector<vector<Point>> get_Triangles(vector<vector<Point>> contours) {
    vector<vector<Point>> approxs(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        float Prec = 0.1;
        int t = 0;
        int corners = 3;
        while (1) {
            cv::approxPolyDP(contours[i], approxs[i], cv::arcLength(contours[i], true) * Prec, true);
            if (t > 100 || approxs[i].size() == corners) {
                break;
            }
            if (approxs[i].size() > corners) {
                Prec += 0.01;
                t++;
            }
            else if (approxs[i].size() < corners && Prec > 0.01) {
                Prec -= 0.01;
                t++;
            }
            else {
                t++;
            }
        }
    }
    return approxs;
}

//分辨正面和侧面
void distinguish_Faces(vector<vector<Point>>& faceContours, vector<vector<Point>>& sideContours, vector<vector<Point>>& approxs) {
    float Length = 0;
    int max_Num = -1;
    int sec_Num = -1;
    for (size_t i = 0; i < approxs.size(); i++) {
        if (arcLength(approxs[i], true) > Length) {
            Length = arcLength(approxs[i], true);
            max_Num = i;    
        }
    }
    Length = 0;
    for (size_t i = 0; i < approxs.size(); i++) {
        if (arcLength(approxs[i], true) > Length && i != max_Num) {
            Length = arcLength(approxs[i], true);
            sec_Num = i;    
        }
    }
    if (sec_Num == -1) {
        sideContours.push_back(approxs[max_Num]);
    }
    else if (arcLength(approxs[max_Num], true) > arcLength(approxs[sec_Num], true) * 1.8) {
        sideContours.push_back(approxs[max_Num]);
        for (size_t i = 0; i < approxs.size(); i++) {
            if (i != max_Num) {
                faceContours.push_back(approxs[i]);
            }
        }
    }
    else {
        for (size_t i = 0; i < approxs.size(); i++) {
            faceContours.push_back(approxs[i]);
        }
    }
}

// 侧面按Y排序
bool compareByY(const cv::Point& a, const cv::Point& b) {
    return a.y < b.y;
}

void sortByY(vector<cv::Point>& points) {
    sort(points.begin(), points.end(), compareByY);
}

//找轮廓顶点
//TODO: 待修改
Point findVertice(const vector<Point>& contour, Mat& img) {
    //cout<<contour.size()<<endl;
    Point vertice = contour[0];

    if (contour.size() == 3) {
        for (int j = 0; j < 3; j++) {
            Point midPoint1 = (contour[j] + contour[(j + 1) % 3]) / 2;
            //circle(img, midPoint1, 3, Scalar(0, 255, 0), -1);
            Point midPoint2 = (contour[j] + contour[(j + 2) % 3]) / 2;
            //circle(img, midPoint2, 3, Scalar(0, 255, 0), -1);
            int a = isRedPixel(midPoint1, img);
            int b = isRedPixel(midPoint2, img);

            if (a && b) {
                vertice = contour[j];
                //cout << img.at<Vec3b>(midPoint1) << endl;
                break;
            }
            else {
                float r = 3;// 调整
                Point midPoint1_1(midPoint1.x + r, midPoint1.y + r);
                Point midPoint1_2(midPoint1.x - r, midPoint1.y - r);
                Point midPoint1_3(midPoint1.x + r, midPoint1.y - r);
                Point midPoint1_4(midPoint1.x - r, midPoint1.y + r);
                Point midPoint2_1(midPoint2.x + r, midPoint2.y + r);
                Point midPoint2_2(midPoint2.x - r, midPoint2.y - r);
                Point midPoint2_3(midPoint2.x + r, midPoint2.y - r);
                Point midPoint2_4(midPoint2.x - r, midPoint2.y + r);

                int a1 = isRedPixel(midPoint1_1, img);
                int a2 = isRedPixel(midPoint1_2, img);
                int a3 = isRedPixel(midPoint1_3, img);
                int a4 = isRedPixel(midPoint1_4, img);
                int b1 = isRedPixel(midPoint2_1, img);
                int b2 = isRedPixel(midPoint2_2, img);
                int b3 = isRedPixel(midPoint2_3, img);
                int b4 = isRedPixel(midPoint2_4, img);

                if ((a1 || a2 || a3 || a4) && (b1 || b2 || b3 || b4)){
                    vertice = contour[j];
                }
            }
        }
    }

    // if (contour.size() == 3) {
    //     double max = 0;
    //     for (int j = 0; j < 3; j++) {
    //         double angle1 = atan2(contour[(j + 1) % 3].y - contour[j].y, contour[(j + 1) % 3].x - contour[j].x);
    //         double angle2 = atan2(contour[(j + 2) % 3].y - contour[j].y, contour[(j + 2) % 3].x - contour[j].x);
    //         double angle = angle2 - angle1;
    //         if (angle < 0)angle *= -1;
    //         if (angle > M_PI)angle = 2 * M_PI - angle;
    //         if (angle > max) {
    //             max = angle;
    //             vertice = contour[j];
    //         }
    //     }
    // }

    return vertice;
}

bool isRedPixel(Point point, Mat image) {
    int x = point.x;
    int y = point.y;
    if (x < 0 || x >= image.cols || y < 0 || y >= image.rows || image.empty()) {
        return false;
    }

    const Scalar redLower(0, 0, 50);
    const Scalar redUpper(50, 50, 255);
    //adjust

    Vec3b color = image.at<Vec3b>(y, x);
    return (color[0] >= redLower[0] && color[0] <= redUpper[0] &&
            color[1] >= redLower[1] && color[1] <= redUpper[1] &&
            color[2] >= redLower[2] && color[2] <= redUpper[2]);
}

//在面部轮廓中找特殊轮廓
//TODO: 后期换一种识别方法
void distinguish_Special(vector<vector<Point>>& faceContours, vector<vector<Point>>& specialContours, vector<vector<Point>>& otherContours) {
    float allArea = 0;
    for (size_t i = 0; i < faceContours.size(); i++) {
        allArea += contourArea(faceContours[i]);
    }
    for (size_t i = 0; i < faceContours.size(); i++) {
        float area = contourArea(faceContours[i]);
        //调参数
        if (area / allArea > 0.05 && area / allArea < 0.17) {
            specialContours.push_back(faceContours[i]);
        }
        else if(area / allArea >= 0.15) {
            otherContours.push_back(faceContours[i]);
        }
    }
}

//顺时针排序
bool comparewithAngle(const PointWithAngle& a, const PointWithAngle& b) {
    return a.angle < b.angle;
}

void sortByAngle(vector<Point>& points, Point startPt) {
    vector<PointWithAngle> pointsWithAngles;
    for (const auto& pt : points) {
        double angle = atan2(pt.y - startPt.y, pt.x - startPt.x);
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        //cout<<angle<<" "<<pt<<endl;
        pointsWithAngles.push_back({ pt, angle });
    }
    sort(pointsWithAngles.begin(), pointsWithAngles.end(), comparewithAngle);
    int Q = 0;
    for (size_t i = 1; i < pointsWithAngles.size(); i++) {
        if (pointsWithAngles[i].angle - pointsWithAngles[i - 1].angle > M_PI) {
            pointsWithAngles[i].angle -= 2 * M_PI;
            Q = 1;
        }
    }
    if (Q) {
        pointsWithAngles[0].angle -= 2 * M_PI;
    }
    sort(pointsWithAngles.begin(), pointsWithAngles.end(), comparewithAngle);
    points.clear();
    for (const auto& pt : pointsWithAngles) {
        points.push_back(pt.pt);
    }
}

Mat PNP(const vector<Point>& face_Points) {
    vector<Point3f> objectPoints; 
    vector<Point2f> imagePoints; 

    objectPoints.push_back(Point3f(0.0f, 0.0f, 0.0f));
    objectPoints.push_back(Point3f(0.0f, -275.0f, 0.0f));
    objectPoints.push_back(Point3f(-275.0f, -275.0f, 0.0f));
    objectPoints.push_back(Point3f(-275.0f, 0.0f, 0.0f));

    for (int i = 0; i < face_Points.size(); i++) {
        imagePoints.push_back(Point2f(face_Points[i].x, face_Points[i].y));
    }
    // 定义相机内参矩阵
    Mat cameraMatrix = (Mat_<double>(3, 3) << 1.521928836685752e+03, 0, 9.504930579948792e+02, 0, 1.521695508574793e+03, 6.220985582938733e+02, 0, 0, 1);
    // 定义畸变系数
    Mat distCoeffs = (cv::Mat_<double>(5, 1) << -0.157095872989630, 0.166823029778507, 1.356728774532785e-04, 2.266474993725451e-04, -0.070807947517560);

    // 调用solvePnP函数
    Mat rvec, tvec;
    bool success = solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

    if (success) {
        // 位姿解算成功，rvec和tvec包含了相机的旋转和平移信息

        // 将旋转向量转换为旋转矩阵
        Mat R;
        Rodrigues(rvec, R); // R是3x3的旋转矩阵

        // 构建4x4变换矩阵
        Mat transformMatrix = Mat::eye(4, 4, R.type());
        R.copyTo(transformMatrix(Rect(0, 0, 3, 3))); // 将旋转矩阵R复制到左上角
        tvec.copyTo(transformMatrix(Rect(3, 0, 1, 3))); // 将平移向量tvec复制到右上角

        // 打印旋转矩阵和平移向量
        cout << "Rotation Matrix: " << R << endl;
        cout << "Translation Vector: " << tvec << endl;

        // 打印变换矩阵
        cout << "Transformation Matrix: " << transformMatrix << endl;

        // 使用变换矩阵将世界坐标系中的点转换到相机坐标系
        Mat pointWorld = (Mat_<double>(4,1) << 1.0, 1.0, 1.0, 1.0); // 世界坐标系中的点（齐次坐标）
        Mat pointCamera = transformMatrix * pointWorld; // 转换到相机坐标系
        cout << "Point in Camera Coordinates: " << pointCamera.t() << endl;

        return transformMatrix;
    }
    else {
        // 位姿解算失败
        cout << "Failed to solve PnP" << endl;
    }

    return Mat();
}