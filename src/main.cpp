
//#include "impl/delay.hpp"

#include "SavefileManager.cpp"
#include "impl/delay.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

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

#if 0
  auto save = Savefile("Test.tmp");
  save.date = "1444_11_11";
  std::cout << save << std::endl;
  impl::makePath("./"+ save.name); 
  assert(impl::directoryExist("./"+ save.name)); 

  std::fstream file_in{"text.txt", std::ios::in};
  Savefile save_in;
  std::set<Savefile> savefiles;
  while(!file_in.eof()) {
    std::string line;
    std::getline(file_in, line);
    if(line.size() > 4) {
      line >> save_in;
      savefiles.insert(save_in);
    }
  }
  file_in.close();

#if 0
  std::ofstream file_out;
  file_out.open("text.txt", std::ios::out | std::ios::trunc);
  for(auto const save : savefiles) {
    file_out << save << std::endl;
  }
  file_out.close();
#endif

  std::cout << save_in << std::endl;
  assert(save_in == save);
#else

  return 0;
  SavefileManager savefileManager;
  for(;;) {
    savefileManager.update();
    //impl::delayMS(50);
  }
#endif
return 0;
}


