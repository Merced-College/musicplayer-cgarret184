#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(SoftwareSerial* serial) {
    dfSerial = serial;
    volume = 20;
    currentState = PlayerState::STOPPED;
    currentTrackNumber = -1;
}

bool AudioPlayer::begin() {
    dfSerial->begin(9600);

    if (!dfPlayer.begin(*dfSerial)) {
        currentState = PlayerState::ERROR;
        printStateChange(currentState);
        return false;
    }

    dfPlayer.volume(volume);
    currentState = PlayerState::STOPPED;
    printStateChange(currentState);
    return true;
}

// Helper function to print state changes
void AudioPlayer::printStateChange(PlayerState newState) {
    Serial.print("Player State: ");
    switch (newState) {
        case PlayerState::STOPPED:
            Serial.println("STOPPED");
            break;
        case PlayerState::PLAYING:
            Serial.println("PLAYING");
            break;
        case PlayerState::PAUSED:
            Serial.println("PAUSED");
            break;
        case PlayerState::ERROR:
            Serial.println("ERROR");
            break;
    }
}

// Play a track by track number
void AudioPlayer::playTrack(int trackNumber) {
    // Validate track number
    if (trackNumber < 1 || trackNumber > 99) {
        currentState = PlayerState::ERROR;
        Serial.println("ERROR: Invalid track number");
        printStateChange(currentState);
        return;
    }

    currentTrackNumber = trackNumber;
    dfPlayer.play(trackNumber);
    currentState = PlayerState::PLAYING;
    Serial.print("▶ Now playing track ");
    Serial.println(trackNumber);
    printStateChange(currentState);
}

// Pause the current track
void AudioPlayer::pause() {
    if (currentState == PlayerState::STOPPED) {
        Serial.println("ERROR: Cannot pause when player is STOPPED");
        return;
    }

    if (currentState != PlayerState::PLAYING) {
        Serial.println("ERROR: Player is not currently playing");
        return;
    }

    dfPlayer.pause();
    currentState = PlayerState::PAUSED;
    Serial.println("⏸ Paused");
    printStateChange(currentState);
}

// Stop playing
void AudioPlayer::stop() {
    dfPlayer.stop();
    currentState = PlayerState::STOPPED;
    currentTrackNumber = -1;
    Serial.println("⏹ Stopped");
    printStateChange(currentState);
}

// Resume playing from pause
void AudioPlayer::resume() {
    if (currentState != PlayerState::PAUSED) {
        Serial.println("ERROR: Nothing to resume");
        return;
    }

    dfPlayer.start();
    currentState = PlayerState::PLAYING;
    Serial.println("▶ Resumed");
    printStateChange(currentState);
}

// Go to next track
void AudioPlayer::nextTrack() {
    if (currentTrackNumber < 0) {
        Serial.println("ERROR: No track currently selected. Use playTrack() first");
        return;
    }

    int nextTrack = currentTrackNumber + 1;
    if (nextTrack > 99) {
        nextTrack = 1; // Wrap around to first track
    }

    playTrack(nextTrack);
}

// Go to previous track
void AudioPlayer::previousTrack() {
    if (currentTrackNumber < 0) {
        Serial.println("ERROR: No track currently selected. Use playTrack() first");
        return;
    }

    int prevTrack = currentTrackNumber - 1;
    if (prevTrack < 1) {
        prevTrack = 99; // Wrap around to last track
    }

    playTrack(prevTrack);
}

// Set volume with validation (0-30)
void AudioPlayer::setVolume(int level) {
    if (level < 0 || level > 30) {
        currentState = PlayerState::ERROR;
        Serial.print("ERROR: Volume must be between 0 and 30, got ");
        Serial.println(level);
        printStateChange(currentState);
        return;
    }

    volume = level;
    dfPlayer.volume(volume);
    Serial.print("Volume set to: ");
    Serial.println(volume);
}

uint8_t AudioPlayer::getVolume() {
    return volume;
}

PlayerState AudioPlayer::getState() {
    return currentState;
}
