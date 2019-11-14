#include "function.h"


std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
  std::vector<std::string> names;
  std::string search_path = SEARCH_PATH + "/*.*";
  WIN32_FIND_DATA fd;
  HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      // read all (real) files in current folder
      // , delete '!' read other 2 default folder . and ..
      if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        names.push_back(fd.cFileName);
      }
    } while (::FindNextFile(hFind, &fd));
    ::FindClose(hFind);
  }
  return names;
}

std::vector<std::string> get_directories(const std::string& s)
{
	std::vector<std::string> r;
	for (auto& p : std::filesystem::recursive_directory_iterator(s))
		if (p.is_directory())
			r.push_back(p.path().string());
	return r;
}

bool IsFolderExist(const std::string& folderName)
{
  return std::filesystem::exists(std::filesystem::path(folderName));
}

std::string CreateFolder(const std::string& folderPath)
{
	//std::filesystem::path lookupPath = folderName;
  std::string actualFolderName = folderPath.substr(folderPath.find_first_of('/') + 1, folderPath.length());
	//check if folder exists
	if (IsFolderExist(OUTPUT_PATH + actualFolderName))
	{
		std::cout << "Folder already exists! DO NOT CREATE";
		return OUTPUT_PATH + actualFolderName;
	}

  //std::filesystem::create_directories(lookupPath);
  std::filesystem::create_directories(OUTPUT_PATH + actualFolderName);
  return OUTPUT_PATH + actualFolderName;
}

void CommentFiles(std::vector<std::string> filenames, std::string inpath, std::string outpath)
{
  std::string headerComment("comment-text.txt");

  for (auto& elem : filenames)
  {
    std::ifstream commentText(headerComment, std::ifstream::in);

    if (!commentText.is_open())
    {
      std::cout << "Can't open comment text file.\nEnding program now\n";
      return;
    }

    std::fstream file((inpath + elem), std::fstream::in | std::fstream::out);
    std::fstream newfile((outpath + "/" + elem), std::fstream::out);
    
    std::string line, line2;

	//get line from comment block
    while (getline(commentText, line))
    {
		//find file name tag
		auto pos = line.find(FILENAME_TEXT);
		//if file comment tag found
		if (pos != std::string::npos)
		{
			auto fileNameStartPos = line.find_first_not_of(' ', pos + 6);
			line.replace(fileNameStartPos, line.length(), elem);
		}

		newfile << line << "\n";
    }

    while (getline(file, line))
    {
      newfile << line << std::endl;
    }
    
    file.close();
    commentText.close();
  }


}

void CommentFilesNew(std::vector<std::string> filenames, std::string inpath, std::string outpath)
{
  //loop through all files
  for (auto& elem : filenames)
  {
    //open header comment file
    std::ifstream commentText(HEADER_COMMENT_FILE, std::ifstream::in);

    if (!commentText.is_open())
    {
      std::cout << "Can't open comment text file.\nEnding program now\n";
      return;
    }

    std::fstream file((inpath + elem), std::fstream::in | std::fstream::out);
    std::fstream newfile((outpath + "/" + elem), std::fstream::out);

    std::string line, line2;
    std::map<std::string, std::string>::iterator tagIterator = tagConfigValueMap.begin();
    //get line from comment block
    while (getline(commentText, line))
    {
      //if file tag, put file name
      auto filePos = line.find(tagConfigValueMap[FILENAME_TEXT]);
      //if file comment tag found
      if (filePos != std::string::npos)
      {
        auto fileNameStartPos = line.find_first_not_of(' ', filePos + tagConfigValueMap[FILENAME_TEXT].length());
        line.replace(fileNameStartPos, line.length(), elem);
        newfile << line << "\n";
        continue;
      }

      //find given tags in config file
      auto pos = line.find(tagIterator->second);
      //if file comment tag found
      if (pos != std::string::npos)
      {
        auto firstNotOfSpace = line.find_first_not_of(' ', pos + (tagIterator->second).length());
        auto firstNotOfTab = line.find_first_not_of('\t', pos + (tagIterator->second).length());
        auto fileNameStartPos = firstNotOfSpace > firstNotOfTab ? firstNotOfSpace : firstNotOfTab;
        auto tmp = configValueMap[tagIterator->first];
        line.replace(fileNameStartPos, line.length(), tmp);
      }

      newfile << line << "\n";
    }

    while (getline(file, line))
    {
      newfile << line << std::endl;
    }

    file.close();
    commentText.close();
  }


}

void CommentFileWrapper()
{
	auto names = get_all_files_names_within_folder(SEARCH_PATH);
	auto folders = get_directories(SEARCH_PATH);

  //loop through folders (if any) and comments all files inside
  for (const auto& elem : folders)
  {
    //get all files in elem
    auto filesInFolder = get_all_files_names_within_folder(elem);
    auto outpath = CreateFolder(elem);
    //comment
    //CommentFiles(filesInFolder, elem, outpath);
    CommentFilesNew(filesInFolder, elem, outpath);
  }

  //comment any files that are not in subdirectories
	//CommentFiles(names);
  CommentFilesNew(names, SEARCH_PATH, OUTPUT_PATH);
}

bool ReadConfigFile()
{
  std::fstream config(CONFIG_FILE, std::fstream::in);
  std::fstream tagConfig(TAGCONFIG_FILE, std::fstream::in);

  if (!config.is_open() || !tagConfig.is_open())
  {
    std::cout << "Unable to open config file";
    return false;
  }

  std::string currLine, tag, var;
  while (getline(tagConfig, currLine))
  {
    auto pos = currLine.find_first_of('=');
    tagConfigValueMap[currLine.substr(0, pos)] = currLine.substr(pos + 1, currLine.length());
  }

  while (getline(config, currLine))
  {
    auto pos = currLine.find_first_of('=');
    configValueMap[currLine.substr(0, pos)] = currLine.substr(pos + 1, currLine.length());
  }

  return true;
}

int main(int argc, char argv[])
{
  ReadConfigFile();
	CommentFileWrapper();

  return 0;
}