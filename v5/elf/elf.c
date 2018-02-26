#include "lib.h"
#include "elf.h"
#include "printf.h"

typedef struct{
    unsigned char magic[4];
    unsigned char class;
    unsigned char format;
    unsigned char version;
    unsigned char abi;
    unsigned char abi_version;
    unsigned char reserved[7];
}elf_id;

typedef struct{
    elf_id  id;
    short   type;
    short   arch;
    long    version;
    long    entry_point;
    long    program_header_offset;
    long    section_header_offset;
    long    flags;
    short   header_size;
    short   program_header_size;
    short   program_header_num;
    short   section_header_size;
    short   section_header_num;
    short   section_name_index;
}elf_header;

typedef struct{
    long    type;
    long    offset;
    long    virtual_addr;
    long    physical_addr;
    long    file_size;
    long    memory_size;
    long    flags;
    long    align;
}elf_program_header;

static int ElfCheck(elf_header* header)
{
    if(memcmp(header->id.magic, "\x7f" "ELF", 4)){
        return -1;
    }

    if(header->id.class     != 1) return -1; // ELF32
    if(header->id.format    != 1) return -1; // little endian
    if(header->id.version   != 1) return -1; // version 1
    if(header->type         != 2) return -1; // Executable file
    if(header->version      != 1) return -1; // version 1 
    if(header->arch         !=40) return -1; // ARM

    return 0;
}

static int ElfLoadProgram(elf_header* header)
{
    elf_program_header* phdr;

    for(int i=0; i<header->program_header_num; i++){
        // get program header
        phdr = (elf_program_header*)((char*)header + header->program_header_offset + header->program_header_size * i);

        if(phdr->type != 1){ // loadable segment?
            continue;
        }

        printf("%x ", phdr->offset); 
        printf("%x ", phdr->virtual_addr); 
        printf("%x ", phdr->physical_addr); 
        printf("%x ", phdr->file_size); 
        printf("%x ", phdr->memory_size); 
        printf("%x ", phdr->flags); 
        printf("%x\n", phdr->align); 
    }
    return 0;
}

int ElfLoad(char* buf)
{
    elf_header* header = (elf_header*)buf;

    if(ElfCheck(header) < 0){
        return -1;
    }

    if(ElfLoadProgram(header) < 0){
        return -1;
    }

    return 0;
}
