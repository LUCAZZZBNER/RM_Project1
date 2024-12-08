#include "impl.h"
#include "test.h"

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
    namedWindow("vertex Points", WINDOW_NORMAL);
    resizeWindow("vertex Points", 1200, 750);
    namedWindow("pnp", WINDOW_NORMAL);
    resizeWindow("pnp", 1200, 750);

    vector<string> imagePaths;
    glob("../images/station/*.jpg", imagePaths);
    for (int i = 0; i < imagePaths.size(); i++) {
        cv::Mat image = cv::imread(imagePaths[i]);
        if (image.empty()) {
            std::cerr << "Could not read the image: " << imagePaths[i] << std::endl;
            continue;
        }
        test_Points(image);
        cout << imagePaths[i] << endl;
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