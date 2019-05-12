
#include <vector>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ARR_SIZE 0x100

int main(int argc, char* argv[])
{
    //check arguments
    if(argc != 2) { printf("usage: tad-extractor [input.tad]\n"); return 0; }
    
    //open input file
    FILE* in = fopen(argv[1], "rb");
    
    if(in == NULL) { printf("main() error: cannot open input file: %s\n", argv[1]); return 1; }
    
    //read number of files in tad archive
    char number_of_files_txt[ARR_SIZE] = { 0 };
    
    int b = 0;
    while(isalnum(b = fgetc(in)))
    {
        static int i = 0;
        number_of_files_txt[i++] = (char)b;
    }
    
    unsigned number_of_files = atoi(number_of_files_txt);
    
    printf("LOG: number of files in archive: %u\n", number_of_files);
    
    //read entries, entry = [g%u, size of file]
    std::vector<unsigned> entries;
    entries.reserve(number_of_files);
    
    for(unsigned i = 0; i < number_of_files; i++)
    {
        printf("LOG: entry type: 0x%02x\n", fgetc(in));
        char entry_size_txt[ARR_SIZE] = { 0 };
        int j = 0;
        while(isalnum(b = fgetc(in)))
        {
            entry_size_txt[j++] = (char)b;
        }
        printf("LOG: entry size: %u\n", atoi(entry_size_txt));
        entries.push_back(atoi(entry_size_txt));
    }
    
    //there is for some reason one more entry type
    //with no following size -> skip it
    fgetc(in);
    
    //extract files from the rest of the archive
    for(unsigned i = 0; i < number_of_files; i++)
    {
        FILE* out = fopen(std::string("out" + std::to_string(i) + ".png").c_str(), "wb");
        
        char* buffer = (char*)malloc(entries[i]);
        
        fread(buffer,  1, entries[i], in);
        fwrite(buffer, 1, entries[i], out);
        
        free(buffer);
        
        fclose(out);
    }
    
    fclose(in);
    
    return 0;
}
