
//#include "impl/delay.hpp"

#include "SavefileManager.cpp"
#include "impl/delay.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include<iostream>


int main() {
#if 0
  SpellCheck spellcheck;

  std::string text = "12 December 1444";

  //TODO: Add whitespace between number and string if missing 12December1444 -> 12 December 1444
  auto vDate = spellcheck(text);

  if(vDate.size() > 0) {
    std::cout << joinStringVector(vDate, "_") << "\n";
  }

  ScreenShot screen(1706, 16, 116, 19);
  cv::Mat image;
  screen(image);
  cv::imshow("image", image);
  cv::waitKey(0);
#endif

  SavefileManager savefileManager;
  for(;;) {
    savefileManager.update();
    //impl::delayMS(50);
  }

  return 0;
}


