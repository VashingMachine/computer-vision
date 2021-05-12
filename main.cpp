#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <unistd.h>

using namespace cv;

bool openSource(cv::VideoCapture &cap, std::string source) {
    return source == "0" ? cap.open(0) : cap.open(source);
}

void createMarks(Mat& marks, Mat& dest) {
    for(int y = 0; y < marks.size[1]; y++) {
        for(int x = 0; x < marks.size[0]; x++) {
            if(marks.at<uchar>(x, y) > 0) {
                dest.at<Vec3b>(x, y) = Vec3b (255, 0, 0);
            }
        }
    }
}

Mat image, fgMask;
Ptr<BackgroundSubtractorMOG2> subtractor;

void on_trackbarNMixtures(int value, void * p) {
    std::cout << value << std::endl;
    subtractor->setNMixtures(value);
    subtractor->apply(image, fgMask, 1);
}

int main() {
    cv::VideoCapture camera;
    if(!openSource(camera, "bike.avi")) {
        std::cout << "Cannot open source!\n";
        return -1;
    }

    camera.set(CAP_PROP_FPS, 1);
    RNG rng(12345);
    namedWindow("BaseWindow", WINDOW_AUTOSIZE);
    namedWindow("Foreground", WINDOW_AUTOSIZE);
    namedWindow("Background", WINDOW_AUTOSIZE);
    namedWindow("Fgopened", WINDOW_AUTOSIZE);
    namedWindow("Trackpads", WINDOW_AUTOSIZE);

    int history = 500, nmixtures = 16, threshold = 16, learning_rate = 1000, kernel_size = 3;
    createTrackbar("History", "Trackpads", &history, 2000);
    createTrackbar("Threshold", "Trackpads", &threshold, 255);
    createTrackbar("N mixtures", "Trackpads", &nmixtures, 255, on_trackbarNMixtures);
    createTrackbar("Learning rate", "Trackpads", &learning_rate, 1000);
    createTrackbar("Kernel size", "Trackpads", &kernel_size, 10);


    subtractor = createBackgroundSubtractorMOG2(history, threshold, true);
    while(camera.isOpened()) {
        Mat foreground, background, eroded, dilated;
        Mat kernel = getStructuringElement(MORPH_RECT, Size(2 * kernel_size + 1, 2 * kernel_size + 1), Point(kernel_size, kernel_size));
        camera >> image;
        if(image.empty()) {
            camera.set(CAP_PROP_POS_FRAMES, 0);
        } else {
            subtractor->apply(image, fgMask, learning_rate / 1000.);
            subtractor->setHistory(history);
            subtractor->setVarThreshold(threshold);
            subtractor->getBackgroundImage(background);
            erode(fgMask, eroded, kernel);
            dilate(eroded, dilated, kernel);
            createMarks(dilated, image);
            imshow("BaseWindow", image);
            imshow("Foreground", fgMask);
            imshow("Background", background);
            imshow("Fgopened", dilated);

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
