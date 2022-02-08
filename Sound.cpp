#include "Global.h"

Sound::Sound(string filename) :Filename(filename), wfx({ 0 }), buffer({ 0 })
{
    //Open File
    if FAILED(errorcode = OpenFile())
    {
        OutputDebugStringA("Failed to Open File\n");
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    DWORD filetype;
    //check the file type, should be fourccWAVE or 'XWMA'
    //Locating RIFF chunk
    if FAILED(errorcode = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition))
    {
        OutputDebugStringA("Failed to Find RIFF\n");
    }
    //Identifying FileType
    if FAILED(errorcode = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition))
    {
        OutputDebugStringA("Failed to Read RIFF filetype\n");
    }
    if FAILED(errorcode = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition))
    {
        OutputDebugStringA("Failed to Read RIFF filetype\n");
    }
    if (filetype != fourccWAVE)
    {
        OutputDebugStringA("Wrong FileType\n");
    }
    //Locating FMT chunk
    errorcode = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    if FAILED(errorcode)
    {
        OutputDebugStringA("Failed to Find FMT\n");
    }
    errorcode = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
    if FAILED(errorcode)
    {
        OutputDebugStringA("Failed to Read FMT ChunkSize\n");
    }

 
    //fill out the audio data buffer with the contents of the fourccDATA chunk
    errorcode = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    if FAILED(errorcode)
    {
        OutputDebugStringA("Failed to Find Data\n");
    }
    //Locating Audio Data
    BYTE* pDataBuffer = new BYTE[dwChunkSize];
 
    errorcode = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
    if FAILED(errorcode)
    {
        OutputDebugStringA("Failed to Read Data\n");
    }

    //Populate Buffer
    buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes 
    if (buffer.AudioBytes == 0)
    {
        OutputDebugStringA("Audio Data is 0\n");
    }
    buffer.pAudioData = pDataBuffer;  //buffer containing audio data
    buffer.Flags = 0; // tell the source voice not to expect any data after this buffer
}

 
HRESULT Sound::OpenFile()
{
    // Open the file
    hFile = CreateFile(
        Filename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        OutputDebugStringA("hFile failed\n");
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
    {
        OutputDebugStringA("hFile failed\n");
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return S_OK;
}

Sound::~Sound()
{
    
}

HANDLE Sound::GetFile()
{
    return hFile;
}

HRESULT Sound::FindChunk(HANDLE& hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;
    HRESULT hr = S_OK;
    /*
        SetFilePointer Takes a File Handle,
        The number of bytes to move the file pointer,
        starting position of file pointer
    */
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    while (hr == S_OK)
    {
        /*
            WAV file is arranged with ChunkID first then ChunkSize
            dwChunkType is a buffer that receives the chunkID for the first 4 bytes
            dwRead Receives the number of bytes read
            dwChunkSize receives the ChunkDataSize for the next 4 bytes
        */
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        /*
            Check the ChunkType that was read
            We could be looking for the data/format/RIFF
        */
        switch (dwChunkType)
        {
            /*
                Store the FileFormat (WAVE) in dwFileType if ChunkType is fourccRIFF
                If not, Move File Pointer foward by 4 Bytes
            */
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            /*
                Move File Pointer by ChunkDataSize to the next SubChunk
            */
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        /*
            Read ChunkID and ChunkSize which offsets by total of 8 Bytes (4 Bytes Each)
        */
        dwOffset += sizeof(DWORD) * 2;

        /*
            Found the chunkType specified by User
        */
        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }
        /*
            Set dwOffset to the next SubChunk
        */
        dwOffset += dwChunkDataSize;

        /*
            Checking if dwRIFFDataSize is 0 or smaller,
            Suggesting there is a problem with the file Chunks
        */
        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    //if cannot find chunk, return S_False;
    return S_FALSE;

}

HRESULT Sound::ReadChunkData(HANDLE& hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}