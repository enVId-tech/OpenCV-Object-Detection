#include <opencv2/opencv.hpp>

cv::Mat frame;
cv::Point selectedPoint(-1, -1);

// Mouse callback function to capture the color where the user clicks
void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        selectedPoint = cv::Point(x, y);

        // Convert the frame to HSV color space
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Get the color at the selected point in HSV
        cv::Vec3b color = hsvFrame.at<cv::Vec3b>(selectedPoint);
        std::cout << "Selected Color (HSV): " << static_cast<int>(color[0]) << ", "
                  << static_cast<int>(color[1]) << ", " << static_cast<int>(color[2]) << std::endl;
    }
}

int main() {
    cv::VideoCapture cap(0);  // Open the default camera (you may need to adjust the index)

    if (!cap.isOpened()) {
        std::cerr << "Error opening camera." << std::endl;
        return -1;
    }

    cv::namedWindow("Ball Tracking", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Ball Tracking", onMouse);

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            std::cerr << "Error reading frame." << std::endl;
            break;
        }

        // Convert the frame to HSV color space
        cv::Mat hsvFrame;
        cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

        // Set lower and upper bounds based on the selected color (adjust these values based on your needs)
        cv::Scalar lowerBound, upperBound;
        if (selectedPoint.x != -1 && selectedPoint.y != -1) {
            cv::Vec3b selectedColor = hsvFrame.at<cv::Vec3b>(selectedPoint);
            // You may need to adjust these values based on the selected color
            lowerBound = cv::Scalar(selectedColor[0] - 10, selectedColor[1] - 50, selectedColor[2] - 50);
            upperBound = cv::Scalar(selectedColor[0] + 10, selectedColor[1] + 50, selectedColor[2] + 50);
        } else {
            // Default values if no color is selected
            lowerBound = cv::Scalar(151, 164, 108);
            upperBound = cv::Scalar(241, 244, 188);
        }

        // Threshold the image to obtain the mask
        cv::Mat mask;
        cv::inRange(hsvFrame, lowerBound, upperBound, mask);

        // Find contours in the mask
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        // Draw contours and bounding circle around the ball
        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);
            if (area > 100) {  // Adjust this threshold based on the size of the ball in pixels
                // Find the minimum enclosing circle
                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);

                // Draw the circle
                cv::circle(frame, center, static_cast<int>(radius), cv::Scalar(0, 0, 255), 2);

                // Calculate the center of the circle (ball position)
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