#include "AudioPlayer.h"
#include "Playlist.h"

// String array to store song names
String songs[] = {
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

void setup() {
    Serial.begin(9600);
    delay(500);

    // Initialize audio player
    if (!player.begin()) {
        Serial.println("ERROR: Failed to initialize DFPlayer Mini");
        while (1);
    }
    Serial.println("DFPlayer Mini initialized successfully");

    // Create playlist with 8 songs from the songs array
    playlist.addTrack(AudioTrack(1, songs[0], 245));
    playlist.addTrack(AudioTrack(2, songs[1], 198));
    playlist.addTrack(AudioTrack(3, songs[2], 267));
    playlist.addTrack(AudioTrack(4, songs[3], 223));
    playlist.addTrack(AudioTrack(5, songs[4], 289));
    playlist.addTrack(AudioTrack(6, songs[5], 276));
    playlist.addTrack(AudioTrack(7, songs[6], 214));
    playlist.addTrack(AudioTrack(8, songs[7], 251));

    Serial.println("\n=== ORIGINAL PLAYLIST ===");
    printPlaylist();

    // Demonstrate linear search by title
    Serial.println("\n=== LINEAR SEARCH BY TITLE ===");
    linearSearchDemo("Forest Morning");
    linearSearchDemo("Ocean Waves");
    linearSearchDemo("Mountain Echo");

    // Demonstrate linear search by track number
    Serial.println("\n=== LINEAR SEARCH BY TRACK NUMBER ===");
    trackNumberSearchDemo(5);
    trackNumberSearchDemo(8);
    trackNumberSearchDemo(10);

    // Create a copy for sorting by title
    Serial.println("\n=== SORTING BY TITLE (A-Z) ===");
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

    Serial.println("\n=== SORTING BY TRACK NUMBER ===");
    sortByTrackNumberDemo();

    Serial.println("\n=== SETUP COMPLETE ===");
    delay(500);
    printMenu();
}

void loop() {
    // Main loop for Serial menu control
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        input.toLowerCase();
        
        // Parse command
        int spaceIndex = input.indexOf(' ');
        String command = (spaceIndex > 0) ? input.substring(0, spaceIndex) : input;
        String argument = (spaceIndex > 0) ? input.substring(spaceIndex + 1) : "";
        
        // Handle commands
        if (command == "p") {
            // Play track by number: p <number>
            if (argument.length() > 0) {
                int trackNum = argument.toInt();
                if (trackNum > 0 && trackNum <= 8) {
                    player.playTrack(trackNum);
                    Serial.print("Playing: ");
                    Serial.println(songs[trackNum - 1]);
                } else {
                    Serial.println("ERROR: Track number must be between 1 and 8");
                }
            } else {
                Serial.println("ERROR: Usage: p <number>");
            }
        }
        else if (command == "n" || command == "next") {
            // Next track
            player.nextTrack();
        }
        else if (command == "b" || command == "prev" || command == "previous") {
            // Previous track
            player.previousTrack();
        }
        else if (command == "pause") {
            // Pause playback
            player.pause();
        }
        else if (command == "stop") {
            // Stop playback
            player.stop();
        }
        else if (command == "v") {
            // Set volume: v <number>
            if (argument.length() > 0) {
                int volLevel = argument.toInt();
                player.setVolume(volLevel);
            } else {
                Serial.println("ERROR: Usage: v <number> (0-30)");
            }
        }
        else if (command == "list") {
            // Show playlist
            Serial.println("\n=== PLAYLIST ===");
            printPlaylist();
        }
        else if (command == "find") {
            // Find song: find <word>
            if (argument.length() > 0) {
                findSong(argument);
            } else {
                Serial.println("ERROR: Usage: find <song name>");
            }
        }
        else if (command == "sort") {
            // Sort playlist
            if (argument.length() > 0) {
                if (argument == "title" || argument == "name") {
                    Serial.println("Sorting playlist by title...");
                    playlist.sortByTitle();
                    Serial.println("Done!");
                    printPlaylist();
                } else if (argument == "track" || argument == "number") {
                    Serial.println("Sorting playlist by track number...");
                    playlist.sortByTrackNumber();
                    Serial.println("Done!");
                    printPlaylist();
                } else {
                    Serial.println("ERROR: Usage: sort <title|track>");
                }
            } else {
                Serial.println("ERROR: Usage: sort <title|track>");
            }
        }
        else if (command == "help") {
            // Show help menu
            printMenu();
        }
        else if (command == "r" || command == "resume") {
            // Resume from pause (legacy support)
            player.resume();
        }
        else if (input.length() > 0) {
            // Invalid command
            Serial.print("ERROR: Unknown command '");
            Serial.print(input);
            Serial.println("' - Type 'help' for menu");
        }
    }
}

// Helper function to print the entire playlist
void printPlaylist() {
    Serial.println("┌─────────────────────────────────────┐");
    Serial.println("│ #  │ Song Name              │ Length │");
    Serial.println("├─────────────────────────────────────┤");
    for (uint8_t i = 0; i < playlist.getTrackCount(); i++) {
        AudioTrack* track = playlist.getTrack(i);
        Serial.print("│ ");
        Serial.print(track->getTrackNumber());
        if (track->getTrackNumber() < 10) Serial.print(" ");
        Serial.print(" │ ");
        
        String title = track->getTitle();
        if (title.length() <= 22) {
            Serial.print(title);
            for (int j = title.length(); j < 22; j++) Serial.print(" ");
        } else {
            for (int k = 0; k < 19; k++) Serial.print(title[k]);
            Serial.print("...");
        }
        
        Serial.print(" │ ");
        uint16_t dur = track->getDuration();
        if (dur < 100) Serial.print("0");
        if (dur < 10) Serial.print("0");
        Serial.print(dur);
        Serial.println("s │");
    }
    Serial.println("└─────────────────────────────────────┘\n");
}

// Demonstration of linear search by title
void linearSearchDemo(String title) {
    int index = playlist.linearSearchByTitle(title);
    Serial.print("Searching for: \"");
    Serial.print(title);
    Serial.print("\" -> ");
    
    if (index != -1) {
        Serial.print("Found at index ");
        Serial.println(index);
    } else {
        Serial.println("NOT FOUND");
    }
}

// Demonstration of linear search by track number
void trackNumberSearchDemo(uint16_t trackNum) {
    int index = playlist.linearSearchByTrackNumber(trackNum);
    Serial.print("Searching for track #");
    Serial.print(trackNum);
    Serial.print(" -> ");
    
    if (index != -1) {
        Serial.print("Found at index ");
        Serial.println(index);
    } else {
        Serial.println("NOT FOUND");
    }
}

// Demonstration of sorting by title
void sortByTitleDemo() {
    playlist.sortByTitle();
    Serial.println("Playlist sorted by title:");
    printPlaylist();
}

// Demonstration of sorting by track number
void sortByTrackNumberDemo() {
    playlist.sortByTrackNumber();
    Serial.println("Playlist sorted by track number:");
    printPlaylist();
}

// Linear search to find a song by name
void findSong(String songName) {
    // Linear search through songs array
    for (int i = 0; i < NUM_SONGS; i++) {
        if (songs[i] == songName) {
            Serial.print("Song found: ");
            Serial.println(songs[i]);
            Serial.print("Track number: ");
            Serial.println(i + 1);
            return;
        }
    }
    
    // Song not found
    Serial.println("Song not found");
}

// Print available commands menu
void printMenu() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║     MUSIC PLAYER - COMMAND MENU        ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\nPLAYBACK CONTROL:");
    Serial.println("  p <number>      Play track (1-8)");
    Serial.println("  n               Next track");
    Serial.println("  b               Previous track");
    Serial.println("  pause           Pause playback");
    Serial.println("  stop            Stop playback");
    Serial.println("  r               Resume from pause");
    Serial.println("\nVOLUME CONTROL:");
    Serial.println("  v <number>      Set volume (0-30)");
    Serial.println("\nPLAYLIST MANAGEMENT:");
    Serial.println("  list            Show playlist");
    Serial.println("  find <word>     Find song by name");
    Serial.println("  sort <type>     Sort playlist (title|track)");
    Serial.println("\nOTHER:");
    Serial.println("  help            Show this menu");
    Serial.println("\nEXAMPLES:");
    Serial.println("  p 3             → Play track 3");
    Serial.println("  v 25            → Set volume to 25");
    Serial.println("  find Rainfall   → Search for 'Rainfall'");
    Serial.println("  sort title      → Sort by song title");
    Serial.println("════════════════════════════════════════\n");
}
