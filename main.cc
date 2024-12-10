#include "impl.h"
#include "test.h"
#include "glob.h"

int main() {

// test  

    // while (1) {
    //     string relative_Path;
    //     cout << "\nPlease input the relative path of the image: " << endl;
    //     cin >> relative_Path;
    //     if (relative_Path == "exit" || relative_Path == "0") {
    //         break;
    //     }
    //     Mat src = imread("../" + relative_Path);
    //     if (src.empty()) {
    //         cout << "Error: Unable to load image at path '" << "../" + relative_Path << "'. Please check the path and try again." << endl;
    //         continue; 
    //     }
    //     // namedWindow("src", WINDOW_NORMAL);
    //     // resizeWindow("src", 1600, 1000);
    //     // namedWindow("Red Lines", WINDOW_NORMAL);
    //     // resizeWindow("Red Lines", 1600, 1000);
    //     // namedWindow("approx_Lines", WINDOW_NORMAL);
    //     // resizeWindow("approx_Lines", 1600, 1000);
    //     namedWindow("vertex Points", WINDOW_NORMAL);
    //     resizeWindow("vertex Points", 1600, 1000);
    //     namedWindow("pnp", WINDOW_NORMAL);
    //     resizeWindow("pnp", 1600, 1000);

    //     test_Points(src);
    //     waitKey(0);
    //     destroyAllWindows();
    // }

//play
    
    // namedWindow("src", WINDOW_NORMAL);
    // resizeWindow("src", 1600, 1000);
    // namedWindow("Red Lines", WINDOW_NORMAL);
    // resizeWindow("Red Lines", 1600, 1000);
    // namedWindow("approx_Lines", WINDOW_NORMAL);
    // resizeWindow("approx_Lines", 1600, 1000);
    // namedWindow("vertex Points", WINDOW_NORMAL);
    // resizeWindow("vertex Points", 1200, 750);
    // namedWindow("pnp", WINDOW_NORMAL);
    // resizeWindow("pnp", 1200, 750);
    namedWindow("dst", WINDOW_NORMAL);
    resizeWindow("dst", 1200, 750);

    vector<string> imagePaths;
    glob("../images/station/*.jpg", imagePaths);
    for (int i = 0; i < imagePaths.size(); i++) {
        cv::Mat image = cv::imread(imagePaths[i]);
        if (image.empty()) {
            std::cerr << "Could not read the image: " << imagePaths[i] << std::endl;
            continue;
        }
        Mat dst = test_Points(image);
        imwrite("../images/outPut/" + imagePaths[i].substr(18), dst);
        cout << imagePaths[i] << endl;
        imshow("dst", dst);
        int num = waitKey(0);
        if (num == 27) {
            break;
        }
        else if (num == 82|| num == 81) {
            if (i > 0) {
                i -= 2;
            }
        }
    }
    destroyAllWindows();

    
    return 0;
}

// int main() {
//     // 图片文件夹路径
//     string image_folder = "/mnt/d/0_wsl_workspace/RM_Project1/images/outPut/";
//     // 视频输出文件名，包括完整路径
//     string video_name = "/mnt/d/0_wsl_workspace/RM_Project1/videos/video.mp4"; // 注意这里是绝对路径
//     // 视频帧的宽度和高度（根据图片的实际分辨率设置）
//     int frame_width, frame_height;
//     // 视频帧率
//     double fps = 30.0;

//     // 获取所有图片文件名，并按文件名排序
//     vector<string> images;
//     glob_t glob_result;
//     glob((image_folder + "*.*").c_str(), GLOB_TILDE, NULL, &glob_result);
//     for (size_t i = 0; i < glob_result.gl_pathc; ++i) {
//         images.push_back(glob_result.gl_pathv[i]);
//     }
//     globfree(&glob_result);
//     sort(images.begin(), images.end());

//     // 读取第一张图片以获取视频帧的尺寸
//     Mat frame = imread(images[0]);
//     if (frame.empty()) {
//         cerr << "Failed to read the first image." << endl;
//         return -1;
//     }
//     frame_width = frame.cols;
//     frame_height = frame.rows;

//     // 创建一个VideoWriter对象，使用MP4编码器
//     int codec = VideoWriter::fourcc('m', 'p', '4', 'v'); // 或者使用 'X', 'V', 'I', 'D'
//     VideoWriter video_writer(video_name, codec, fps, Size(frame_width, frame_height));

//     // 检查VideoWriter是否成功初始化
//     if (!video_writer.isOpened()) {
//         cerr << "Failed to open video writer." << endl;
//         return -1;
//     }

//     // 循环读取每张图片并写入视频
//     for (size_t i = 0; i < images.size(); ++i) {
//         Mat image = imread(images[i]);
//         if (image.empty()) {
//             cerr << "Failed to read image: " << images[i] << endl;
//             continue;
//         }
//         video_writer.write(image);
//     }

//     // 释放VideoWriter对象
//     video_writer.release();

//     cout << "Video has been created at " << video_name << endl;
//     return 0;
// }