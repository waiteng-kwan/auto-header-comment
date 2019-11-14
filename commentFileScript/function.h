#include <Windows.h>
#include <vector>             //store file names
#include <string>
#include <iostream>           //debug
#include <fstream>
#include <map>
#include <filesystem> // C++17

#define TAGCONFIG_FILE "TAG_CONFIG.txt"
#define CONFIG_FILE "config.txt"
#define FILENAME_TEXT "FILE"

const std::string SEARCH_PATH = "to-comment/";
const std::string OUTPUT_PATH = "commented/";
const std::string HEADER_COMMENT_FILE = "comment-text.txt";

std::map<std::string, std::vector<std::string>> directoryFilesMap;
std::map<std::string, std::string> tagConfigValueMap;
std::map<std::string, std::string> configValueMap;


std::vector<std::string> get_all_files_names_within_folder(std::string folder);
std::vector<std::string> get_directories(const std::string& s);
bool IsFolderExist(const std::string& folderName);
std::string CreateFolder(const std::string& folderPath);
void CommentFiles(std::vector<std::string> filenames, std::string inpath, std::string outpath);
void CommentFilesNew(std::vector<std::string> filenames, std::string inpath, std::string outpath);
void CommentFileWrapper();
bool ReadConfigFile();