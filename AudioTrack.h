#ifndef AUDIO_TRACK_H
#define AUDIO_TRACK_H

#include <Arduino.h>

// This class represents a single audio track with a number, title, and duration.
class AudioTrack {
private:
    // The track's number (like track 1, 2, etc.)
    uint16_t trackNumber;
    // The track's title or name
    String title;
    // How long the track is in seconds
    uint16_t duration;

public:
    // Creates an empty track
    AudioTrack();
    // Creates a track with given number, title, and duration
    AudioTrack(uint16_t number, String name, uint16_t durationSec);

    // Gets the track number
    uint16_t getTrackNumber();
    // Gets the track title
    String getTitle();
    // Gets the track duration
    uint16_t getDuration();
};

#endif
