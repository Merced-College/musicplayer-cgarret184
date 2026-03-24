#include "AudioTrack.h"

// Default constructor: Creates an empty audio track with default values
AudioTrack::AudioTrack() {
    trackNumber = 0;
    title = "";
    duration = 0;
}

// Constructor with parameters: Creates a track with given number, title, and duration
AudioTrack::AudioTrack(uint16_t number, String name, uint16_t durationSec) {
    trackNumber = number;
    title = name;
    duration = durationSec;
}

// Returns the track's number
uint16_t AudioTrack::getTrackNumber() {
    return trackNumber;
}

// Returns the track's title
String AudioTrack::getTitle() {
    return title;
}

// Returns the track's duration in seconds
uint16_t AudioTrack::getDuration() {
    return duration;
}
