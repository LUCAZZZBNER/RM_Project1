#include "impl.h"
#include "test.h"

int main() {
    while (1) {
        string relative_Path;
        cout<<"\nPlease input the relative path of the image: "<<endl;
        cin >> relative_Path;
        if (relative_Path == "exit" || relative_Path == "0") {
            return 0;
        }
        Mat src = imread("../" + relative_Path);
        if (src.empty()) {
            cout << "Error: Unable to load image at path '" << "../" + relative_Path << "'. Please check the path and try again." << endl;
            continue; 
        }
        test_Points(src);
        //test_AnglePoints(src);
    }
    return 0;
}