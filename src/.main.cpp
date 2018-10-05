// g++ screena.cpp -o screena -lX11 -lXext -Ofast -mfpmath=both -march=native -m64 -funroll-loops -mavx2 `pkg-config opencv --cflags --libs` && ./screena

//#include <tesseract/baseapi.h>

//#include <opencv2/opencv.hpp>  // This includes most headers!

#include <cstdint>
#include <vector>

#include <fstream>
#include <iostream>

#include <time.h>
#define FPS(start) (CLOCKS_PER_SEC / (clock()-start))

#include<assert.h>

#include "impl/delay.hpp"

#include "ScreenShot.cpp"
#include "SpellCheck.cpp"

#include <tesseract/baseapi.h>
// #include <allheaders.h>
#include <sys/time.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using std::string;
using std::vector;

#include <locale>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cctype> // For isdigit()

#define PLATFORM_WIN32 1
#define PLATFORM_LINUX 2
#define PLATFORM_KQUEUE 3

#if defined(_WIN32)
#	define PLATFORM PLATFORM_WIN32
#elif defined(__APPLE_CC__) || defined(BSD)
#	define PLATFORM PLATFORM_KQUEUE
#elif defined(__linux__)
#	define PLATFORM PLATFORM_LINUX
#endif
#define INPUT_FILE              "sample.png"
#define OUTPUT_FOLDER_PATH      string("")

vector<Rect> findLetterContours(cv::Mat img) {
  //Mat large = imread(INPUT_FILE);
  Mat large = img.clone();
  Mat rgb;
  // downsample and use it for processing
  //pyrDown(large, rgb);
  rgb = large.clone();
  Mat small;
  cvtColor(rgb, small, CV_BGR2GRAY);
  // morphological gradient
  Mat grad;
  Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
  morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
  // binarize
  Mat bw;
  threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
  // connect horizontally oriented regions
  Mat connected;
  morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
  morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
  // find contours
  Mat mask = Mat::zeros(bw.size(), CV_8UC1);
  vector<vector<Point>> contours;
  vector<Vec4i> hierarchy;
  findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

  vector<Rect> vRects;//(contours.size());

  // filter contours
  for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
  {
    Rect rect = boundingRect(contours[idx]);
    Mat maskROI(mask, rect);
    maskROI = Scalar(0, 0, 0);
      // fill the contour
    drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
    // ratio of non-zero pixels in the filled region
    double r = (double)countNonZero(maskROI)/(rect.width*rect.height);

    if (r > .45 /* assume at least 45% of the area is filled if it contains text */
        && 
        (rect.height > 8 && rect.width > 8) /* constraints on region size */
        /* these two conditions alone are not very robust. better to use something 
           like the number of significant peaks in a horizontal projection as a third condition */
        )
        {
          rectangle(rgb, rect, Scalar(0, 255, 0), 2);
          vRects.push_back(rect);
        }
    }
  imwrite(OUTPUT_FOLDER_PATH + string("rgb.jpg"), rgb);
  imwrite(OUTPUT_FOLDER_PATH + string("small.jpg"), small);
  imwrite(OUTPUT_FOLDER_PATH + string("large.jpg"), large);
  imwrite(OUTPUT_FOLDER_PATH + string("grad.jpg"), grad);
  imwrite(OUTPUT_FOLDER_PATH + string("bw.jpg"), bw);
  imwrite(OUTPUT_FOLDER_PATH + string("connected.jpg"), connected);
  imwrite(OUTPUT_FOLDER_PATH + string("mask.jpg"), mask);
  return vRects;
}
#if 0
int main() {
  // initilize tesseract OCR engine
  tesseract::TessBaseAPI *myOCR = new tesseract::TessBaseAPI();

  printf("Tesseract-ocr version: %s\n", myOCR->Version());
// printf("Leptonica version: %s\n",
//        getLeptonicaVersion());

if (myOCR->Init(NULL, "eng")) {
  fprintf(stderr, "Could not initialize tesseract.\n");
  exit(1);
                              }

//ScreenShot screen(1706, 27+12, 116, 16);
ScreenShot screen(1706, 4, 116, 16);
cv::Mat img;

for(uint i;; ++i){
  double start = clock();

  screen(img);

  if(!(i & 0b111111))
    printf("fps %4.f  spf %.4f\n", FPS(start), 1 / FPS(start));
  break;

 }

tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line
myOCR->SetPageSegMode(pagesegmode);

//using tesseract::TessBaseAPI;
using tesseract::PageSegMode;

//myOCR->SetPageSegMode(PageSegMode::PSM_SINGLE_LINE);
//myOCR->SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
//myOCR->SetVariable("tessedit_char_whitelist", ".,0123456789");
//myOCR->SetVariable("classify_bln_numeric_mode", "1");

img = cv::imread("sample.png");


#if 1
cv::cvtColor(img, img, CV_BGR2RGBA); 
myOCR->SetImage(img.data, img.cols, img.rows, 4, 4*img.cols);

cv::Mat gray = img.clone();
#else // Convert to Gray

cv::Mat gray;
cv::cvtColor(img, gray, CV_BGR2GRAY); 
//myOCR->SetImage(gray.data().asBuffer(),gray.size().width(),gray.size().height(),gray.channels(),gray.size())

//myOCR->SetImage((uchar*)img.data, img.size().width, img.size().height, img.channels(), img.step1());

myOCR->SetImage(gray.data, gray.cols, gray.rows, gray.channels(), gray.channels()*gray.cols);

cv::imwrite("sample_gray.png", gray);
#endif


//myOCR->TesseractRect( img.data, 1, img.step1(), 0, 0, screen.width, screen.height);
cv::Rect text1ROI(80, 50, 800, 110);
myOCR->TesseractRect( img.data, 1, img.step1(), text1ROI.x, text1ROI.y, text1ROI.width, text1ROI.height);


myOCR->Recognize(0);
const char *text1 = myOCR->GetUTF8Text();

// remove "newline"
std::string t1(text1);

if(t1.size() >= 1) {
  t1.erase(std::remove(t1.begin(), t1.end(), '\n'), t1.end());

  // print found text
  printf("Found text: \n %s \n", t1.c_str());
  
  //printf("found text1: \n");
  //printf(t1.c_str());
  //printf("\n");
  int confidence = myOCR->MeanTextConf();
printf("Confidence: %i \n", confidence);
 
// draw text on original image
cv::Mat scratch = img.clone();//cv::imread("sample.png");

int fontFace = cv::FONT_HERSHEY_PLAIN;
double fontScale = 10;
int thickness = 2;
//cv::putText(scratch, t1, cv::Point(0, 0), fontFace, fontScale, cv::Scalar(0, 255, 0), thickness, 8);
//cv::putText(scratch, t1, cv::Point(0, 0), fontFace, fontScale, cv::Scalar(0, 255, 0), thickness, 8);

putText(scratch, t1, cv::Point(text1ROI.x, text1ROI.y), fontFace, fontScale, cv::Scalar(0, 255, 0), thickness, 8);

rectangle(scratch, text1ROI, cv::Scalar(0, 0, 255), 2, 8, 0);


//cv::imshow("mpv", scratch);
//cv::imshow("mpv", img);
//cv::imwrite("sample.png", img);
cv::imwrite("sample_scratch.png", scratch);

cv::waitKey(3000);


}
  

delete [] text1;

// destroy tesseract OCR engine
myOCR->Clear();
myOCR->End();
}

#endif

void findAndReplaceInString(std::string & data, std::string toSearch, std::string replaceStr) {
  // Get the first occurrence
  size_t pos = data.find(toSearch);
 
  // Repeat till end is reached
  while( pos != std::string::npos)
    {
      // Replace this occurrence of Sub String
      data.replace(pos, toSearch.size(), replaceStr);
      // Get the next occurrence from the current position
      pos = data.find(toSearch, pos + toSearch.size());
    }
}

//#include "../third_party/simplefilewatcher/source/FileWatcher.cpp"
#include <FileWatcher/FileWatcher.h>

/// Processes a file action
class UpdateListener : public FW::FileWatchListener {
public:
  UpdateListener() {}
  bool handleFileAction(FW::WatchID watchid, const FW::String& dir, const FW::String& filename,
                        FW::Action action)
  {
    //std::cout << currentDate << std::endl;
    //
    //if(currentDate.size() <= 6) return false;

    //if (filename.find("Backup") != std::string::npos) return false; // Check if filename contains 'Backup'

    // Insert delimiter _ and se if the last 2 elements are digits
    //
    // TODO: Can check if the last three elements are digits if I insert '_' between characters and
    // numbers with extract_ints(), I can then replace any superflous '_' with:
    // string.replace('__', '_')

    std::stringstream ss(filename.substr(0, filename.size()-4)); // remove '.tmp'
    std::string word;
    int count = 0;

    while(std::getline(ss, word, '_')) { // count '_' to discard any savefile that already has dates
      if(std::isdigit(word[0])) {
        count++;
      }
    }

    //TODO: Get Country tag 'Timurids1444_11_11' -> Timurids
    // for now Timurids1444 is savefile name

    //TODO: Keep track on which file autosave belongs to
    // if user manually saves, set that any autosave filename as ${Country}.eu4.1445_12_1

    if (count >= 2 && currentSavefile == "autosave" && filename.find("autosave") == std::string::npos ) {
    currentSavefile = filename.substr(0, filename.find('_')-4);
    std::cout << "Detected filename: "<< currentSavefile << std::endl;
                                                            //return false;
                                                            }

                                                          if (filename.substr(filename.size() - 4) != ".tmp") return false; // If file isn't .tmp, return

                                                          if(action == 2) { // Deleted, I believe
                                                            std::string actual_save = filename.substr(0, filename.size() - 4) + ".eu4";

                                                            std::ifstream src(dir + "/" + filename.substr(0, filename.size() - 4) + ".eu4", std::ios::binary);

                                                            if(filename.find("autosave.tmp") != std::string::npos) { // If file contains 'autosave.tmp'
                                                              if(currentDate.substr(currentDate.size()-1) != "1") { // Make sure Day is the first of the month
                                                                //currentDate.insert(currentDate.size(),"1");
                                                                currentDate[currentDate.size()-1] = '1';
                                                              }

                                                            }      
                                                            //std::string newDate = currentDate; 
                                                            //findAndReplaceInString(newDate, " ", "_");

                                                            std::ofstream  dst(dir + "/" + currentSavefile + "." + currentDate + ".eu4",  std::ios::binary);
                                                            dst << src.rdbuf();
                                                                   std::cout << "Created savefile: " << currentSavefile + "." + currentDate + ".eu4" <<std::endl;

                                                                                                                                                       return true;
                                                                                                                                                       }
                                                                                                                                                      return false;

                                                                                                                                                      //if(action == FW::Action::Delete && currentDate.size() != 0) {
                                                                                                                                                      //std::cout << "DIR (" << dir + ") FILE (" + filename + ") has event " << action << std::endl;
                                                                                                                                                      /*
                                                                                                                                                        if (filename.substr(filename.size() - 3) != "eu4") return;
                                                                                                                                                        std::cout << "DIR (" << dir + ") FILE (" + filename + ") has event " << action << std::endl;


                                                                                                                                                        if(action == FW::Action::Add && currentDate.size() != 0) {
                                                                                                                                                        if (filename.find("Backup") != std::string::npos) return; // Check if filename contains 'Backup'

                                                                                                                                                        std::ifstream  src(dir + "/" + filename, std::ios::binary);
                                                                                                                                                        std::ofstream  dst(dir + "/" + filename + "." + currentDate,   std::ios::binary);
                                                                                                                                                        dst << src.rdbuf();
                                                                                                                                                      */
                                                                                                                                                      }

                                                                         public:
                                                                                                        std::string currentDate;
                                                                                                        std::string currentSavefile = "autosave";
                                                                                                        };



#if (PLATFORM == PLATFORM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#if (PLATFORM == PLATFORM_WIN32)
#include <Shlobj.h>  // need to include definitions of constants
#endif

                                                                                                      int main(int argc, char* argv[]) {
                                                                                                        FW::FileWatcher* fileWatcher = new FW::FileWatcher();
#if (PLATFORM == PLATFORM_WIN32)
#include <Shlobj.h>  // need to include definitions of constants
                                                                                                        WCHAR path[MAX_PATH];
                                                                                                        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path))) {
                                                                                                          std::string save_path{path};
                                                                                                        }
#endif

#if (PLATFORM == PLATFORM_LINUX)
                                                                                                        const char *homedir;

                                                                                                        if ((homedir = getenv("HOME")) == NULL) {
                                                                                                          homedir = getpwuid(impl::getuid())->pw_dir;
                                                                                                      }
                                                                                                      std::string save_path = std::string(homedir) + "/.local/share/Paradox Interactive/Europa Universalis IV/save games";
#endif

                                                                                                      auto updateListener = new UpdateListener();

                                                                                                      //FW::WatchID watchid = fileWatcher->addWatch(save_path, new UpdateListener());
                                                                                                      FW::WatchID watchid = fileWatcher->addWatch(save_path, updateListener);


                                                                                // initilize tesseract OCR engine
                                                                                tesseract::TessBaseAPI *myOCR =
                                                                                new tesseract::TessBaseAPI();

                                                                                printf("Tesseract-ocr version: %s\n",
                                                                                       myOCR->Version());
                                                                              // printf("Leptonica version: %s\n",
                                                                              //        getLeptonicaVersion());

                                                                              if (myOCR->Init(NULL, "eng")) {
                                                                     fprintf(stderr, "Could not initialize tesseract.\n");
                                                                     exit(1);
                                                                   }

                                                                   tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line
                                                                   //tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(3);  // Default value -- Best results, for now
                                                                   //tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(6);  // Assume a single uniformbb	

                                                                   myOCR->SetPageSegMode(pagesegmode);
                                                                 myOCR->SetVariable("tessedit_enable_dict_correction", "1");
                                                            myOCR->SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
                                                          myOCR->SetVariable("tessedit_char_whitelist", ".,0123456789");
                                         myOCR->SetVariable("classify_bln_numeric_mode", "0");



                                       namedWindow("tesseract-opencv", 0);
                                       //Mat image = imread("sample.png", CV_LOAD_IMAGE_COLOR);
                                       //Mat image = imread("large.jpg", CV_LOAD_IMAGE_COLOR);

                                       //ScreenShot screen(1706, 27+9, 110, 19);
                                       ScreenShot screen(1706, 16, 116, 19);
  
                                       //ScreenShot screen(1706, 19, 116, 16);
                                       SpellCheck spellcheck;
                                       std::string last_word = "";
                                       int last_day = 0;
                                       std::vector<std::string> vDate;

                                       //std::map<const char*, int> months;
                                       std::map<std::string, int> months;

                                       months["january"] = 1;
                                       months["february"] = 2;
                                       months["march"] = 3;
                                       months["april"] = 4;
                                       months["may"] = 5;
                                       months["june"] = 6;
                                       months["july"] = 7;
                                       months["august"] = 8;
                                       months["september"] = 9;
                                       months["october"] = 10;
                                       months["november"] = 11;
                                       months["december"] = 12;

                                       Mat image;

                                       for(;;) {
                                         screen(image);
                                         assert(image.cols !=0 & image.rows != 0);

                                         // set region of interest (ROI), i.e. regions that contain text
                                         //Rect text1ROI(, 0, screen.width, screen.height);
                                         //Rect text2ROI(190, 200, 550, 50);

                                         //  Rect text1ROI(0, 0, image.cols, image.rows);

                                         // recognize text
                                         //  myOCR->TesseractRect( image.data, 1, image.step1(), text1ROI.x, text1ROI.y, text1ROI.width, text1ROI.height);


                                         vector<Rect> vRects;
                                         //vRects = findLetterContours(image);
                                         Rect rect;
                                         if (!vRects.empty()) {
                                           //auto rect = vRects[3];
                                           //assert(vRects.size() == 3);
                                           //printf("\n----------------%i\n", vRects[3].x);


                                           //for (auto rect : vRects) {
                                           //myOCR->TesseractRect( image.data, 1, image.step1(), rect.x, rect.y, rect.width, rect.height);
                                           //printf(myOCR->GetUTF8Text());
                                           //}
                                         } else {
                                           rect = Rect{0, 0, image.cols, image.rows};
                                         }


#if 0
                                         cv::cvtColor(image, image, CV_BGR2RGBA);
                                         myOCR->SetImage(image.data, image.cols, image.rows, 4, 4*image.cols);

                                       myOCR->SetImage(gray.data().asBuffer(),gray.size().width(),gray.size().height(),gray.channels(),gray.size());
#else

                 cv::cvtColor(image, image, CV_BGR2GRAY); 
                 // binarization



                 cv::resize(image, image, cv::Size(image.cols*2, image.rows*2), cv::INTER_NEAREST);
                 threshold(image, image, 0.0, 255.0, THRESH_BINARY_INV | THRESH_OTSU);

#if 0//NOTE: Might improve OCR
                 cv::Mat oldImage = image.clone();
                 //cv::GaussianBlur(oldImage, image, cv::Size(0, 0), 3);
                 //cv::addWeighted(oldImage, 1.5, image, -0.5, 0, image);
#endif

                 myOCR->SetImage((uchar*)image.data, image.size().width, image.size().height, image.channels(), image.step1());

               //myOCR->SetImage(image.data, image.cols, image.rows, image.channels(), image.channels()*image.cols);


#endif

               imwrite("test.png", image);
               //imshow("test.png", image);
   
               //break;
    
               //myOCR->TesseractRect( image.data, 1, image.step1(), rect.x, rect.y, rect.width, rect.height);
               myOCR->Recognize(0);

    const char *text1 = myOCR->GetUTF8Text();

  /*
    Scalar color = Scalar(0,255,0);
    for( int i = 0; i < vRects.size(); i++ )
    {
    if(vRects[i].area()<100)continue;
    printf("\n Test\n");
    //drawContours( image, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    //rectangle( image, vRects[i].tl(), vRects[i].br(),color, 2, 8, 0 );
    }
  */


  // remove "newline"
  string t1(text1);
  if (t1.size() >= 1) {
  t1.erase(std::remove(t1.begin(), t1.end(), '\n'), t1.end());

  // print found text
  //printf("found text1: \n");
  //printf(t1.c_str());
  //printf("\n");

  //std::string text = "31DecenberES";
  vDate = spellcheck(t1);
#if 1 // If we want Numerical months
  for (auto &w : vDate) {
    if(w.size() >= 3) {
    if(!std::isdigit(w[0])) {
      w = std::to_string(months[w]);
    }
  }
}
#endif

  t1 = joinStringVector(vDate, "_");
//TODO: Sanity check that the Date Month & Year isn't lower than the last
//for (auto const &w : vDate) {
//std::cout << w << std::endl;
//}

//findAndReplaceInString(t1, " ", "_");
if(last_word != t1) {
  std::cout << t1 << std::endl;
                     last_word = t1;
                     }
                   }
      
  

               delete [] text1;

               image.release();
               updateListener->currentDate = t1;
             fileWatcher->update();
  if(vDate.size() != 0) {
    int day = 0;
    std::stringstream ss(vDate.back());
    ss >> day;
if( day >= 28 || day <=2) {
  //impl::delayMS(500);
  //TODO: Find a reliable way to tell update speed so we can speed up processing if need be.
  // and we can also be sure it's an autosave if it's close to the end of the month + the speed
  // high
           } else if (std::abs(((31 - day)- (31 - last_day))% 31) <= 5 ){
               impl::delayMS(1000);
             }

             }
  }

waitKey(7000);



// destroy tesseract OCR engine
myOCR->Clear();
myOCR->End();

return 0;
}
