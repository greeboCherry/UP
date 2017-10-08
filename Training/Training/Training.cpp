// Training.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
    //PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS);
    //PlaySound((LPCTSTR)SND_ALIAS_SYSTEMWELCOME, NULL, SND_ALIAS_ID);  //niestety ale oba nie dzia³aj¹ mi na dziesi¹tce

    //https://msdn.microsoft.com/en-us/library/z5hh6ee9.aspx - fopen_s
    FILE *fp;
    std::string path = "sound.wav";
    auto error = fopen_s(&fp, path.c_str(), "rb");  //rb = read binary
    if (error)
    {
        std::cout << "Error opening file" << path;
    }
    if (fp)
        {
            char id[4]; //four bytes to hold 'RIFF'
            //int size; //32 bit value to hold file size

            fread(id, sizeof(char), 4, fp); //read in first four bytes
            if (id == "RIFF")
            { //we had 'RIFF' let's continue
                fread(id, sizeof(char), 4, fp);
                //read in 4 byte string now
                if (id == "WAVE")
                { //this is probably a wave file since it contained "WAVE"
                    fread(id, sizeof(char), 4, fp); //read in 4 bytes "fmt ";
                }
            }
        }



    std::cin.get();
    return 0;
}

