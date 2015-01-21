//This code is public domain, taken from https://gitorious.org/wikibooks-opengl

#include "file.h"

char* file_read(const char* filename)
{
    FILE* input = fopen(filename, "rb");
    if(input == nullptr) return nullptr;

    if(fseek(input, 0, SEEK_END) == -1) return nullptr;
    long size = ftell(input);
    if(size == -1) return nullptr;
    if(fseek(input, 0, SEEK_SET) == -1) return nullptr;

    auto content = new char[(size_t)size+1];
    if(content == nullptr)
        return nullptr;

    fread(content, 1, (size_t)size, input);
    if(ferror(input))
    {
        delete [] content;
        return nullptr;
    }

    fclose(input);
    content[size] = '\0';
    return content;
}