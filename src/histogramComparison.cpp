#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;

void show_histogram(std::string name, cv::Mat const& image)
{
    // Set histogram bins count
    int bins = 256;
    int histSize[] = {bins};
    // Set ranges for histogram bins
    float lranges[] = {0, 256};
    const float* ranges[] = {lranges};
    // create matrix for histogram
    cv::Mat hist;
    int channels[] = {0};

    // create matrix for histogram visualization
    int const hist_height = 256;
    cv::Mat hist_image = cv::Mat::zeros(bins, hist_height, CV_8UC1 );

    cv::calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

    double max_val=0;
    minMaxLoc(hist, 0, &max_val);

    // visualize each bin
    for(int b = 0; b < bins; b++) {
        float const binVal = hist.at<float>(b);
        int   const height = cvRound(binVal*hist_height/max_val);
        cv::line
            ( hist_image
            , cv::Point(b, hist_height-height), cv::Point(b, hist_height)
            , cv::Scalar::all(255)
            );
    }
    cv::imwrite(name, hist_image);
}

int main(int argc, char** argv) {
    string path = argv[1];
    cv::Mat image = cv::imread(path);
    cv::Mat greyImage, equalizedMat;
    cv::cvtColor(image,greyImage,cv::COLOR_BGR2GRAY);
    cv::equalizeHist(greyImage,equalizedMat);
    show_histogram("output_images/histogram.jpg",greyImage);
    show_histogram("output_images/eq_histogram.jpg",equalizedMat);
    imwrite("output_images/equalized_image.jpg",equalizedMat);
    imwrite("output_images/grey_image.jpg",greyImage);
    // cv::waitKey(0);
    return 0;
}