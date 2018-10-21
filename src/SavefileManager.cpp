#include <FileWatcher/FileWatcher.h>
#include "SpellCheck.cpp"
#include "ScreenShot.cpp"
#include "OCR.cpp"

#include "impl/mkdir.cpp"

#include <sstream>
#include <iostream>
#include <fstream>

#include <sys/types.h> // needed by:
#include <unistd.h> // For impl::getuid()
#include <pwd.h> // for getpwuid()

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

unsigned int createDateValue(std::vector<std::string> dates) {
  unsigned int result = 0;
  for(int i = 0; i<dates.size();i++) {
    switch(i) {
    case(0):
      result += std::stoi(dates[i])*10000;
    case(1):
      result += std::stoi(dates[i])*100;
    case(2):
      result += std::stoi(dates[i]);
    }
  }
  return result;
}

#if (PLATFORM == PLATFORM_WIN32)
#include <stdlib.h>

typedef unsigned __int16 U2;
typedef unsigned __int32 U4;
typedef unsigned __int64 U8;

#define bswap2 _byteswap_ushort
#define bswap4 _byteswap_ulong
#define bswap8 _byteswap_uint64

#elif (PLATFORM == PLATFORM_LINUX)
typedef uint16_t U2;
typedef uint32_t U4;
typedef uint64_t U8;
#define bswap2 __builtin_bswap16
#define bswap4 __builtin_bswap32
#define bswap8 __builtin_bswap64


#endif


U8 constexpr DateToInt(char* const sz) {
  U4 y = *(U4*)sz, m = *(U2*)(sz + 5), d = *(U2*)(sz + 8);
  return ((U8)bswap4(y) << 32) | (bswap2(m) << 16) | bswap2(d);
}
U8 const DateToInt(std::string const &sz) {
  return DateToInt(const_cast<char*>(sz.c_str()));
}

int64 constexpr CompareDate(char* const sz1, char* const sz2) {
  return DateToInt(sz1) - DateToInt(sz2);
}

int64 const CompareDate(std::string const sz1, std::string const sz2) {
  return DateToInt(sz1) - DateToInt(sz2);
}

inline bool FileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

/// Processes a file action
class UpdateListener : public FW::FileWatchListener {
public:
  UpdateListener(FW::FileWatcher *fileWatcher, std::set<std::string> *savefileDirs, std::pair<int, int> resolution={1920, 1080}) :
      fileWatcher(fileWatcher),  spellCheck("../data/dictionary.txt"), savefileDirs(savefileDirs),
      //screen(1706, 16, 116, 19)
      screen(resolution.first - 214, 16, 124, 19)
  {
    ocr = new OCR();
  }

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

    if (filename.substr(filename.size() - 4) != ".tmp") return false; // If file isn't .tmp, return

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

    if(filename.find("autosave") == std::string::npos) {
      if (count >= 2) {
        int npos = filename.find('_');
        if(npos != std::string::npos) {
          currentSavefile = filename.substr(0, npos-4);
        }

        currentSavefile = filename.substr(0, filename.find('.'));
        std::cout << "Detected filename: "<< currentSavefile << std::endl;

      } else {
        int i;
        for(i = 0; i<filename.size(); i++) {
          if(std::isdigit(filename[i])) {
            break;
          }
        }
        currentSavefile = filename.substr(0, i-4);
        std::cout << "Detected filename: "<< currentSavefile << std::endl;
      }
    }

    // When $name$.tmp save file is deleted, it has completed its moved to $name$.eu4
    if(action == 2 ) { // Add - 1, Delete - 2, Modifed - 3 
      std::string actual_save = filename.substr(0, filename.size() - 4) + ".eu4";

      std::ifstream src(dir + "/" + actual_save, std::ios::binary);

      screen(image);
      auto text = ocr->recognize(image);
      std::cout << "Detected text: " << text << std::endl;

      auto vDate = spellCheck(std::string(text));
      if(vDate.size() == 0 || ! std::isdigit(vDate.back()[0])) return false;

      currentDate = joinStringVector(vDate, "_");
      //assert(currentDate.size() > 0);

      U8 dateValue = DateToInt(currentDate.c_str()); // creates a value of Year + Month

      if(dateValue < DateToInt("1444_11_11")) return false; // NOTE: Discards dates less than 1444 11 11

      std::cout <<"Date Value: "<< dateValue << std::endl;

      if(lastDateValue < dateValue) {
        subversionInt = 0;
        if(filename.find("autosave.tmp") != std::string::npos) { // If file contains 'autosave.tmp'
          if(currentDate.substr(currentDate.size()-1) != "1") { // Make sure Day is the first of the month and is lower date than last save
            currentDate[currentDate.size()-1] = '1';
          }
        }
      } else if (lastDateValue == dateValue) {
        subversionInt++;
      }

      lastDateValue = dateValue;

      // Check if current watcher is in subdirectory
      if(!isSubdirectory && FileExists(dir+"/"+currentSavefile)) {
        isSubdirectory = true;
      }
      //std::string newDate = currentDate; 
      //findAndReplaceInString(newDate, " ", "_");

      if (savefileDirs->count(currentSavefile) == 0 && impl::makePath(dir + "/" + currentSavefile)) {
        savefileDirs->insert(currentSavefile);
        fileWatcher->addWatch(dir + "/" + currentSavefile, this);
      } else {
        //std::cout << "Failed to create \"" +currentSavefile + "\" folder in \"" << dir << "\"" << std::endl;
      }

      if(savefileDirs->count(currentSavefile) != 0 && !isSubdirectory) {
        for(int i{subversionInt};i<10;i++) {
          auto const filename = dir + "/" + currentSavefile +"/" + currentSavefile + "." + std::to_string(subversionInt) +"." + currentDate + ".eu4";
          if(FileExists(filename)) subversionInt++;
          else break;
        }
        std::ofstream  dst(dir + "/" + currentSavefile +"/" + currentSavefile + "." + std::to_string(subversionInt) +"." + currentDate + ".eu4",  std::ios::binary);
        dst << src.rdbuf();
        std::cout << "Created savefile: " << currentSavefile  +"/" + currentSavefile + "." + std::to_string(subversionInt) + "." + currentDate + ".eu4" <<std::endl;
      } else {
        for(int i{subversionInt};i<64;i++) {
          auto const filename = dir + "/" + currentSavefile + "." + std::to_string(subversionInt) +"." + currentDate + ".eu4";
          if(FileExists(filename)) subversionInt++;
          else break;
        }
        std::ofstream  dst(dir + "/"  + currentSavefile + "." + std::to_string(subversionInt) +"." + currentDate + ".eu4",  std::ios::binary);
        dst << src.rdbuf();
      std::cout << "Created savefile: " <<  currentSavefile + "." + std::to_string(subversionInt) + "." + currentDate + ".eu4" <<std::endl;
      }
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

  void refresh() {
    image.release();
    screen(image);
  }

  //NOTE: Need to be configured
  void setResolution(std::pair<int, int> resolution) {
    std::cout << "setResolution isn't configure, forcing close." << std::endl;
    assert(false);
    screen.x = resolution.first - 850;
    screen.y = 16;
    screen.width = 116;
    screen.height = 19;
  };

public:
  std::string currentSavefile = "";
  std::string currentDate;
  U8 lastDateValue = 0;
  int subversionInt = 0;
  bool isSubdirectory = false;

  std::ofstream *ofs;
private:
  ScreenShot screen;
  SpellCheck spellCheck;
  OCR *ocr;
  cv::Mat image;

  FW::FileWatcher *fileWatcher;
  std::unique_ptr<std::set<std::string>> savefileDirs;
};

struct SavefileManager
{
  SavefileManager() : fileWatcher{} {//, updateListener(&fileWatcher,&savefileDirs) {
#if (PLATFORM == PLATFORM_WIN32)
#include <Shlobj.h>  // need to include definitions of constants
    WCHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path))) {
      std::string save_path{path + "/Documents/Paradox Interactive/Europa Universalis IV/save games"};
    }
#endif

#if (PLATFORM == PLATFORM_LINUX)
    const char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
    }
    std::string save_path = std::string(homedir) + "/.local/share/Paradox Interactive/Europa Universalis IV/save games";
#endif
    //FW::WatchID watchid = fileWatcheraddWatch(save_path, new UpdateListener());

    //TODO: Find resolution in E U 4/logs aswell (?)
    std::pair<int, int> resolution;
    std::ifstream ifs_systemlog(save_path + "/../settings.txt");
    if (ifs_systemlog) {
      std::string word;
      while(ifs_systemlog >> word) {
        int pos = word.find("x=");
        if(pos != std::string::npos) {
          resolution.first = std::stoi(word.substr(pos+2, word.size()));
        } else {
          pos = word.find("y=");
          if (pos != std::string::npos) {
            resolution.second = std::stoi(word.substr(pos+2, word.size()));
            break;
          }
        }
      }
    }
    std::cout << resolution.first << ", " << resolution.second << std::endl;

    updateListener = new UpdateListener(&fileWatcher,&savefileDirs, resolution);
    fileWatcher.addWatch(save_path, updateListener);

    std::ifstream ifs("../data/savefile_dirs.txt");
    if (ifs) {
      std::string word;
      while (ifs >> word) {
        std::cout << word << std::endl;
        fileWatcher.addWatch(save_path + "/" + word, updateListener); 
      }
    }
    ifs.close();


    //    ofs.open("../data/savefile_dirs.txt", std::ios::app);

    //fileWatcher.addWatch(save_path, &updateListener);

     }
  void update() {
    fileWatcher.update();
#if 0
    std::ofstream ofs;
    ofs.open("../data/savefile_dirs.txt");
    assert(ofs);
    for(auto &w : savefileDirs) {
      ofs.write(w.c_str(), w.size()*sizeof(char));
    }
    //ofs.write(currentSavefile.c_str(), currentSavefile.size()*sizeof(char));
    ofs.close();
#endif
  }

  ~SavefileManager() {
    ofs.open("../data/savefile_dirs.txt");
    assert(ofs);
    for(auto &w : savefileDirs) {
      ofs.write(w.c_str(), w.size()*sizeof(char));
      //      ofs << w;
    }

    ofs.close();
  }

  FW::FileWatcher fileWatcher;
  UpdateListener *updateListener;
  std::set<std::string> savefileDirs;
  std::set<std::string> savefileDates;
  std::ofstream ofs;

};
