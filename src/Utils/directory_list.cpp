#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../../include/Utils.hpp"

#define HTML_HEAD_TITLE "<html>\r\n<head><title>Index of "
#define HTML_HEAD_TO_BODY \
  "</title><meta charset=\"utf-8\"></head>\r\n<body>\r\n<h1>Index of "
#define HTML_BEFORE_LIST "</h1><hr>\r\n<pre>\r\n"
#define HTML_AFTER_LIST "</pre>\r\n<hr>\r\n</body>\r\n</html>"
#define TD_STYLE "style=\"padding-left: 20px;padding-right: 20px;\""

#define A_TAG_START "<a>"
#define CLOSE_A_TAG "</a>"
#define CRLF "\r\n"

static std::string get_file_timetable(struct stat file_status) {
  std::stringstream result;
  size_t size = file_status.st_size;
  time_t mtime = file_status.st_mtime;
  std::tm* modify_time = std::gmtime(&mtime);

  char date_buf[32];
  std::strftime(date_buf, sizeof(date_buf), "%d-%b-%Y %H:%M", modify_time);

  result << date_buf << "</td><td>" << size;
  return result.str();
}

static void generate_root_path(std::string& full_path,
                               const std::string& path) {
  // (req_uri = /) => root가 아닌 경우
  if (path.size() != 1) {
    full_path = path;
  }
  if (full_path.size() != 0) {
    size_t original_size = full_path.size();
    full_path.erase(full_path.begin() + full_path.find_last_of('/'),
                    full_path.end());
    if (full_path.size() == original_size - 1) {
      full_path.erase(full_path.begin() + full_path.find_last_of('/'),
                      full_path.end());
    }
  }
  if (full_path.size() == 0) {
    full_path = "/";
  }
}

static void generate_file_path(std::string& full_path,
                               const std::string& filename,
                               const std::string& path) {
  if (path.size() != 1) {
    full_path = path + "/" + filename;
  } else {
    full_path = filename;
  }
}

static void generate_file_status(std::stringstream& result,
                                 std::string& filename,
                                 const std::string& path) {
  struct stat file_status;
  filename = path + "/" + filename;
  if (stat(filename.c_str(), &file_status) == 0) {
    result << get_file_timetable(file_status);
  }
}

// 앞에 ./가 붙어있어야 되는 것 같음 EX) ../ -> ./../
std::string generate_directory_list(const std::string& path) {
  DIR* dir;
  struct dirent* entry;
  std::stringstream result;
  std::string full_path;

  char* base_name = basename((char*)path.c_str());
  result << HTML_HEAD_TITLE << base_name << HTML_HEAD_TO_BODY << path
         << HTML_BEFORE_LIST;
  result << "<table>";

  if ((dir = opendir(path.c_str())) != NULL) {
    entry = readdir(dir);
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type & DT_DIR) {
        std::string filename = entry->d_name;
        result << "<tr>"
               << "<td>";
        if (filename.compare("..") == 0) {
          generate_root_path(full_path, path);
          result << "📁 " << filename << CLOSE_A_TAG << CRLF;
          continue;
        } else {
          generate_file_path(full_path, filename, path);
        }
        result << "📁 " << filename << "/" << CLOSE_A_TAG << "</td>"
               << "<td " << TD_STYLE << ">";
        generate_file_status(result, filename, path);
        result << "</td>"
               << "</tr>" << CRLF;
      }
    }
    rewinddir(dir);
    while ((entry = readdir(dir)) != NULL) {
      if ((entry->d_type & DT_DIR) == false) {
        std::string filename = entry->d_name;
        if (filename.size() > 1 && filename[0] == '.' && filename[1] != '.')
          continue;
        result << "<tr>";
        result << "<td>";
        if (path.size() != 1) {
          full_path = path + filename;
        } else {
          full_path = filename;
        }
        result << "📄 " << filename << CLOSE_A_TAG << "</td>"
               << "<td " << TD_STYLE << ">";
        generate_file_status(result, filename, path);
        result << "</td>"
               << "</tr>" << CRLF;
      }
    }
    result << "</table>" << HTML_AFTER_LIST;
    closedir(dir);
  }
  return result.str();
}
