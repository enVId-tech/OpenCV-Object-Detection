#include <opencv2/opencv.hpp>
#include <cmath>

cv::Mat frame;
cv::Rect selectedRegion(-1, -1, 0, 0);
cv::Scalar selectedColor(0, 0, 0);

// Sensitive parameters for color detection
const int addedSaturation = 10;
const int removedSaturation = 10;
constexpr int regionSize = 10; // In pixels
bool printHSV = true;
const int ballSize = 100; // In pixels

// Function to calculate the percentage of color matching
double calculateColorMatchingPercentage(const cv::Scalar &color1, const cv::Scalar &color2)
{
    double matchingPercentage = 0.0;

    for (int i = 0; i < 3; ++i)
    {
        double colorDifference = std::abs(color1[i] - color2[i]);
        matchingPercentage += (1.0 - colorDifference / 255.0);
    }

    return (matchingPercentage / 3.0) * 100.0;
}

// Mouse callback function to capture the color where the user clicks
void onMouse(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        selectedRegion = cv::Rect(x - regionSize, y - regionSize, 2 * regionSize, 2 * regionSize);
        selectedRegion &= cv::Rect(0, 0, frame.cols, frame.rows);
        cv::rectangle(frame, selectedRegion, cv::Scalar(0, 255, 0), 2);

        if (!selectedRegion.empty())
        {
            cv::Mat hsvFrame;
            cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
            cv::Mat selectedRegionHSV = hsvFrame(selectedRegion);
            selectedColor = cv::mean(selectedRegionHSV);

            cv::Scalar lowerBound(selectedColor[0] - removedSaturation, 50, 50);
            cv::Scalar upperBound(selectedColor[0] + addedSaturation, 255, 255);

            if (printHSV)
            {
                std::cout << "Selected Color (HSV): " << static_cast<int>(selectedColor[0]) << ", "
                          << static_cast<int>(selectedColor[1]) << ", " << static_cast<int>(selectedColor[2]) << std::endl;
                std::cout << "Lower Bound (HSV): " << static_cast<int>(lowerBound[0]) << ", "
                          << static_cast<int>(lowerBound[1]) << ", " << static_cast<int>(lowerBound[2]) << std::endl;
                std::cout << "Upper Bound (HSV): " << static_cast<int>(upperBound[0]) << ", "
                          << static_cast<int>(upperBound[1]) << ", " << static_cast<int>(upperBound[2]) << std::endl;
            }
        }
        cv::imshow("Ball Tracking", frame);
    }
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Ball Tracking", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Ball Tracking", onMouse);

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error reading frame." << std::endl;
            break;
        }

        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
        cv::Scalar lowerBound(selectedColor[0] - removedSaturation, 50, 50);
        cv::Scalar upperBound(selectedColor[0] + addedSaturation, 255, 255);

        if (selectedRegion.width > 0 && selectedRegion.height > 0)
        {
            cv::Mat selectedRegionHSV = hsvFrame(selectedRegion);
            selectedColor = cv::mean(selectedRegionHSV);
            lowerBound = cv::Scalar(selectedColor[0] - removedSaturation, 50, 50);
            upperBound = cv::Scalar(selectedColor[0] + addedSaturation, 255, 255);
        }

        cv::Mat mask;
        cv::inRange(hsvFrame, lowerBound, upperBound, mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto &contour : contours)
        {
            double area = cv::contourArea(contour);
            if (area > ballSize)
            {
                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);

                double colorMatchingPercentage = calculateColorMatchingPercentage(selectedColor, selectedColor);
                constexpr double colorMatchingThreshold = 25.0;

                if (colorMatchingPercentage >= colorMatchingThreshold)
                {
                    cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 0, 255), 2);
                    cv::circle(frame, center, 5, cv::Scalar(255, 0, 0), -1);
                }
            }
        }

        cv::imshow("Ball Tracking", frame);

        if (cv::waitKey(30) == 27)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}