#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


// initializing the variables and objects that are used in a 
//program to perform image processing operations such as blurring and smoothing.

Mat src;
Mat dst;

int kernel_length = 3;
char window_name[] = "Smoothing";
int display_caption(const char* caption); //This line declares a function named display_caption 
                                          //that takes a pointer to a constant character array as its argument and returns an integer value.
int display_dst(int delay); 

                            
Mat image; //object of the Mat class named image. This object is used to store the image data that will be processed.
int blurMin = 1;
int blurMax = 50;
int sigma_min = 1;
int sigmaR_min = 1;
int sigmaS_min = 1;
int sigma_max = 100;

Mat draw_histogram(Mat& hist, Mat& image, Scalar color) {
    // Set the histogram parameters
    int histSize = 256;
    float range[] = { 0, 256 };               //sets the range of pixel values that will be considered in the histogram.
    const float* histRange = { range };       //creates a pointer to the array that contains the range of pixel values.
    bool uniform = true, accumulate = false;
    

    // Calculate the histogram
    Mat hist_image;
    calcHist(&hist, 1, 0, Mat(), hist_image, 1, &histSize, &histRange, uniform, accumulate);

    // Draw the histogram
    int hist_height = 200, hist_width = 256; // sets the dimensions
    int bin_width = cvRound((double)hist_width / histSize);
    Mat histImage(hist_height, hist_width, CV_8UC3, Scalar(0, 0, 0)); //black image 
    normalize(hist_image, hist_image, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_width * (i - 1), hist_height - cvRound(hist_image.at<float>(i - 1))),
            Point(bin_width * (i), hist_height - cvRound(hist_image.at<float>(i))),
            color, 2, LINE_AA);
    }

    // Combine and concatenate the histogram and image
    Mat combined_image;
    vconcat(histImage, combined_image);

    return combined_image;
}

void show_histogram(Mat& img) {
    // Separate the image into 3 channels BGR (blue, green, red)
    vector<Mat> bgr_planes;
    split(img, bgr_planes);

    // Set the histogram parameters
    int histSize = 256;              //specifies the number of bins in the histogram.
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true, accumulate = false;

    // Compute the histograms
    //calcHist() function is used to compute the histograms of individual channels using the previously defined parameters.
    Mat hist_b, hist_g, hist_r;
    calcHist(&bgr_planes[0], 1, 0, Mat(), hist_b, 1, &histSize, &histRange, uniform, accumulate); //refers to the blue channel of the image
    calcHist(&bgr_planes[1], 1, 0, Mat(), hist_g, 1, &histSize, &histRange, uniform, accumulate); //refers to the green channel of the image
    calcHist(&bgr_planes[2], 1, 0, Mat(), hist_r, 1, &histSize, &histRange, uniform, accumulate); //refers to the red channel of the image

    // Draw the histograms
    int hist_height = 400, hist_width = 512;
    int bin_width = cvRound((double)hist_width / histSize);
    Mat histImage(hist_height, hist_width, CV_8UC3, Scalar(0, 0, 0));
    normalize(hist_b, hist_b, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(hist_g, hist_g, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(hist_r, hist_r, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_width * (i - 1), hist_height - cvRound(hist_b.at<float>(i - 1))),
            Point(bin_width * (i), hist_height - cvRound(hist_b.at<float>(i))),
            Scalar(255, 0, 0), 2, LINE_AA);
        line(histImage, Point(bin_width * (i - 1), hist_height - cvRound(hist_g.at<float>(i - 1))),
            Point(bin_width * (i), hist_height - cvRound(hist_g.at<float>(i))),
            Scalar(0, 255, 0), 2, LINE_AA);
        line(histImage, Point(bin_width * (i - 1), hist_height - cvRound(hist_r.at<float>(i - 1))),
            Point(bin_width * (i), hist_height - cvRound(hist_r.at<float>(i))),
            Scalar(0, 0, 255), 2, LINE_AA);
    }

    // Display the histogram
    namedWindow("Histogram", WINDOW_AUTOSIZE);
    imshow("Histogram", histImage);
}

void show_histograms(Mat& image, String window_name) {
    // Separate the image into 3 channels
    vector<Mat> bgr_planes;
    split(image, bgr_planes);
   

    // Define the colors for each channel
    Vec3b blue(255, 0, 0);
    Vec3b green(0, 255, 0);
    Vec3b red(0, 0, 255);

    // Draw the histograms
    Mat hist_b = draw_histogram(bgr_planes[0], image, blue);
    Mat hist_g = draw_histogram(bgr_planes[1], image, green);
    Mat hist_r = draw_histogram(bgr_planes[2], image, red);
    

    // Concatenate the histograms and show them
    Mat hist_combined;
    hconcat(hist_b, hist_g, hist_combined);
    hconcat(hist_combined, hist_r, hist_combined);
    imshow(window_name+" Histograms", hist_combined);
    
   
}

   

void Blur(int sliderValue, void*)
{
    if(sliderValue <= 0)
    {
        return;
    }
    Mat output;
    blur(src, output, Size(sliderValue, sliderValue));
    imshow("BlurWindow", output);
}

void MedianBlur(int sliderValue, void*)
{
    if (sliderValue <= 0)
    {
        return;
    }
    sliderValue = 2 * sliderValue - 1;
    Mat output;
    medianBlur(src, output, sliderValue);
    imshow("MedianBlurWindow", output);
}

void GaussianBlur(int sliderValue, void*)
{
    if (sliderValue <= 0)
    {
        return;
    }
    sliderValue = 2 * sliderValue - 1;
    Mat output;
    GaussianBlur(src, output, Size(sliderValue, sliderValue), sigma_min, sigma_min);
    imshow("GaussianBlurWindow", output);
}

void BilateralFilter(int sliderValue, void*)
{
    if (sliderValue <= 0)
    {
        return;
    }
    //sliderValue = 2 * sliderValue - 1;
    Mat output;
    bilateralFilter(src, output, sliderValue, sigmaR_min * 2, sigmaS_min / 2);
    imshow("BilateralFilterWindow", output);

    //setting the slider values
}




int main(int argc, char** argv) {


    

    /*    PART-1 of LAB-2    */

	// Load image
    Mat image = imread("portello.png");

    /*    ORIGINAL RGB IMAGE AND HISTOGRAM   */
    // Show original image
    imshow("Original Image", image);

    // Concatenate histograms of each channel of original image
    vector<Mat> bgr_orig;
    split(image, bgr_orig);
    show_histograms(image, "Original Image");
    /**********************************************/

	/*    EQUALIZED RGB IMAGE AND HISTOGRAM   */
    // Equalize each channel separately
    Mat eq_image;
    vector<Mat> eq_channels;
    for (int i = 0; i < bgr_orig.size(); i++) {
        Mat eq_channel;
        equalizeHist(bgr_orig[i], eq_channel);
        eq_channels.push_back(eq_channel);
    }
    merge(eq_channels, eq_image);
    imshow("Equalized Image (RGB)", eq_image);

    // Concatenate histograms of each channel of equalized image (RGB)
    vector<Mat> bgr_eq;
    split(eq_image, bgr_eq);
    show_histograms(eq_image, "Equalized Image (RGB)");
    /**********************************************/

    /*    EQUALIZED LAB IMAGE AND HISTOGRAM    */
    // Convert image to Lab color space and equalize only L channel
    Mat lab_image;
    cvtColor(image, lab_image, COLOR_BGR2Lab);
    vector<Mat> lab_channels;
    split(lab_image, lab_channels);
    equalizeHist(lab_channels[0], lab_channels[0]);
    merge(lab_channels, lab_image);
    cvtColor(lab_image, lab_image, COLOR_Lab2BGR);
    imshow("Equalized Image (Lab)", lab_image);

    // Concatenate histograms of L channel of equalized image 
    show_histograms(lab_image, "Equalized Image (L Channel)");
    /***********************************************/



    /*    PART-2 of LAB-2    */

    src = lab_image;
    Mat img_blur, img_gaus, img_median, img_bilat;

    //blur(src, img_blur, Size(kernel_length, kernel_length), Point(-1, -1));
    //GaussianBlur(src, img_gaus, Size(kernel_length, kernel_length), 0, 0);
    //medianBlur(src, img_median, kernel_length);
    //bilateralFilter(src, img_bilat, kernel_length, kernel_length * 2, kernel_length / 2);

    //imshow("Normal Blurred Image", img_blur);
    //imshow("Gaussian Blurred Image", img_gaus);
    //imshow("Median Blurred Image", img_median);
    //imshow("Bilateral Filtered Image", img_bilat);


    struct Userdata {int sliderId; std::string sliderName;};


    namedWindow("BlurWindow");
    createTrackbar("Kernel Value", "BlurWindow", &blurMin, blurMax, Blur);
    imshow("BlurWindow", src);


    namedWindow("MedianBlurWindow");
    createTrackbar("Kernel Value", "MedianBlurWindow", &blurMin, blurMax, MedianBlur);
    imshow("MedianBlurWindow", src);


    Userdata userdata0{ 0, "kernelVal" };
    Userdata userdata1{ 1, "sigmaVal" };
    namedWindow("GaussianBlurWindow");
    createTrackbar("kernel Value", "GaussianBlurWindow", &blurMin, blurMax, GaussianBlur, &userdata0);
    createTrackbar("Sigma Value", "GaussianBlurWindow", &sigma_min, sigma_max, GaussianBlur, &userdata1);
    imshow("GaussianBlurWindow", src);


    Userdata userdata2{ 2, "kernelVal" };
    Userdata userdata3{ 3, "sigmaRVal" };
    Userdata userdata4{ 4, "sigmaSVal" };
    namedWindow("BilateralFilterWindow");
    //Creating and naming the trackbar
    createTrackbar("Kernel Value", "BilateralFilterWindow", &blurMin, 10, BilateralFilter, &userdata2);
    createTrackbar("Sigma R Value", "BilateralFilterWindow", &sigmaR_min, sigma_max, BilateralFilter, &userdata3);
    createTrackbar("Sigma S Value", "BilateralFilterWindow", &sigmaS_min, sigma_max, BilateralFilter, &userdata4);
    imshow("BilateralFilterWindow", src);

    

    waitKey(0);
    return 0;
}