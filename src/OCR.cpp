
#include<tesseract/baseapi.h>

#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/highgui/highgui.hpp> // For debug imwrite / imshow / waitKey

#include<iostream>

#include "KNearestOcr.h"
#if 0
Pix *mat8ToPix(cv::Mat *mat8) {
  Pix *pixd = pixCreate(mat8->size().width, mat8->size().height, 8);
  for(int y=0; y<mat8->rows; y++) {
    for(int x=0; x<mat8->cols; x++) {
      pixSetPixel(pixd, x, y, (l_uint32) mat8->at<uchar>(y,x));
    }
  }
  return pixd;
}

cv::Mat pix8ToMat(Pix *pix8)
{
  cv::Mat mat(cv::Size(pix8->w, pix8->h), CV_8UC1);
  uint32_t *line = pix8->data;
  for (uint32_t y = 0; y < pix8->h; ++y) {
    for (uint32_t x = 0; x < pix8->w; ++x) {
      mat.at<uchar>(y, x) = GET_DATA_BYTE(line, x);
    }
    line += pix8->wpl;
  }
  return mat;
}
#endif

using tesseract::TessBaseAPI;
using tesseract::PageSegMode;

class OCR : public tesseract::TessBaseAPI {
 private:
  Config _config;
  KNearestOcr *_pOcr;
 public:
  OCR() : _config() {
    if (Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
    }
    auto pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line

    SetPageSegMode(pagesegmode);

    _config.loadConfig();
    _pOcr = new KNearestOcr(_config);
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

    Recognize(0);

#ifdef DEBUG
    // Iterate through the results.
    auto iterator = GetIterator();

    std::string lastUTF8Text;
    float lastConfidence;
    int count = 0;
    do {
      //lastUTF8Text = iterator->GetUTF8Text(tesseract::PageIteratorLevel::RIL_WORD);
      lastUTF8Text = iterator->GetUTF8Text(tesseract::PageIteratorLevel::RIL_SYMBOL);
      lastConfidence = iterator->Confidence(tesseract::PageIteratorLevel::RIL_SYMBOL);
      //      iterator->GetBinaryImage(tesseract::PageIteratorLevel::RIL_SYMBOL);
      int bleft, btop, bright, bbottom;
      iterator->BoundingBoxInternal(tesseract::PageIteratorLevel::RIL_SYMBOL, &bleft, &btop, &bright, &bbottom);
      auto const subimg = image({bleft, btop, bright - bleft, bbottom - btop});
      cv::imwrite("output/"+lastUTF8Text+".png", subimg);
      _pOcr->learn(subimg, *lastUTF8Text.c_str());
      count++;
      std::cout <<"Conf: "<< lastConfidence << "\t Text: " << lastUTF8Text << std::endl;
    } while (iterator->Next(tesseract::PageIteratorLevel::RIL_SYMBOL));

    _pOcr->saveTrainingData();
#endif
    return std::string(GetUTF8Text());
  }
};
