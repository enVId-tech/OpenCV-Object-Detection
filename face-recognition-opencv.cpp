#include <opencv2/opencv.hpp>

int main() {
    // Load the Haar cascade for face detection
    cv::CascadeClassifier faceCascade;
    faceCascade.load("C:/Users/theli/Documents/Visual-Studio-Projects/OPENCV/ObjectDetection/haarcascade_frontalface_default.xml");

    // Open the video capture device
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Failed to open video capture device" << std::endl;
        return -1;
    }

    // Set the video capture resolution to 

    // Create a window to display the video feed
    cv::namedWindow("Face Tracking", cv::WINDOW_NORMAL);

    while (true) {
        // Read a frame from the video capture device
        cv::Mat frame;
        cap.read(frame);

        // Convert the frame to grayscale for face detection
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Detect faces in the grayscale frame
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

        // Draw rectangles around the detected faces
        for (const cv::Rect& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        }

        // Display the frame with face detection
        cv::imshow("Face Tracking", frame);

        // Break the loop if the user presses 'Esc'
        if (cv::waitKey(30) == 27) {
            break;
        }
    }
    
    // Release the video capture device and destroy the window
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
