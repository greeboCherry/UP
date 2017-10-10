// SoundCard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <iostream>
#include <Windows.h> // Additional Dependencies += winmm.lib
#include <DSound.h>


WAVEHDR waveHeader; //https://msdn.microsoft.com/en-us/library/dd743837(v=vs.85).aspx
HWAVEOUT hwaveout;
WAVEFORMATEX waveFormatex; //https://msdn.microsoft.com/en-us/library/windows/desktop/dd390970(v=vs.85).aspx 
                           // uklad http://tiny.systems/software/soundProgrammer/WavFormatDocs.pdf

void* g_buffer;

bool loadFileAndPlayWaveOut(std::string path = "Input/JazzTrio.wav")
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
                std::cout << "File good, reading " << path << std::endl;
        std::string stringData(4, '0'); //four bytes to hold 'RIFF'

        fread(&stringData[0], sizeof(stringData[0]), 4, file); //read in first four bytes
        std::cout << "ChunkID: \t" << stringData << std::endl;
        if (stringData == "RIFF")
        { //we had 'RIFF' let's continue
            
            DWORD dIntegerData; //this ain't string :(
            fread(&dIntegerData, sizeof(dIntegerData), 1, file);    //4
            std::cout << "ChunkSize : \t" << dIntegerData << "\t+8" << std::endl;
            
            fread(&stringData[0], sizeof(stringData[0]), 4, file); //8
            std::cout << "Format : \t" << stringData << std::endl;    

            fread(&stringData[0], sizeof(stringData[0]), 4, file); //12
            std::cout << "Subchunk1ID: \t" << stringData << std::endl; 
            
            DWORD Subchunk1Size;
            fread(&Subchunk1Size, sizeof(Subchunk1Size), 1, file); //16
            std::cout << "Subchunk1Size: \t" << Subchunk1Size << std::endl;
            
            fread(&waveFormatex.wFormatTag, sizeof(WORD), 1, file); //20
            std::cout << "AudioFormat: \t" << waveFormatex.wFormatTag << std::endl;
            fread(&waveFormatex.nChannels, sizeof(WORD), 1, file); //22
            std::cout << "NumChannels: \t" << waveFormatex.nChannels << std::endl;
            fread(&waveFormatex.nSamplesPerSec, sizeof(DWORD), 1, file); //24
            std::cout << "SampleRate: \t" << waveFormatex.nSamplesPerSec << std::endl;
            fread(&waveFormatex.nAvgBytesPerSec, sizeof(DWORD), 1, file); //28
            std::cout << "ByteRate: \t" << waveFormatex.nAvgBytesPerSec << std::endl;
            fread(&waveFormatex.nBlockAlign, sizeof(WORD), 1, file);    //32
            std::cout << "BlockAlign: \t" << waveFormatex.nBlockAlign << std::endl;
            fread(&waveFormatex.wBitsPerSample, sizeof(WORD), 1, file); //34
            std::cout << "BitsPerSample: \t" << waveFormatex.wBitsPerSample << std::endl;
            
            //WORD IntegerData;
            //fread(&IntegerData, sizeof(IntegerData), 1, file);
            //std::cout << "ExtraParamSize: \t" << IntegerData << std::endl;
            ////if (waveFormatex.wFormatTag == 1) waveFormatex.cbSize = 0;
            ////else fread(&waveFormatex.cbSize, sizeof(WORD), 1, file); //36

            fseek(file, 20 + Subchunk1Size, SEEK_SET);  //we move at the beiging of "data" sub-chunk
            fread(&stringData[0], sizeof(stringData[0]), 4, file); //read in first four bytes
            std::cout << "Subchank2ID: \t" << stringData << std::endl;

            fread(&(waveHeader.dwBufferLength), sizeof(DWORD), 1, file);
            std::cout << "Subchunk2Size: \t" << waveHeader.dwBufferLength << std::endl;

            g_buffer = std::malloc(waveHeader.dwBufferLength);      //TODO make it RAII
            waveHeader.lpData = (LPSTR)g_buffer;
            fread(waveHeader.lpData, waveHeader.dwBufferLength, 1, file);
            waveHeader.dwFlags = 0;
            waveHeader.dwLoops = 0;

            std::cout << "WaveOutOpen(): " << waveOutOpen(&hwaveout, WAVE_MAPPER, &waveFormatex, NULL, NULL,
                CALLBACK_NULL) << std::endl;
            std::cout << "waveOutPrepareHeader(): " << waveOutPrepareHeader(hwaveout, &waveHeader, sizeof(waveHeader)) << std::endl;
            std::cout << "waveOutWrite(): " << waveOutWrite(hwaveout, &waveHeader, sizeof(waveHeader)) << std::endl;
        }
    }
    return true;
}

bool loadFileAndPlayDirectSound(std::string path = "Input/JazzTrio.wav")
{
    LPDIRECTSOUND8 lpds;
    std::cout << "DirectSoundCreate8(): " << DirectSoundCreate8(NULL, &lpds, NULL) << std::endl;

    DSBUFFERDESC bufferDescription;




    /*int a, b, c;
    LPCGUID guidPtr;
    LPDIRECTSOUND8 ds8Ptr;
    IDirectSound* DS;
    LPUNKNOWN unknownPtr;
    DS->Initialize(NULL);
    */
    //DirectSoundCaptureEnumerate()
//    DirectSoundCreate8(guidPtr, &ds8Ptr, DS);
    
    return true;
}

int main()
{
  //  PlaySound(TEXT("Input/JazzTrio.wav"), NULL, SND_FILENAME);
    //loadFileAndPlayWaveOut("input/gtr-nylon22.wav");

    loadFileAndPlayDirectSound();

    std::cin.get();
    std::free(g_buffer);// - can't call until music is off
    return 0;
}
/*
na sprawko - wszytkie paremtry z headera
sadsad */
