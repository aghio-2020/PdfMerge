#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <regex>
#include <filesystem>

#include <pdfio.h>
#include <pdfio-content.h>

bool error_cb(pdfio_file_t *pdf, const char *message, void *data)
{
  (void)data; // This callback does not use the data pointer

  fprintf(stderr, "%s: %s\n", pdfioFileGetName(pdf), message);

  // Return true for warning messages (continue) and false for errors (stop)
  return (!strncmp(message, "WARNING:", 8));
}

bool MergePdfFiles(const char* basePdfName, const char* version, std::vector<std::string> const& pdfFilesToMerge)
{   
    pdfio_rect_t media_box = { 0.0, 0.0, 612.0, 792.0 };  // US Letter
    pdfio_rect_t crop_box = { 36.0, 36.0, 576.0, 756.0 }; // w/0.5" margins
    char* error_data = nullptr;
    bool result = true;

    pdfio_file_t* basePdfFile = pdfioFileCreate(basePdfName, version, &media_box, &crop_box, error_cb, error_data);
    

    if (!basePdfFile)
    {
        std::cout << "error creating pdf: " << error_data << "\n";
        return false;
    }

    for (auto& pdfName : pdfFilesToMerge)
    {
        std::filesystem::path absolutePath = std::filesystem::absolute(pdfName);
        std::cout << "merging pdf file: " << absolutePath.string() << "\n";

        pdfio_file_t* pdfReadFile = pdfioFileOpen(absolutePath.string().data(), NULL, NULL, error_cb, error_data);
        if (!pdfReadFile)
        {
            std::cout << "error reading file: " << error_data << "\n";
            return false;
        }
        size_t numPages = pdfioFileGetNumPages(pdfReadFile);

        for (unsigned int j = 0; j < numPages - 1; j++)
        {
            pdfio_obj_t* page = pdfioFileGetPage(pdfReadFile, j);
            if (!page)
            {
                std::cout << "page load error\n";
                return false;
            }
            pdfioPageCopy(basePdfFile, page);
        }
        pdfioFileClose(pdfReadFile);
    }

    pdfioFileClose(basePdfFile);
    return result;
}

bool CheckArgs(int argc, char *argv[], std::string &errorMessage)
{
    std::ostringstream oss;

    if (argc <= 3)
    {
        oss << "Too few arguments, use -h for help\n";
    }

    for (unsigned int i = 1; i < argc; i++)
    {
        std::string argument = argv[i];

        // for now don't support changing version
        if (argument == "-v")
        {
	    return false;
            if (i + 1 < argc)
            {
                std::regex versionPattern(R"(\d+\.\d+)");
                if (!std::regex_match(argv[++i], versionPattern))
                {
                    oss << argv[i] << " is not a valid version\n";
                }
            }
            else
            {
                oss << "Missing arguments, use -h for help\n";
            }
            continue;
        }

        const std::string extension = ".pdf";
        if (argument.length() <= extension.length())
        {
            oss << argv[i] << " Invalid pdf file\n";
        }
        else
        {
            if (argument.substr(argument.length() - extension.length()) != extension)
            {
                oss << argv[i] << " Invalid pdf file, check the file extension\n";
            }    
        }
    }

    errorMessage = oss.str();
    if (errorMessage.empty())
        return true;
    return false;
}

void AskHelp()
{
    std::cout << "Usage:\n"
    << "pdfMerge <PDFFILE1> ... <PDFFILEn> <NEWPDF>\n"
    << "use -h for help\n";
}

int main(int argc, char *argv[])
{
    if (argc == 1 || (strcmp(argv[1], "-h") == 0))
    {
        AskHelp();
        return 0;
    }

    std::string errorMessage;
    if (!CheckArgs(argc, argv, errorMessage))
    {
        std::cout << errorMessage;
        return -1;
    }

    std::vector<std::string> pdfFileNames;
    for (unsigned int i = 1; i < argc - 1; i++)
    {
        pdfFileNames.emplace_back(argv[i]);
    }

    std::string version = "2.0";
    std::string pdfName = argv[argc - 1];
    std::cout << pdfName << " " << version << "\n";
    int count = 0;
    for(auto& pdf : pdfFileNames)
    {
        std::cout << count++ << " -- ";
        std::cout << pdf << "\n";
    }
    MergePdfFiles(pdfName.c_str(), version.c_str(), pdfFileNames);

    return 0;
}
