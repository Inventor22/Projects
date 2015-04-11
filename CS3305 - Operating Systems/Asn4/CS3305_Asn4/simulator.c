/*
Name: Dustin Dobransky
Id:   250575030
Date: 11/04/15

Description:
    Simulator for Least Recently Used and Least Frequently Used
    Virtual memory replacement algorithms, with Transitive Lookaside Buffer
    optimization.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "Deque.h"

#define LRU 0 // Least recently used
#define LFU 1 // Least frequently used

#define TLB_SIZE 10 // the Transitive lookaside buffer table size

typedef struct PageInfo
{
    int frameNumber;
    bool valid; // 1 == valid, 0 == invalid
    int numReferences;
} PageInfoEntry;

typedef struct TlbInfo
{
    int pageNum;
    int frameNum;
    int hitCount;
} TlbInfoEntry;

Deque* parseFile(char* fName)
{
    FILE* f = fopen(fName, "r");
    if (f == NULL) {
        perror("Cannot open file\n");
    }

    Deque* d = getDeque();

    char line[255];
    while (fgets(line, 255, f) != NULL) {
        pushBottom(d, atoi(line)); // assuming all input is valid...
    }

    return d;
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        printf("Incorrect number of arguments.\n"
               "1: Number of frames\n"
               "2: traces file name\n"
               "3: LRU or LFU\n");
    }

    int nFrames = atoi(argv[1]);
    char* fName = argv[2];
    char* opt = argv[3];

    const unsigned int pageTableSize = 1024;
    bool mode = 0;
    int numFaults = 0;
    int numUsedFrames = 0;

    int tlbMisses = 0;
    int tlbCount = 0;

    if (strcmp(opt, "LRU") == 0 || strcmp(opt, "LFU") == 0) {
        mode = strcmp(opt, "LRU") == 0 ? LRU : LFU;
    } else {
        printf("Wrong input. Need LRU or LFU.\n");
        exit(EXIT_FAILURE);
    }

    Deque* traces = parseFile(fName); // read in page table
    Deque* lru = getDeque(); // most recent on top, oldest on bottom

    // initialize transitive lookaside buffer
    TlbInfoEntry* tlb = (TlbInfoEntry*)malloc(sizeof(TlbInfoEntry) * TLB_SIZE);
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].frameNum = -1;
        tlb[i].hitCount = 0;
        tlb[i].pageNum = -1;
    }

    // initialize page table
    PageInfoEntry* pageTable = (PageInfoEntry*) malloc(sizeof(PageInfoEntry) * pageTableSize);
    for (int i = 0; i < pageTableSize; i++) {
        pageTable[i].frameNumber = 0;
        pageTable[i].numReferences = 0;
        pageTable[i].valid = false;
    }

    while (traces->size > 0) {
        int pageIndex = popTop(traces); // get next trace

        pageTable[pageIndex].numReferences++; // page is referenced, add to count (for LFU), not LRU

        if (pageTable[pageIndex].valid) { // if page is valid, remove it

            // simulate parallel TLB lookup of page #.  (Serial lookup must be done for simulation)
            bool inTlb = false;
            for (int i = 0; i < TLB_SIZE; i++) {
                if (tlb[i].pageNum == pageIndex) {
                    int frameNum = tlb[i].frameNum;
                    /*
                    Code that uses actual physical frame memory
                    */
                    tlb[i].hitCount++;
                    inTlb = true;
                }
            }
            if (!inTlb) {
                tlbMisses++;
                // add missed item to tlb;
                if (tlbCount < TLB_SIZE) {
                    tlb[tlbCount].pageNum  = pageIndex;
                    tlb[tlbCount].frameNum = pageTable[pageIndex].frameNumber;
                } else {
                    // find the least recently used page #, and replace it
                    int minHitCount = INT_MAX, minHitIndex = 0;
                    for (int i = 0; i < TLB_SIZE; i++) {
                        if (tlb[i].hitCount < minHitCount) {
                            minHitCount = tlb[i].hitCount;
                            minHitIndex = i;
                        }
                    }
                    tlb[minHitIndex].pageNum = pageIndex;
                    tlb[minHitIndex].frameNum = pageTable[pageIndex].frameNumber;
                }

                // if the page# was not found in the TLB, use page table instead
                if (mode == LRU) {            // and place back at bottom of Deque
                    removeItem(lru, pageIndex);
                    pushTop(lru, pageIndex);
                }
            }
        } else {
            numFaults++; // item not valid -> numFaults++

            int frameId = 0;
            if (numUsedFrames < nFrames) {
                frameId = numUsedFrames++;
                pushBottom(lru, frameId); // push to bottom of lru stack
            } else {
                switch (mode) {
                    case LRU:
                    {// find least recently used item --> i.e. top of lru stack.
                        int topFrameId = popTop(lru); // remove top item in lru stack
                        frameId = pageTable[topFrameId].frameNumber; // get the frame id
                        pageTable[topFrameId].valid = false; // mark old frame index as invalid
                        pushBottom(lru, topFrameId); // push the new frame id to bottom of lru stack
                        break;
                    }
                    case LFU: // opposite of LRU --> find items which have been used the least
                    {
                        // find items with min. references (closest to 0)
                        // Grab first valid frame with 0 references, or minimum otherwise
                        int minIndex = 0;
                        int minRefs = INT_MAX;
                        for (int i = 0; i < pageTableSize; i++) {
                            if (pageTable[i].valid) {
                                if (pageTable[i].numReferences == 0) {
                                    minIndex = i;
                                    break;
                                }
                                if (pageTable[i].numReferences < minRefs) {
                                    minRefs = pageTable[i].numReferences;
                                    minIndex = i;
                                }
                            }
                        }
                        pageTable[minIndex].valid = false; // Since used, make invalid
                        frameId = pageTable[minIndex].frameNumber; // update new frame number (frameId)
                        break;
                    }
                    default:
                        break;
                }
            }
            pageTable[pageIndex].frameNumber = frameId; // update frame, and reset staistics
            pageTable[pageIndex].numReferences = 0;
            pageTable[pageIndex].valid = true;
        }
    }

    freeDeque(lru); // free all allocated memory
    freeDeque(traces);
    free(pageTable);

    printf("The number of Page Faults was: %d\n"
           "The number of TLB misses was:  %d\n", numFaults, tlbMisses);

    //getchar();

    return EXIT_SUCCESS;
}