#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap(0);  // Open the default camera (you may need to adjust the index)

    if (!cap.isOpened()) {
        std::cerr << "Error opening camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Ball Tracking", cv::WINDOW_AUTOSIZE);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error reading frame." << std::endl;
            break;
        }

        // Convert the frame to HSV color space
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Define the color range for the ball (you may need to adjust these values)
        cv::Scalar lowerBound(59, 111, 0);
        cv::Scalar upperBound(136, 255, 0);

        // Threshold the image to obtain the mask
        cv::Mat mask;
        cv::inRange(hsvFrame, lowerBound, upperBound, mask);

        // Find contours in the mask
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Draw contours and bounding box around the ball
        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 100) {  // Adjust this threshold based on the size of the ball in pixels
                cv::drawContours(frame, std::vector<std::vector<cv::Point>>{contour}, 0, cv::Scalar(0, 255, 0), 2);

                // Get bounding box
                cv::Rect boundingBox = cv::boundingRect(contour);
                cv::rectangle(frame, boundingBox, cv::Scalar(0, 0, 255), 2);

                // Calculate the center of the bounding box (ball position)
                cv::Point center(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
                cv::circle(frame, center, 5, cv::Scalar(255, 0, 0), -1);
            }
        }

        // Display the frame
        cv::imshow("Ball Tracking", frame);

        // Break the loop if the user presses 'Esc'
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}