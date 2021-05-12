#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

bool openSource(cv::VideoCapture &cap, std::string source) {
    return source == "0" ? cap.open(0) : cap.open(source);
}
int main() {
    cv::VideoCapture camera;
    if(!openSource(camera, "https://192.168.0.178:8080/video")) {
        std::cout << "Cannot open source!\n";
        return -1;
    }
    RNG rng(12345);
    namedWindow("BaseWindow", WINDOW_AUTOSIZE);
    namedWindow("BlurWindow", WINDOW_AUTOSIZE);
    namedWindow("CannyWindow", WINDOW_AUTOSIZE);
    namedWindow("SobelX", WINDOW_AUTOSIZE);
    namedWindow("SobelY", WINDOW_AUTOSIZE);
    namedWindow("Magnitude", WINDOW_AUTOSIZE);
    namedWindow("Angle", WINDOW_AUTOSIZE);
    namedWindow("Trackpads", WINDOW_AUTOSIZE);

    int lowThreshold = 40, highThreshold = 120, cannyKernelSize = 0, gaussKernelSize = 5;
    int houghThreshold = 80, nlines = 30, minLineHeight = 10;
    createTrackbar("Low Threshold", "Trackpads", &lowThreshold, 255);
    createTrackbar("High Threshold", "Trackpads", &highThreshold, 255);
    createTrackbar("Canny Kernel Size", "Trackpads", &cannyKernelSize, 2);
    createTrackbar("Gauss Kernel Size", "Trackpads", &gaussKernelSize, 10);
    createTrackbar("Hough threshold", "Trackpads", &houghThreshold, 255);
    createTrackbar("Hough lines", "Trackpads", &nlines, 200);
    createTrackbar("Hough minLineHeight", "Trackpads", &minLineHeight, 30);


    while(camera.isOpened()) {
        Mat image, grayImage, blurred, cannyImage;
        camera >> image;
        if(image.empty()) {
            break;
        } else {
            cvtColor(image, grayImage, COLOR_BGR2GRAY);
            blur(grayImage, blurred, Size(gaussKernelSize + 1, gaussKernelSize + 1));
            Canny(blurred, cannyImage, lowThreshold, highThreshold, cannyKernelSize * 2 + 3);

            std::vector<Vec4i> lines;
            HoughLinesP(cannyImage, lines, 1, CV_PI/180, houghThreshold, nlines, minLineHeight);
            for( size_t i = 0; i < lines.size(); i++ )
            {
                line( image, Point(lines[i][0], lines[i][1]),
                      Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
            }

            imshow("BaseWindow", image);
            imshow("CannyWindow", cannyImage);
        }
        int k;
        switch((k = waitKey(15))) {
            case 27:
                break;
        }
    }
    destroyAllWindows();
    camera.release();
    return 0;
}
