#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>
#include <regex>

#include <pdfio.h>
#include <pdfio-content.h>

// Pequeña herramienta que ingresa nombres de archivos pdf, verifica que tengan extension .pdf y los une en un nuevo pdf
// Recibe los pdfs que se van a usar y al final el nombre del pdf que se va a crear a partir de los mismos

// Clase para almacenar datos del pdf y operaciones comunes
struct PDF_Data
{
public:
    PDF_Data(const char* name, const char* version) 
    : myPdfName(name)
    , myPdfVersion(version)
    , myNumPages(0)
    {}

    void MergePdfFiles(std::vector<std::string> &pdfFileNames)
    {   
        pdfio_rect_t media_box = { 0.0, 0.0, 612.0, 792.0 };  // US Letter
        pdfio_rect_t crop_box = { 36.0, 36.0, 576.0, 756.0 }; // w/0.5" margins

        myPdfFile = pdfioFileCreate(myPdfName.c_str(), myPdfVersion.c_str(), &media_box, &crop_box, NULL, NULL);
        std::vector<pdfio_file_t*> pdfFiles;

        unsigned int numFiles = pdfFileNames.size();
        for (unsigned int i = 0; i < numFiles; i++)
        {
            pdfio_file_t* pdfReadFile = pdfioFileOpen(pdfFileNames[i].c_str(), NULL, NULL, NULL, NULL);
            size_t numPages = pdfioFileGetNumPages(pdfReadFile);
            for (unsigned int j = 0; j < numPages; j++)
            {
                pdfio_obj_t* page = pdfioFileGetPage(pdfReadFile, j);
                pdfioPageCopy(myPdfFile, page);
            }
            pdfioFileClose(pdfReadFile);
        }

        pdfioFileClose(myPdfFile);
    }

private:
    pdfio_file_t *myPdfFile;
    std::string myPdfName;
    std::string myPdfVersion;
    int myNumPages;
};

bool CheckArgs(int argc, char *argv[], std::string &errorMessage)
{
    std::ostringstream oss;

    if (argc <= 3)
    {
        oss << "Too few arguments, use -h for help\n";
    }

    for (unsigned int i = 1; i <= argc; i++)
    {
        std::string argument = argv[i];
        if (argument == "-v")
        {
            if (i + 1 < argc)
            {
                std::regex versionPattern(R"(\d+\.\d+)");
                if (std::regex_match(argv[++i], versionPattern))
                {
                    i++;
                }
                else
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
    << "pdfMerge [PDFFILE] ... [PDFFILE] -v [n.n] [PDFFILENAME]\n"
    << "use -h for help\n"
    << "use -v to specify the pdf version (OPTIONAL)\n";
}

int main(int argc, char *argv[])
{
    if (argc == 1 || strcmp(argv[1], "-h") == 0)
    {
        AskHelp();
        return 0;
    }

    std::string errorMessage;
    if (!CheckArgs(--argc, argv, errorMessage))
    {
        std::cout << errorMessage;
        return -2;
    }

    char *version = (char*)malloc(sizeof(char) * 5);
    if (strcmp(argv[argc - 2], "-v") == 0)
        strcpy(version, argv[argc - 1]);
    else
        strcpy(version, "2.0");

    std::vector<std::string> pdfFileNames;
    for (unsigned int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0)
            break;
        pdfFileNames.push_back(argv[i]);
        std::cout << i << " ";
    }

    PDF_Data pdfFile(argv[argc], version);
    pdfFile.MergePdfFiles(pdfFileNames);

    return 0;
}
