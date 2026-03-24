#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "AudioTrack.h"

// Player state enum
enum class PlayerState { STOPPED, PLAYING, PAUSED, ERROR };

class AudioPlayer {
private:
    SoftwareSerial* dfSerial;
    DFRobotDFPlayerMini dfPlayer;
    uint8_t volume;
    PlayerState currentState;
    int currentTrackNumber;
    
    // Helper to print state changes
    void printStateChange(PlayerState newState);

public:
    AudioPlayer(SoftwareSerial* serial);

    bool begin();

    // Control functions
    void playTrack(int trackNumber);
    void pause();
    void stop();
    void nextTrack();
    void previousTrack();
    void setVolume(int level);
    
    // Legacy function (kept for compatibility)
    void resume();

    uint8_t getVolume();
    PlayerState getState();
};

#endif
