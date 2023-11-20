#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error opening camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Rubik's Cube Tracking", cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty())
        {
            std::cerr << "Error reading frame." << std::endl;
            break;
        }

        // Convert the frame to HSV color space
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define color ranges in HSV space
        std::vector<std::pair<cv::Scalar, cv::Scalar>> colorRanges = {
            {cv::Scalar(30, 100, 100), cv::Scalar(90, 255, 255)},   // Green
            {cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255)},    // Red
            {cv::Scalar(110, 100, 100), cv::Scalar(130, 255, 255)}, // Blue
            {cv::Scalar(20, 100, 100), cv::Scalar(30, 255, 255)},   // Yellow
            {cv::Scalar(10, 100, 100), cv::Scalar(20, 255, 255)},   // Orange
        };

        std::vector<std::string> colorNames = {"Green", "Red", "Blue", "Yellow", "Orange", "White"};

        for (int i = 0; i < colorRanges.size(); i++)
        {
            // Threshold the image to obtain the mask for the current color
            cv::Mat mask;
            cv::inRange(hsvFrame, colorRanges[i].first, colorRanges[i].second, mask);

            // Find contours in the mask
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

            // Draw contours and bounding box around the squares
            for (const auto &contour : contours)
            {
                double area = cv::contourArea(contour);
                if (area > 100)
                { // Adjust this threshold based on the size of the squares in pixels
                    cv::Rect boundingBox = cv::boundingRect(contour);
                    cv::rectangle(frame, boundingBox, cv::Scalar(0, 255, 0), 2);

                    // Draw the color name
                    cv::putText(frame, colorNames[i], cv::Point(boundingBox.x, boundingBox.y > 15 ? boundingBox.y - 10 : boundingBox.y + 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);
                }
            }
        }

        // Display the frame
        cv::imshow("Rubik's Cube Tracking", frame);

        // Break the loop if the user presses 'Esc'
        if (cv::waitKey(30) == 27)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
