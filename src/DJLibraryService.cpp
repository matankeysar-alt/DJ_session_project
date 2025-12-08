#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    for (const auto& track_data : library_tracks) {
        AudioTrack* new_track = nullptr;
        const std::string& format = track_data.type; 

        if (format == "MP3") { // create the track from the data in config
            new_track = new MP3Track(
                track_data.title, 
                track_data.artists, 
                track_data.duration_seconds, 
                track_data.bpm, 
                track_data.extra_param1,  
                track_data.extra_param2  
            );
        } else if (format == "WAV") { // create the track from the data in config
            new_track = new WAVTrack(
                track_data.title, 
                track_data.artists, 
                track_data.duration_seconds, 
                track_data.bpm, 
                track_data.extra_param1,  
                track_data.extra_param2   
            );
        }

        library.push_back(new_track); // insert the track
    
//        if (format == "MP3") {
//            std::cout << track_data.extra_param1 << "kpbs" << std::endl;
//        }

//        if (format == "WAV") {
//           std::cout << "WAVTrack created: " << track_data.extra_param1
//            << "Hz/" << track_data.extra_param2 << "bit" << std::endl;
//        }
        
        
    }

    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}

DJLibraryService::~DJLibraryService() { // all tracks must be deleted, as well as the vector itself
    
    for (AudioTrack* track : library) {
        delete track;
    }
    
    library.clear(); 
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    return(playlist.find_track(track_title)); // using the "find_track" function of Playlist
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;

    this->playlist = Playlist(playlist_name);
    
    for (int index : track_indices) {
        if (index > 0 && (size_t) index <= library.size()) {
            AudioTrack* track = library[index - 1];
            
            // 1. Create the wrapper (Owns the new track)
            PointerWrapper<AudioTrack> clone = track->clone();

            if (!clone) {
                std::cout << "[ERROR]:"  << playlist_name << " failed to clone" << std::endl;
                continue;
            }

            clone.get()->load(); 
            clone.get()->analyze_beatgrid(); 
                    
            playlist.add_track(clone.release());  
        } 
        else {
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
        }
    }

    std::cout << "[INFO] Playlist loaded: " << playlist_name
     << " (" << playlist.get_track_count() << " tracks)" << std::endl;
}

/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    std::vector<std::string> titles{};
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    for (AudioTrack* track : tracks) {
        if (track) {
        titles.push_back(track -> get_title());
        }
    }
    return titles; 
}
