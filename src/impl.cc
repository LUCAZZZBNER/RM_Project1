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
    //TODO: 改倍数
    else if (arcLength(approxs[max_Num], true) > arcLength(approxs[sec_Num], true) * 1.6) {
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

//找顶点
void find_Vertices(vector<vector<Point>>& faceContours, vector<Point>& vertices) {
    vector<vector<Point>> faceContours_copy = faceContours;
    for (int i = 0;i < faceContours.size(); i++) {
        for (size_t j = 0; j < vertices.size(); j++) {
            if (pointPolygonTest(faceContours[i], vertices[j], false) == 0) {
                faceContours_copy[j] = faceContours[i];
            }
        }
    }
    faceContours.clear();
    for (size_t i = 0; i < faceContours_copy.size(); i++) {
        faceContours.push_back(faceContours_copy[i]);
    }

    vector<Point> vertices_copy = vertices;
    for (int i = 0;i < faceContours.size();i++) {
        double max = 0;
        for (int j = 0;j < faceContours[i].size();j++) {
            vertices_copy[i] = faceContours[i][j];
            if (contourArea(vertices_copy) >= max) {
                max = contourArea(vertices_copy);
                vertices[i] = faceContours[i][j];
            }
        }
    }

}

//PnP解算
Mat PNP(const vector<Point>& face_Points, const Mat& src) {
    vector<Point3f> objectPoints;
    vector<Point2f> imagePoints; 

    objectPoints.push_back(Point3f(0.0f, 0.0f, 0.0f));
    objectPoints.push_back(Point3f(275.0f, 0.0f, 0.0f));
    objectPoints.push_back(Point3f(275.0f, 275.0f, 0.0f));
    objectPoints.push_back(Point3f(0.0f, 275.0f, 0.0f));

    for (int i = 0; i < face_Points.size(); i++) {
        imagePoints.push_back(face_Points[i]);
    }
    // 定义相机内参矩阵
    Mat cameraMatrix = (Mat_<double>(3, 3) << 1.521928836685752e+03, 0, 9.504930579948792e+02, 0, 1.521695508574793e+03, 6.220985582938733e+02, 0, 0, 1);
    // 定义畸变系数
    Mat distCoeffs = (cv::Mat_<double>(5, 1) << -0.157095872989630, 0.166823029778507, 1.356728774532785e-04, 2.266474993725451e-04, -0.070807947517560);

    // 调用solvePnP函数
    Mat rvec, tvec;
    bool success = solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec);

    if (success) {
        Mat pnp = src.clone();
        drawFrameAxes(pnp, cameraMatrix, distCoeffs, rvec, tvec, 100, 2);
        return pnp;
    }
    else {
        // 位姿解算失败
        cout << "Failed to solve PnP" << endl;
        return src;
    }
}