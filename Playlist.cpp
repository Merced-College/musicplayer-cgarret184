#include "Playlist.h"

// Constructor: Initializes a new Playlist object with no tracks and sets current index to 0.
Playlist::Playlist() {
    trackCount = 0;
    currentIndex = 0;
}

// Adds a new AudioTrack to the end of the playlist if there's space. Returns true if successful, false if the playlist is full.
bool Playlist::addTrack(AudioTrack track) {
    if (trackCount >= MAX_TRACKS) return false;

    tracks[trackCount] = track;
    trackCount++;
    return true;
}

// Retrieves the currently selected track for playback. Returns a pointer to the track at currentIndex, or nullptr if the playlist is empty.
AudioTrack* Playlist::getCurrentTrack() {
    if (trackCount == 0) return nullptr;
    return &tracks[currentIndex];
}

// Advances to the next track in the playlist with circular navigation. Returns a pointer to the new current track, or nullptr if empty.
AudioTrack* Playlist::nextTrack() {
    if (trackCount == 0) return nullptr;

    currentIndex = (currentIndex + 1) % trackCount;
    return &tracks[currentIndex];
}

// Moves to the previous track in the playlist with circular navigation. Returns a pointer to the new current track, or nullptr if empty.
AudioTrack* Playlist::previousTrack() {
    if (trackCount == 0) return nullptr;

    if (currentIndex == 0)
        currentIndex = trackCount - 1;
    else
        currentIndex--;

    return &tracks[currentIndex];
}

// Returns the total number of tracks currently in the playlist.
uint8_t Playlist::getTrackCount() {
    return trackCount;
}

// Resets the playlist navigation to the beginning by setting currentIndex to 0.
void Playlist::reset() {
    currentIndex = 0;
}

// Selects the current track by its track number.
// Returns true if the track exists and the current index was updated.
bool Playlist::setCurrentTrackByNumber(uint16_t trackNumber) {
    int index = linearSearchByTrackNumber(trackNumber);
    if (index >= 0) {
        currentIndex = index;
        return true;
    }
    return false;
}

// Get track at specific index
AudioTrack* Playlist::getTrack(uint8_t index) {
    if (index >= trackCount) return nullptr;
    return &tracks[index];
}

static bool equalsIgnoreCase(const char* a, const char* b) {
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return false;
        a++; b++;
    }
    return *a == *b;
}

// Linear search by title - returns index if found, -1 if not found
int Playlist::linearSearchByTitle(const char* title) {
    for (int i = 0; i < trackCount; i++) {
        if (equalsIgnoreCase(tracks[i].getTitle(), title)) {
            return i;
        }
    }
    return -1;
}

// Linear search by track number - returns index if found, -1 if not found
int Playlist::linearSearchByTrackNumber(uint16_t trackNumber) {
    for (int i = 0; i < trackCount; i++) {
        if (tracks[i].getTrackNumber() == trackNumber) {
            return i;
        }
    }
    return -1;
}

// Selection sort by title (ascending order)
void Playlist::sortByTitle() {
    for (int i = 0; i < trackCount - 1; i++) {
        int minIndex = i;
        
        for (int j = i + 1; j < trackCount; j++) {
            if (strcmp(tracks[j].getTitle(), tracks[minIndex].getTitle()) < 0) {
                minIndex = j;
            }
        }
        
        // Swap
        if (minIndex != i) {
            AudioTrack temp = tracks[i];
            tracks[i] = tracks[minIndex];
            tracks[minIndex] = temp;
        }
    }
}

// Selection sort by track number (ascending order)
void Playlist::sortByTrackNumber() {
    for (int i = 0; i < trackCount - 1; i++) {
        int minIndex = i;
        
        for (int j = i + 1; j < trackCount; j++) {
            if (tracks[j].getTrackNumber() < tracks[minIndex].getTrackNumber()) {
                minIndex = j;
            }
        }
        
        // Swap
        if (minIndex != i) {
            AudioTrack temp = tracks[i];
            tracks[i] = tracks[minIndex];
            tracks[minIndex] = temp;
        }
    }
}
