#include "test.h"

Mat test_Points(Mat src) {
    resize(src, src, Size(), 4, 4);

    vector<vector<Point>> contours = get_Contours(src);
    vector<vector<Point>> approxs = get_Triangles(contours);
    
    // Mat red_Lines = src.clone();
    // drawContours(red_Lines, contours, -1, Scalar(0, 255, 0), 2);;

    // Mat approx_Lines = src.clone();
    // drawContours(approx_Lines, approxs, -1, Scalar(0, 255, 0), 2);

    Mat dst = src.clone();
    
    vector<Point> side_Points;
    vector<Point> face_Points;

    vector<vector<Point>> faceContours;
    vector<vector<Point>> sideContours;
    vector<vector<Point>> specialContours;
    vector<vector<Point>> otherContours;

    // for (int i = 0; i < approxs.size(); i++) {
    //     cout << arcLength(approxs[i], true) << endl;
    // }

    //区分正面和侧面
    distinguish_Faces(faceContours, sideContours, approxs);

    //drawContours(dst, sideContours, -1, Scalar(255, 150, 20), 2);

    //侧面点标序
    if (sideContours.size() == 1) {
        for (int i = 0; i < sideContours[0].size(); i++) {
            side_Points.push_back(sideContours[0][i]);
        }
    }
    if (side_Points.size() == 3) {
        sortByY(side_Points);
        for (int p = 0; p < side_Points.size(); p++) {
            circle(dst, side_Points[p], 7, Scalar(255, 150, 20), -1);
            putText(dst, to_string(p), side_Points[p], FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 150, 20), 2);
        }
        for (int i = 0; i < 3; i++) {
            side_Points[i].x = side_Points[i].x / 4;
            side_Points[i].y = side_Points[i].y / 4;
        }
    }

    //正面点标序
    distinguish_Special(faceContours, specialContours, otherContours);

    //drawContours(dst, faceContours, -1, Scalar(255, 255, 0), 2);

    faceContours.clear();
    if (specialContours.size() == 1 && otherContours.size() == 3) {
        faceContours.push_back(specialContours[0]);
        face_Points.push_back(specialContours[0][0]);
        for (int i = 0;i < otherContours.size();i++) {
            faceContours.push_back(otherContours[i]);
            face_Points.push_back(otherContours[i][0]);
        }
        sortByAngle(face_Points, face_Points[0]);
        find_Vertices(faceContours, face_Points);
        for (int p = 0; p < face_Points.size(); p++) {
            circle(dst, face_Points[p], 7, Scalar(255, 255, 0), -1);
            putText(dst, to_string(p), face_Points[p], FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 0), 2);
        }

        for (int i = 0; i < 4; i++) {
            face_Points[i].x = face_Points[i].x / 4;
            face_Points[i].y = face_Points[i].y / 4;
        }
    }

    //resize(src, src, Size(), 0.25, 0.25);
    resize(dst, dst, Size(), 0.25, 0.25);

    //Mat pnp;
    if (face_Points.size() == 4) {
        dst = PNP(face_Points, dst);
    }
    // else {
    //     pnp = src;
    // }


    // imshow("src", src);
    // imshow("Red Lines", red_Lines);
    // imshow("approx_Lines", approx_Lines);
    //imshow("vertex Points", dst);
    //imshow("pnp", pnp);
    return dst;
}   