#include "stack-searcher.hpp"

using std::cout;

StackSearcher::StackSearcher(const void *stackStart, const MAPS_ENTRY &text, const pid_t pid, const size_t max_heap_obj)
        : stackStart_(stackStart),
          textStart_(text.start),
          textEnd_(text.end),
          textSize_((char *) text.start - (char *) text.end),
          pid_(pid),
          max_heap_obj_(max_heap_obj) {}

//If the address is within the .text part of the target binary
[[nodiscard]]bool StackSearcher::AddrIsInText(const void *addr) const {
    const bool IsInText = addr >= textStart_ && addr <= textEnd_;
    return IsInText;
}

[[nodiscard]] bool StackSearcher::AddrIsOnHeap(const void *addr, const void *heapStart, const void *heapEnd) const {
    const bool IsOnHeap = addr >= heapStart && addr <= heapEnd;
    return IsOnHeap;
}

std::vector<RemoteHeapPointer>
StackSearcher::findHeapPointers(const void *curStackEnd, const MAPS_ENTRY &heap, size_t framesToSearch) const {

    if (framesToSearch == 0) {
        framesToSearch = UINTMAX_MAX;
    }
    size_t framesSearched = 0;

    const size_t curStackSize = (char *) curStackEnd - (char *) stackStart_;
    const char *stackCopy = DeepCopy(pid_, stackStart_, curStackSize);

    auto matches = std::vector<RemoteHeapPointer>();
    size_t zeroCount = 0;
    for (size_t i = 0; i < curStackSize; i += (sizeof(void *))) {
        size_t current = 0;
        memcpy(&current, (char *) stackCopy + i, sizeof(void *));
        if (current == 0) {
            zeroCount++;
            continue;
        }


        /*If our target programs source has a function that has a pointer to a heap object
        Like
        void foobar(){
            int* x = new int;
        }
        then pointerLocation is &x and addressPointedTo is x
        */
        auto pointerLocation = (void **) ((char *) stackCopy + i);
        auto addressPointedTo = (void *) *pointerLocation;


        if (AddrIsOnHeap(addressPointedTo, heap.start, heap.end)) {
            void *actualAddr = (void *) ((char *) stackStart_ + i);
            size_t sizePointedTo = GetMallocMetadata(addressPointedTo, pid_, max_heap_obj_, true);
            cout << "---------------------------\n";
            cout << "Global pointer to heap memory found\n";
            cout << "Pointer memory is at: " << actualAddr << "\n";
            cout << "Which points to : " << addressPointedTo << "\n";
            cout << "Pointer found at stack offset:" << i << "\n";
            cout << "Which points to block of size:" << sizePointedTo << "\n";
            cout << "---------------------------\n";
            const struct RemoteHeapPointer result = {actualAddr, addressPointedTo, sizePointedTo};
            matches.push_back(result);

        } else if (AddrIsInText(addressPointedTo)) {
            std::cout << "Frame end found return addr to :" << addressPointedTo << "\n";
            framesSearched++;
            if (framesSearched > framesToSearch) {
                break;
            }
        }
    }
    delete[] stackCopy;
    return matches;
}
