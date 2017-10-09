// Training.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


void PlaySound()
{
    //PlaySound(TEXT("SystemStart"), NULL, SND_ALIAS);
    //PlaySound((LPCTSTR)SND_ALIAS_SYSTEMWELCOME, NULL, SND_ALIAS_ID);  //niestety ale oba nie dzia³aj¹ mi na dziesi¹tce

    //https://msdn.microsoft.com/en-us/library/z5hh6ee9.aspx - fopen_s
    PlaySound(TEXT("Input/JazzTrio.wav"), NULL, SND_FILENAME);
}
WAVEHDR waveHeader; //https://msdn.microsoft.com/en-us/library/dd743837(v=vs.85).aspx
HWAVEOUT hwaveout;
WAVEFORMATEX waveFormatex; //https://msdn.microsoft.com/en-us/library/windows/desktop/dd390970(v=vs.85).aspx 
                           // uklad http://tiny.systems/software/soundProgrammer/WavFormatDocs.pdf
bool loadFile(std::string path = "Input/JazzTrio.wav")
{
    FILE *file;
    auto error = fopen_s(&file, path.c_str(), "rb");  //rb = read binary
    if (error)
    {
        std::cout << "Error opening file" << path;
        return false;
    }
    if (file)
    {
        
        std::cout << "File good\n";
        std::string id(4,'0'); //four bytes to hold 'RIFF'
                    //int size; //32 bit value to hold file size
       
        fread(&id[0], sizeof(id[0]), 4, file); //read in first four bytes
        std::cout << "should be RIFF : \t" << id << std::endl;
        if (id == "RIFF")
        { //we had 'RIFF' let's continue
            DWORD word;
            fread(&word, sizeof(word), 1, file);
            std::cout << "size : \t" << word << std::endl;
            fread(&id[0], sizeof(id[0]), 4, file);

            
            std::cout << "should be WAVE : \t" << id <<std::endl;
           
            fread(&id[0], sizeof(id[0]), 4, file);
            std::cout << "should be fmt : \t" << id << std::endl;
            
            
            fseek(file, 20, SEEK_SET);  //seek_set znaczy ze przewijamy od poczatku
            
            fread(&waveFormatex.wFormatTag, sizeof(WORD), 1, file); //20
            fread(&waveFormatex.nChannels, sizeof(WORD), 1, file);
            fread(&waveFormatex.nSamplesPerSec, sizeof(DWORD), 1, file); //24
            fread(&waveFormatex.nAvgBytesPerSec, sizeof(DWORD), 1, file); //28
            fread(&waveFormatex.nBlockAlign, sizeof(WORD), 1, file);    //32
            fread(&waveFormatex.wBitsPerSample, sizeof(WORD), 1, file); //34
            
            std::cout << "AudioFormat: " << waveFormatex.wFormatTag << std::endl;
            if (waveFormatex.wFormatTag == 1) waveFormatex.cbSize = 0;
                else fread(&waveFormatex.cbSize, sizeof(WORD), 1, file); //34

            fseek(file, 40, SEEK_SET);
            fread(&(waveHeader.dwBufferLength), sizeof(DWORD), 1, file);
            void* buffer = std::malloc(waveHeader.dwBufferLength);
            waveHeader.lpData = (LPSTR)buffer;
            fread(waveHeader.lpData, waveHeader.dwBufferLength, 1, file);
            waveHeader.dwFlags = 0;
            waveHeader.dwLoops = 0;

           
           std::cout << "WaveOutOpen(): " << waveOutOpen(&hwaveout, WAVE_MAPPER, &waveFormatex, NULL, NULL,
                CALLBACK_NULL) <<std::endl;
           std::cout << "waveOutPrepareHeader(): " << waveOutPrepareHeader(hwaveout, &waveHeader, sizeof(waveHeader)) << std::endl;
           std::cout << "waveOutWrite(): " << waveOutWrite(hwaveout, &waveHeader, sizeof(waveHeader)) << std::endl;
           
           //std::free(buffer); - can't call until music is off
           /*typedef struct {
                WORD  wFormatTag;
                WORD  nChannels;
                DWORD nSamplesPerSec;
                DWORD nAvgBytesPerSec;
                WORD  nBlockAlign;
                WORD  wBitsPerSample;
                WORD  cbSize;
            } WAVEFORMATEX; */
            
           /* HWAVEOUT waveOut;
           
*/
           //std::cin.get();
        }
    }
    return true;
}

int main()
{
    //PlaySound();

    loadFile();


    std::cin.get();
    return 0;
}

