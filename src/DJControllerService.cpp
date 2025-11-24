#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {

    std::string title = track.get_title();

    if (cache.contains(title)) {
        cache.get(title);
        return 1;
    }

    PointerWrapper<AudioTrack> clone = track.clone(); // create a clone
    if (!clone) {
        std::cout << "[ERROR] failed to clone" << std::endl;
        return 0;
    }

    AudioTrack* ptr = clone.release(); // the clone is a wrapper, while the functions should get an AudioTrack
    ptr -> load();
    ptr -> analyze_beatgrid();

    PointerWrapper<AudioTrack> wrapper(ptr); // wrapping the track after the changes
    bool avicted = cache.put(std::move(wrapper));
    if (avicted) {return -1;} // with aviction
    return 0; // without aviction
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    return cache.get(track_title); 
}
