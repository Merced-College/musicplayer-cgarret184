#include "AudioPlayer.h"
#include "Playlist.h"

// Song titles stored as flash-friendly C strings
const char* songs[] = {
    "Forest Morning",
    "River Walk",
    "City Lights",
    "Summer Breeze",
    "Night Drive",
    "Ocean Waves",
    "Rainfall",
    "Golden Hour"
};

const uint8_t NUM_SONGS = 8;

SoftwareSerial dfSerial(10, 11); // RX, TX pins for DFPlayer Mini
AudioPlayer player(&dfSerial);
Playlist playlist;

void trimCharArray(char* str) {
    int start = 0;
    int end = strlen(str) - 1;

    while (str[start] != '\0' && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
        start++;
    }

    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\r' || str[end] == '\n')) {
        end--;
    }

    int len = end - start + 1;
    if (len <= 0) {
        str[0] = '\0';
        return;
    }

    if (start > 0) {
        memmove(str, str + start, len);
    }
    str[len] = '\0';
}

void toLowerCase(char* str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str += 'a' - 'A';
        }
        str++;
    }
}

void setup() {
    Serial.begin(9600);
    delay(500);

    // Initialize audio player
    if (!player.begin()) {
        Serial.println(F("ERROR: Failed to initialize DFPlayer Mini"));
        while (1);
    }
    Serial.println(F("DFPlayer Mini initialized successfully"));

    // Create playlist with 8 songs from the songs array
    playlist.addTrack(AudioTrack(1, songs[0], 245));
    playlist.addTrack(AudioTrack(2, songs[1], 198));
    playlist.addTrack(AudioTrack(3, songs[2], 267));
    playlist.addTrack(AudioTrack(4, songs[3], 223));
    playlist.addTrack(AudioTrack(5, songs[4], 289));
    playlist.addTrack(AudioTrack(6, songs[5], 276));
    playlist.addTrack(AudioTrack(7, songs[6], 214));
    playlist.addTrack(AudioTrack(8, songs[7], 251));

    Serial.println(F("\n=== ORIGINAL PLAYLIST ==="));
    printPlaylist();

    // Demonstrate linear search by title
    Serial.println(F("\n=== LINEAR SEARCH BY TITLE ==="));
    linearSearchDemo("Forest Morning");
    linearSearchDemo("Ocean Waves");
    linearSearchDemo("Mountain Echo");

    // Demonstrate linear search by track number
    Serial.println(F("\n=== LINEAR SEARCH BY TRACK NUMBER ==="));
    trackNumberSearchDemo(5);
    trackNumberSearchDemo(8);
    trackNumberSearchDemo(10);

    // Create a copy for sorting by title
    Serial.println(F("\n=== SORTING BY TITLE (A-Z) ==="));
    sortByTitleDemo();

    // Re-create playlist for sorting by track number
    playlist = Playlist();
    playlist.addTrack(AudioTrack(1, songs[0], 245));
    playlist.addTrack(AudioTrack(2, songs[1], 198));
    playlist.addTrack(AudioTrack(3, songs[2], 267));
    playlist.addTrack(AudioTrack(4, songs[3], 223));
    playlist.addTrack(AudioTrack(5, songs[4], 289));
    playlist.addTrack(AudioTrack(6, songs[5], 276));
    playlist.addTrack(AudioTrack(7, songs[6], 214));
    playlist.addTrack(AudioTrack(8, songs[7], 251));

    Serial.println(F("\n=== SORTING BY TRACK NUMBER ==="));
    sortByTrackNumberDemo();

    Serial.println(F("\n=== SETUP COMPLETE ==="));
    delay(500);
    printMenu();
}

void loop() {
    // Main loop for Serial menu control
    if (Serial.available()) {
        char rawInput[48];
        char command[12] = "";
        char argument[32] = "";
        size_t len = Serial.readBytesUntil('\n', rawInput, sizeof(rawInput) - 1);
        rawInput[len] = '\0';
        trimCharArray(rawInput);

        char* spacePos = strchr(rawInput, ' ');
        if (spacePos != nullptr) {
            *spacePos = '\0';
            strncpy(argument, spacePos + 1, sizeof(argument) - 1);
            argument[sizeof(argument) - 1] = '\0';
            trimCharArray(argument);
        }

        strncpy(command, rawInput, sizeof(command) - 1);
        command[sizeof(command) - 1] = '\0';
        toLowerCase(command);

        // Handle commands
        if (strcmp(command, "p") == 0) {
            // Play track by number: p <number>
            if (argument[0] != '\0') {
                int trackNum = atoi(argument);
                if (trackNum > 0 && trackNum <= NUM_SONGS) {
                    if (playlist.setCurrentTrackByNumber(trackNum)) {
                        player.playTrack(trackNum);
                        Serial.print(F("Playing: "));
                        Serial.println(songs[trackNum - 1]);
                    } else {
                        Serial.println(F("ERROR: Track not found in playlist"));
                    }
                } else {
                    Serial.print(F("ERROR: Track number must be between 1 and "));
                    Serial.println(NUM_SONGS);
                }
            } else {
                Serial.println(F("ERROR: Usage: p <number>"));
            }
        }
        else if (strcmp(command, "n") == 0 || strcmp(command, "next") == 0) {
            // Next track based on playlist order
            AudioTrack* nextTrack = playlist.nextTrack();
            if (nextTrack) {
                player.playTrack(nextTrack->getTrackNumber());
            } else {
                Serial.println(F("ERROR: Playlist is empty"));
            }
        }
        else if (strcmp(command, "b") == 0 || strcmp(command, "prev") == 0 || strcmp(command, "previous") == 0) {
            // Previous track based on playlist order
            AudioTrack* previousTrack = playlist.previousTrack();
            if (previousTrack) {
                player.playTrack(previousTrack->getTrackNumber());
            } else {
                Serial.println(F("ERROR: Playlist is empty"));
            }
        }
        else if (strcmp(command, "pause") == 0) {
            // Pause playback
            player.pause();
        }
        else if (strcmp(command, "stop") == 0) {
            // Stop playback
            player.stop();
        }
        else if (strcmp(command, "v") == 0) {
            // Set volume: v <number>
            if (argument[0] != '\0') {
                int volLevel = atoi(argument);
                player.setVolume(volLevel);
            } else {
                Serial.println(F("ERROR: Usage: v <number> (0-30)"));
            }
        }
        else if (strcmp(command, "list") == 0) {
            // Show playlist
            Serial.println(F("\n=== PLAYLIST ==="));
            printPlaylist();
        }
        else if (strcmp(command, "find") == 0) {
            // Find song: find <word>
            if (argument[0] != '\0') {
                findSong(argument);
            } else {
                Serial.println(F("ERROR: Usage: find <song name>"));
            }
        }
        else if (strcmp(command, "sort") == 0) {
            // Sort playlist
            if (argument[0] != '\0') {
                toLowerCase(argument);
                if (strcmp(argument, "title") == 0 || strcmp(argument, "name") == 0) {
                    Serial.println(F("Sorting playlist by title..."));
                    playlist.sortByTitle();
                    Serial.println(F("Done!"));
                    printPlaylist();
                } else if (strcmp(argument, "track") == 0 || strcmp(argument, "number") == 0) {
                    Serial.println(F("Sorting playlist by track number..."));
                    playlist.sortByTrackNumber();
                    Serial.println(F("Done!"));
                    printPlaylist();
                } else {
                    Serial.println(F("ERROR: Usage: sort <title|track>"));
                }
            } else {
                Serial.println(F("ERROR: Usage: sort <title|track>"));
            }
        }
        else if (strcmp(command, "help") == 0) {
            // Show help menu
            printMenu();
        }
        else if (strcmp(command, "r") == 0 || strcmp(command, "resume") == 0) {
            // Resume from pause (legacy support)
            player.resume();
        }
        else if (rawInput[0] != '\0') {
            // Invalid command
            Serial.print(F("ERROR: Unknown command '"));
            Serial.print(rawInput);
            Serial.println(F("' - Type 'help' for menu"));
        }
    }
}

// Helper function to print the entire playlist
void printPlaylist() {
    Serial.println(F("┌─────────────────────────────────────┐"));
    Serial.println(F("│ #  │ Song Name              │ Length │"));
    Serial.println(F("├─────────────────────────────────────┤"));
    for (uint8_t i = 0; i < playlist.getTrackCount(); i++) {
        AudioTrack* track = playlist.getTrack(i);
        Serial.print(F("│ "));
        Serial.print(track->getTrackNumber());
        if (track->getTrackNumber() < 10) Serial.print(F(" "));
        Serial.print(F(" │ "));
        
        const char* title = track->getTitle();
        size_t titleLen = strlen(title);
        if (titleLen <= 22) {
            Serial.print(title);
            for (int j = titleLen; j < 22; j++) Serial.print(F(" "));
        } else {
            for (int k = 0; k < 19; k++) Serial.print(title[k]);
            Serial.print(F("..."));
        }
        
        Serial.print(F(" │ "));
        uint16_t dur = track->getDuration();
        if (dur < 100) Serial.print(F("0"));
        if (dur < 10) Serial.print(F("0"));
        Serial.print(dur);
        Serial.println(F("s │"));
    }
    Serial.println(F("└─────────────────────────────────────┘\n"));
}

// Demonstration of linear search by title
void linearSearchDemo(const char* title) {
    int index = playlist.linearSearchByTitle(title);
    Serial.print(F("Searching for: \""));
    Serial.print(title);
    Serial.print(F("\" -> "));
    
    if (index != -1) {
        Serial.print(F("Found at index "));
        Serial.println(index);
    } else {
        Serial.println(F("NOT FOUND"));
    }
}

// Demonstration of linear search by track number
void trackNumberSearchDemo(uint16_t trackNum) {
    int index = playlist.linearSearchByTrackNumber(trackNum);
    Serial.print(F("Searching for track #"));
    Serial.print(trackNum);
    Serial.print(F(" -> "));
    
    if (index != -1) {
        Serial.print(F("Found at index "));
        Serial.println(index);
    } else {
        Serial.println(F("NOT FOUND"));
    }
}

// Demonstration of sorting by title
void sortByTitleDemo() {
    playlist.sortByTitle();
    Serial.println(F("Playlist sorted by title:"));
    printPlaylist();
}

// Demonstration of sorting by track number
void sortByTrackNumberDemo() {
    playlist.sortByTrackNumber();
    Serial.println(F("Playlist sorted by track number:"));
    printPlaylist();
}

// Linear search to find a song by name
void findSong(const char* songName) {
    int index = playlist.linearSearchByTitle(songName);
    if (index != -1) {
        AudioTrack* track = playlist.getTrack(index);
        if (track != nullptr) {
            Serial.print(F("Song found: "));
            Serial.println(track->getTitle());
            Serial.print(F("Track number: "));
            Serial.println(track->getTrackNumber());
            return;
        }
    }

    // Song not found
    Serial.println(F("Song not found"));
}

// Print available commands menu
void printMenu() {
    Serial.println(F("\n╔════════════════════════════════════════╗"));
    Serial.println(F("║     MUSIC PLAYER - COMMAND MENU        ║"));
    Serial.println(F("╚════════════════════════════════════════╝"));
    Serial.println(F("\nPLAYBACK CONTROL:"));
    Serial.println(F("  p <number>      Play track (1-8)"));
    Serial.println(F("  n               Next track"));
    Serial.println(F("  b               Previous track"));
    Serial.println(F("  pause           Pause playback"));
    Serial.println(F("  stop            Stop playback"));
    Serial.println(F("  r               Resume from pause"));
    Serial.println(F("\nVOLUME CONTROL:"));
    Serial.println(F("  v <number>      Set volume (0-30)"));
    Serial.println(F("\nPLAYLIST MANAGEMENT:"));
    Serial.println(F("  list            Show playlist"));
    Serial.println(F("  find <word>     Find song by name"));
    Serial.println(F("  sort <type>     Sort playlist (title|track)"));
    Serial.println(F("\nOTHER:"));
    Serial.println(F("  help            Show this menu"));
    Serial.println(F("\nEXAMPLES:"));
    Serial.println(F("  p 3             → Play track 3"));
    Serial.println(F("  v 25            → Set volume to 25"));
    Serial.println(F("  find Rainfall   → Search for 'Rainfall'"));
    Serial.println(F("  sort title      → Sort by song title"));
    Serial.println(F("════════════════════════════════════════\n"));
}
