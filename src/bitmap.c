/**************************************************************
* Class::  CSC-415-01 Summer 2025
* Name:: Derek Yang, Mekonnen Tesfazien, Andrew Brockenborough
* Student IDs:: 923752034 , 923676946 , 921749371
* GitHub-Name:: thats-not-my-name , mokietes, andrewb-03
* Group-Name:: Team Epsilon
* Project:: Epsilon File System
*
* File:: bitmap.c
*
* Description:: Bitmap manipulation functions for filesystem 
* free space management.
*
**************************************************************/

#include <stdlib.h>

#include "bitmap.h"
#include "freeSpace.h"

extern char *freeSpaceMap;

// set bit at position to 1, uses bitwise or with mask
int setBit(int bitmapIndex)
{
    if (freeSpaceMap == NULL) {
        return -1;
    }
    
    int byteIndex = bitmapIndex / BYTE_BITS;
    int bitOffset = bitmapIndex % BYTE_BITS;
    unsigned char mask = 1 << bitOffset;
    
    freeSpaceMap[byteIndex] |= mask;
    return 0;

}

// this function uses bitwise and with a complement mask to clear the bit
// the complement mask has all bits set to 1 except the target bit
int clearBit(int bitmapIndex)
{
    // validates input parameters
    if (freeSpaceMap == NULL) {
        return -1;  // error: NULL bitmap pointer
    }
    
    // calculates which byte contains our target bit
    int byteIndex = bitmapIndex / BYTE_BITS;
    
    // calculates which bit position within that byte (0-7)
    int bitOffset = bitmapIndex % BYTE_BITS;
    
    // creates a mask with only the target bit set to 1
    unsigned char mask = 1 << bitOffset;
    
    // uses bitwise AND with complement to clear the bit
    // and with 0 always clears the bit, AND with 1 preserves the bit
    freeSpaceMap[byteIndex] &= ~mask;
    
    return 0;
}

// toggles a specific bit in a byte array
int toggleBit(int bitmapIndex)
{
    // validates input parameters
    if (freeSpaceMap == NULL) {
        return -1;  // Error: NULL bitmap pointer
    }
    
    // calculates which byte contains our target bit
    int byteIndex = bitmapIndex / BYTE_BITS;
    
    // calculates which bit position within that byte (0-7)
    int bitOffset = bitmapIndex % BYTE_BITS;
    
    // creates a mask with only the target bit set to 1
    unsigned char mask = 1 << bitOffset;
    
    // uses bitwise XOR to toggle the bit
    // XOR with 1 flips the bit, XOR with 0 preserves the bit
    freeSpaceMap[byteIndex] ^= mask;
    
    return 0;
}
