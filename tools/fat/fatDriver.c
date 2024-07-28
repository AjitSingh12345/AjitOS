#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef uint8_t bool;
#define yes 1
#define no 0

/*** GOAL: IMPLEMENT FAT DRIVER IN C ***/

typedef struct 
{
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];

    
    /*
    - smaller compilers may add padding bytes to data structures to align 
        to 4 or 8 bytes to improve performance of ops
    - this is concern since bootsector structure wont match whats on disk
    - tell compiler not to do this 
    */ 
} __attribute__((packed)) BootSector;

/**
 * Directory entry structure based on FAT specification 
 */
typedef struct 
{
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) DirEntry;

// global vars
BootSector bootSector;
uint8_t* FAT = NULL;
DirEntry* rootDir = NULL;       // array of directory entries
uint32_t rootDirEndSec;         // save sector # where root direc ends & data region begins (so we dont repeat calc)


/**
 * Reads the boot sector from the disk into a global variable
 * 
 * @return boolean indicating success
 */
bool readBootSector(FILE* diskImage)
{
    return fread(&bootSector, sizeof(bootSector), 1, diskImage) > 0;
}


/**
 * Read specified sectors from the disk image
 * 
 * @param diskImage is the file handle
 * @param sectorNum is the starting sector number
 * @param sectorCount is the number of sectors to read
 * @param outBuffer is the output buffer for the read data
 * @return boolean indicating success
 */
bool readDiskSectors(FILE* diskImage, uint32_t sectorNum, uint32_t sectorCount, void* outBuffer)
{
    bool success = yes;

    // Seek to the appropriate position in the file
    success = success && (fseek(diskImage, sectorNum * bootSector.BytesPerSector, SEEK_SET) == 0);

    // Read the specified number of sectors
    success = success && (fread(outBuffer, bootSector.BytesPerSector, sectorCount, diskImage) == sectorCount);
    
    return success;
}

/**
 * Load the FAT (File Allocation Table) into memory
 * 
 * @param diskImage is a pointer to the disk image file
 * @return boolean indicating success
 */
bool loadFAT(FILE* diskImage)
{
    // Allocate memory for the FAT
    FAT = (uint8_t*) malloc(bootSector.SectorsPerFat * bootSector.BytesPerSector);
    
    // Read the FAT from the disk
    return readDiskSectors(diskImage, bootSector.ReservedSectors, bootSector.SectorsPerFat, FAT);
}


/**
 * Load the root directory into memory
 * 
 * @param diskImage is a pointer to the disk image file
 * @return boolean indicating success
 */
bool readRootDir(FILE* diskImage)
{
    // Calculate beginning position = sum of sizes of previous 2 regions (reserved sectors + file alloc tables)
    uint32_t sectorNum = bootSector.ReservedSectors + bootSector.SectorsPerFat * bootSector.FatCount;

    // Determine how many sectors to read (rounded up) = size of root direc / size of a sector (in bytes)
    uint32_t dirSize = sizeof(DirEntry) * bootSector.DirEntryCount;
    uint32_t sectorCount = (dirSize / bootSector.BytesPerSector) + ((dirSize % bootSector.BytesPerSector) ? 1 : 0);

    // Allocate memory for the root directory
    rootDirEndSec = sectorNum + sectorCount;
    rootDir = (DirEntry*) malloc(sectorCount * bootSector.BytesPerSector);
    return readDiskSectors(diskImage, sectorNum, sectorCount, rootDir);
}

/**
 * Find a file in the root directory
 * 
 * @param filename is the name of the file to search for
 * @return pointer to the directory entry of the file
 */
DirEntry* findFile(const char* filename)
{
    // Iterate through the root directory entries & compare to name param
    for (uint32_t i = 0; i < bootSector.DirEntryCount; ++i)
    {
        if (memcmp(filename, rootDir[i].Name, 11) == 0)
            return &rootDir[i];
    }

    return NULL;
}

/**
 * Read the contents of a file
 * 
 * @param dirEntry is a pointer to the file's directory entry
 * @param diskImage is the file handle of the disk image
 * @param outBuffer is the output buffer for the file contents
 * @return boolean indicating success
 */
bool readFile(DirEntry* dirEntry, FILE* diskImage, uint8_t* outBuffer)
{
    bool success = yes;

    // Keeps track of current cluster -> start @ first cluster from directory entry
    uint16_t currentCluster = dirEntry->FirstClusterLow;

    do {
        // Calculate the LBA of the current cluster
        uint32_t sectorNum = rootDirEndSec + (currentCluster - 2) * bootSector.SectorsPerCluster;

        // Read the current cluster
        success = success && readDiskSectors(diskImage, sectorNum, bootSector.SectorsPerCluster, outBuffer);

        // Advance the output buffer position
        outBuffer += bootSector.SectorsPerCluster * bootSector.BytesPerSector;

        // To determine next cluster, lookup in file alloc table
        // Each entry in table is 12 bits, so calc byte idx in the table = cluster# * 3/2   
        uint32_t fatIndex = currentCluster * 3 / 2;
        if (currentCluster % 2 == 0) {
            // Take lower 12 bits to get next cluster in chain
            currentCluster = (*(uint16_t*)(FAT + fatIndex)) & 0x0FFF;
        } else {
            // Take upper 12 bits to get next cluster in chain
            currentCluster = (*(uint16_t*)(FAT + fatIndex)) >> 4;
        }
        
        // Keep reading while current cluster is < end of chain
    } while (success && currentCluster < 0x0FF8);

    return success;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    // Open disk image
    FILE* diskImage  = fopen(argv[1], "rb");
    if (!diskImage) {
        fprintf(stderr, "Error: Cannot open disk image %s!\n", argv[1]);
        return -1;
    }

    // Read the boot sector
    if (!readBootSector(diskImage)) {
        fprintf(stderr, "Error: Could not read boot sector!\n");
        return -2;
    }

    // Read the FAT (file allocation table)
    if (!loadFAT(diskImage)) {
        fprintf(stderr, "Error: Could not read FAT!\n");
        free(FAT);
        return -3;
    }

    // Read root direc
    if (!readRootDir(diskImage)) {
        fprintf(stderr, "Error: Could not read root directory!\n");
        free(FAT);
        free(rootDir);
        return -4;
    }

    // Find specific file in root direc 
    DirEntry* fileEntry = findFile(argv[2]);
    if (!fileEntry) {
        fprintf(stderr, "Error: Could not find file %s!\n", argv[2]);
        free(FAT);
        free(rootDir);
        return -5;
    }

    // Alloc memory for file contents + 1 extra sector 
    uint8_t* fileBuffer = (uint8_t*) malloc(fileEntry->Size + bootSector.BytesPerSector);
    if (!readFile(fileEntry, diskImage, fileBuffer)) {
        fprintf(stderr, "Error: Could not read file %s!\n", argv[2]);
        free(FAT);
        free(rootDir);
        free(fileBuffer);
        return -5;
    }

    // Print contents of the file
    for (size_t i = 0; i < fileEntry->Size; ++i) {
        // print each char if its printable, else print hex val
        if (isprint(fileBuffer[i])) fputc(fileBuffer[i], stdout);
        else printf("<%02x>", fileBuffer[i]);
    }
    printf("\n");

    free(fileBuffer);
    free(FAT);
    free(rootDir);
    return 0;
}
