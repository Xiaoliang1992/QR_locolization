/**
 * @file april_tags.cpp
 * @brief Example application for April tags library
 * @author: Michael Kaess
 *
 * Opens the first available camera (typically a built in camera in a
 * laptop) and continuously detects April tags in the incoming
 * images. Detections are both visualized in the live image and shown
 * in the text console. Optionally allows selecting of a specific
 * camera in case multiple ones are present and specifying image
 * resolution as long as supported by the camera. Also includes the
 * option to send tag detections via a serial port, for example when
 * running on a Raspberry Pi that is connected to an Arduino.
 */

using namespace std;

#include <iostream>
#include <cstring>
#include <vector>
#include <list>
#include <sys/time.h>

#include <libv4l2.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <errno.h>

// OpenCV library for easy access to USB camera and drawing of images
// on screen
#include <opencv2/opencv.hpp>

// April tags detector and various families that can be selected by command line option
#include "AprilTags/TagDetector.h"
#include "AprilTags/Tag16h5.h"
#include "AprilTags/Tag25h7.h"
#include "AprilTags/Tag25h9.h"
#include "AprilTags/Tag36h9.h"
#include "AprilTags/Tag36h11.h"


// Needed for getopt / command line options processing
#include <unistd.h>
extern int optind;
extern char *optarg;


const char* windowName = "apriltags_demo";
cv::Mat map1, map2;

// utility function to provide current system time (used below in
// determining frame rate at which images are being processed)
double tic() {
  struct timeval t;
  gettimeofday(&t, NULL);
  return ((double)t.tv_sec + ((double)t.tv_usec)/1000000.);
}


#include <cmath>

#ifndef PI
const double PI = 3.14159265358979323846;
#endif
const double TWOPI = 2.0*PI;

/**
 * Normalize angle to be within the interval [-pi,pi].
 */
inline double standardRad(double t) {
  if (t >= 0.) {
    t = fmod(t+PI, TWOPI) - PI;
  } else {
    t = fmod(t-PI, -TWOPI) + PI;
  }
  return t;
}

/**
 * Convert rotation matrix to Euler angles
 */
void wRo_to_euler(const Eigen::Matrix3d& wRo, double& yaw, double& pitch, double& roll) {
    yaw = standardRad(atan2(wRo(1,0), wRo(0,0)));
    double c = cos(yaw);
    double s = sin(yaw);
    pitch = standardRad(atan2(-wRo(2,0), wRo(0,0)*c + wRo(1,0)*s));
    roll  = standardRad(atan2(wRo(0,2)*s - wRo(1,2)*c, -wRo(0,1)*s + wRo(1,1)*c));
}


class Demo {

  AprilTags::TagDetector* m_tagDetector;
  AprilTags::TagCodes m_tagCodes;

  bool m_draw; // draw image and April tag detections?
  bool m_timing; // print timing information for each tag extraction call

  int m_width; // image size in pixels
  int m_height;
  double m_tagSize; // April tag side length in meters of square black frame
  double m_fx; // camera focal length in pixels
  double m_fy;
  double m_px; // camera principal point
  double m_py;

  int m_deviceId; // camera id (in case of multiple cameras)

  list<string> m_imgNames;

  cv::VideoCapture m_cap;

  int m_exposure;
  int m_gain;
  int m_brightness;


public:

  // default constructor
  Demo() :
    // default settings, most can be modified through command line options (see below)
    m_tagDetector(NULL),
    m_tagCodes(AprilTags::tagCodes36h11),

    m_draw(true),
    m_timing(false),

    m_width(640),
    m_height(480),
    m_tagSize(0.035),
    m_fx(947),
    m_fy(946),
    m_px(289.1),
    m_py(249.7),

    m_exposure(-1),
    m_gain(-1),
    m_brightness(-1),

    m_deviceId(0)
  {}

  // changing the tag family
  void setTagCodes(string s) {
    if (s=="16h5") {
      m_tagCodes = AprilTags::tagCodes16h5;
    } else if (s=="25h7") {
      m_tagCodes = AprilTags::tagCodes25h7;
    } else if (s=="25h9") {
      m_tagCodes = AprilTags::tagCodes25h9;
    } else if (s=="36h9") {
      m_tagCodes = AprilTags::tagCodes36h9;
    } else if (s=="36h11") {
      m_tagCodes = AprilTags::tagCodes36h11;
    } else {
      cout << "Invalid tag family specified" << endl;
      exit(1);
    }
  }

  void setup() {
    m_tagDetector = new AprilTags::TagDetector(m_tagCodes);

    // prepare window for drawing the camera images
    if (m_draw) {
      cv::namedWindow(windowName, 1);
    }
  }

  void setupVideo() {
    // find and open a USB camera (built in laptop camera, web cam etc)
    m_cap = cv::VideoCapture(m_deviceId);
        if(!m_cap.isOpened()) {
      cerr << "ERROR: Can't find video device " << m_deviceId << "\n";
      exit(1);
    }
    m_cap.set(CV_CAP_PROP_FRAME_WIDTH, m_width);
    m_cap.set(CV_CAP_PROP_FRAME_HEIGHT, m_height);
    cout << "Camera successfully opened (ignore error messages above...)" << endl;
    cout << "Actual resolution: "
         << m_cap.get(CV_CAP_PROP_FRAME_WIDTH) << "x"
         << m_cap.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
    cv::Mat image;
    m_cap >> image;
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    cameraMatrix.at<double>(0, 0) = 4.745537506243416e+02;
    cameraMatrix.at<double>(0, 1) = 0.0;
    cameraMatrix.at<double>(0, 2) = 2.974489590204837e+02;
    cameraMatrix.at<double>(1, 1) = 4.745690628394497e+02;
    cameraMatrix.at<double>(1, 2) = 2.651734958206504e+02;

    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);
    distCoeffs.at<double>(0, 0) = -0.416811439187776;
    distCoeffs.at<double>(1, 0) =  0.206770846440788;
    distCoeffs.at<double>(2, 0) =  0;
    distCoeffs.at<double>(3, 0) =  0;
    distCoeffs.at<double>(4, 0) =  0;
    cv::Size imageSize;
    imageSize = image.size();
    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
        cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
        imageSize, CV_16SC2, map1, map2);
  }

  void print_detection(AprilTags::TagDetection& detection) const {
    cout << "  Id: " << detection.id
         << " (Hamming: " << detection.hammingDistance << ")";

    // recovering the relative pose of a tag:

    // NOTE: for this to be accurate, it is necessary to use the
    // actual camera parameters here as well as the actual tag size
    // (m_fx, m_fy, m_px, m_py, m_tagSize)

    Eigen::Vector3d translation;
    Eigen::Matrix3d rotation;
    detection.getRelativeTranslationRotation(m_tagSize, m_fx, m_fy, m_px, m_py,
                                             translation, rotation);

    Eigen::Matrix3d F;
    F <<
      1, 0,  0,
      0,  -1,  0,
      0,  0,  1;
    Eigen::Matrix3d fixed_rot = F*rotation;
    double yaw, pitch, roll;
    wRo_to_euler(fixed_rot, yaw, pitch, roll);

    cout << "  distance=" << translation.norm()
         << "m, x=" << translation(0)
         << ", y=" << translation(1)
         << ", z=" << translation(2)
         << ", yaw=" << yaw
         << ", pitch=" << pitch
         << ", roll=" << roll
         << endl;
    // Also note that for SLAM/multi-view application it is better to
    // use reprojection error of corner points, because the noise in
    // this relative pose is very non-Gaussian; see iSAM source code
    // for suitable factors.
  }

  void processImage(cv::Mat& image, cv::Mat& image_gray) {
    // alternative way is to grab, then retrieve; allows for
    // multiple grab when processing below frame rate - v4l keeps a
    // number of frames buffered, which can lead to significant lag
    //      m_cap.grab();
    //      m_cap.retrieve(image);

    // detect April tags (requires a gray scale image)
    cv::cvtColor(image, image_gray, CV_BGR2GRAY);
    double t0;
    if (m_timing) {
      t0 = tic();
    }
    vector<AprilTags::TagDetection> detections = m_tagDetector->extractTags(image_gray);
    if (m_timing) {
      double dt = tic()-t0;
      cout << "Extracting tags took " << dt << " seconds." << endl;
    }

    // print out each detection
    cout << detections.size() << " tags detected:" << endl;
    for (int i=0; i<detections.size(); i++) {
      print_detection(detections[i]);
    }

    // show the current image including any detections
    if (m_draw) {
      for (int i=0; i<detections.size(); i++) {
        // also highlight in the image
        detections[i].draw(image);
      }
      imshow(windowName, image); // OpenCV call
    }
  }

  // Load and process a single image
  void loadImages() {
    cv::Mat image;
    cv::Mat image_gray;

    for (list<string>::iterator it=m_imgNames.begin(); it!=m_imgNames.end(); it++) {
      image = cv::imread(*it); // load image with opencv
      processImage(image, image_gray);
      while (cv::waitKey(100) == -1) {}
    }
  }

  // Video or image processing?
  bool isVideo() {
    return m_imgNames.empty();
  }

  // The processing loop where images are retrieved, tags detected,
  // and information about detections generated
  void loop() {

    cv::Mat image;
    cv::Mat image_gray;

    int frame = 0;
    double last_t = tic();
    while (true) {

      // capture frame
      m_cap >> image;

      processImage(image, image_gray);

      // print out the frame rate at which image frames are being processed
      frame++;
      if (frame % 1 == 0) {
        double t = tic();
        cout << "  " << 10./(t-last_t) << " fps" << endl;
        last_t = t;
      }

      // exit if any key is pressed
      if (cv::waitKey(1) >= 0) break;
    }
  }

}; // Demo


// here is were everything begins
int main(int argc, char* argv[]) {
  Demo demo;

  demo.setup();

  if (demo.isVideo()) {
    cout << "Processing video" << endl;

    // setup image source, window for drawing, serial port...
    demo.setupVideo();

    // the actual processing loop where tags are detected and visualized
    demo.loop();

  } else {
    cout << "Processing image" << endl;

    // process single image
    demo.loadImages();

  }

  return 0;
}
