#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <Arduino.h>
#include "AudioTrack.h"

#define MAX_TRACKS 50

// This class manages a list of audio tracks for a music player.
// It can add tracks, navigate through them, and sort or search the list.
class Playlist {
private:
    // Array to store up to MAX_TRACKS audio tracks
    AudioTrack tracks[MAX_TRACKS];
    // Number of tracks currently in the playlist
    uint8_t trackCount;
    // Index of the currently selected track (for playback)
    uint8_t currentIndex;

public:
    // Creates a new empty playlist
    Playlist();

    // Adds a track to the playlist if there's space
    bool addTrack(AudioTrack track);
    // Gets the track that's currently selected
    AudioTrack* getCurrentTrack();
    // Selects a track by its track number and makes it current
    bool setCurrentTrackByNumber(uint16_t trackNumber);

    // Moves to the next track in the list
    AudioTrack* nextTrack();
    // Moves to the previous track in the list
    AudioTrack* previousTrack();

    // Returns how many tracks are in the playlist
    uint8_t getTrackCount();
    // Resets the current track to the first one
    void reset();
    
    // Getter for individual track at index
    AudioTrack* getTrack(uint8_t index);

    // Linear search
    int linearSearchByTitle(String title);
    int linearSearchByTrackNumber(uint16_t trackNumber);

    // Selection sort
    void sortByTitle();
    void sortByTrackNumber();
};

#endif
