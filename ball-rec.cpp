#include <opencv2/opencv.hpp>

cv::Mat frame;
cv::Point selectedPoint(-1, -1);

void onMouse(int event, int x, int y, int flags, void *userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        selectedPoint = cv::Point(x, y);

        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        cv::Vec3b color = hsvFrame.at<cv::Vec3b>(selectedPoint);
        std::cout << "Selected Color (HSV): " << static_cast<int>(color[0]) << ", "
                  << static_cast<int>(color[1]) << ", " << static_cast<int>(color[2]) << std::endl;
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

        cv::Scalar lowerBound, upperBound;
        if (selectedPoint.x != -1 && selectedPoint.y != -1)
        {
            cv::Vec3b selectedColor = hsvFrame.at<cv::Vec3b>(selectedPoint);
            // Adjust the sensitivity by changing the value range
            lowerBound = cv::Scalar(selectedColor[0] - 6, selectedColor[1] - 60, selectedColor[2] - 60);
            upperBound = cv::Scalar(selectedColor[0] + 6, selectedColor[1] + 60, selectedColor[2] + 35);
        }
        else
        {
            lowerBound = cv::Scalar(151, 164, 108);
            upperBound = cv::Scalar(241, 244, 188);
        }

        cv::Mat mask;
        cv::inRange(hsvFrame, lowerBound, upperBound, mask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto &contour : contours)
        {
            double area = cv::contourArea(contour);
            if (area > 100)
            {
                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);

                cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 0, 255), 2);
                cv::circle(frame, center, 5, cv::Scalar(255, 0, 0), -1);
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