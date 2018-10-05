
#include<tesseract/baseapi.h>

#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/highgui/highgui.hpp> // For debug imwrite / imshow / waitKey

#include<iostream>

using tesseract::TessBaseAPI;
using tesseract::PageSegMode;

class OCR : public tesseract::TessBaseAPI {
 public:
  OCR() {
    if (Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
    }
    auto pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line

    SetPageSegMode(pagesegmode);
  }

  std::string recognize(cv::Mat image) {
    if(image.cols ==0 || image.rows == 0) return "";

    // Convert to grayscale
    cv::cvtColor(image, image, CV_BGR2GRAY); 

    // Dubble in size
    cv::resize(image, image, cv::Size(image.cols*2, image.rows*2), cv::INTER_NEAREST);

    // Binarization
    threshold(image, image, 0.0, 255.0, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

#if 0//NOTE: Might improve OCR
    cv::Mat oldImage = image.clone();
    cv::GaussianBlur(oldImage, image, cv::Size(0, 0), 3);
    //cv::addWeighted(oldImage, 1.5, image, -0.5, 0, image);
#endif

    assert(image.cols != 0);
    cv::imwrite("test.png", image);

    SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());

    //myOCR->SetImage(image.data, image.cols, image.rows, image.channels(), image.channels()*image.cols);

    Recognize(0);

    return std::string(GetUTF8Text());
  }
};
