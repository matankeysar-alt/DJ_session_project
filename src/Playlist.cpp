#include "Playlist.h"
#include "AudioTrack.h"
#include <iostream>
#include <algorithm>
Playlist::Playlist(const std::string& name) 
    : head(nullptr), playlist_name(name), track_count(0) {
    std::cout << "Created playlist: " << name << std::endl;
}
// TODO: Fix memory leaks!
// Students must fix this in Phase 1
Playlist::~Playlist() {
    #ifdef DEBUG
    std::cout << "Destroying playlist: " << playlist_name << std::endl;
    #endif

    // preventing memory leaks by deleting all the PlaylistNodes from the heap
    PlaylistNode* curr = head;
    while (curr) {
        PlaylistNode* next = curr -> next;
        delete curr;
        curr = next;
    }
}

// rule of 5: implimentation

PlaylistNode::~PlaylistNode() { 
    if (track) {
        delete track;
        track = nullptr;
    }
   
}

Playlist::Playlist(const Playlist& other) 
    : head(nullptr), playlist_name(other.playlist_name), track_count(0) {

    if (!other.head) { // playlist is empty
        return;
    }

    PlaylistNode* current_other = other.head;
    PlaylistNode* last_new = nullptr;

    while (current_other) { // loop for copying every node
        AudioTrack* new_track_clone = current_other->track->clone().release(); // clone the track in the node 
        PlaylistNode* new_node = new PlaylistNode(new_track_clone); // make a new node using the cloned track

        if (last_new == nullptr) { // insert the first node of the new list
            head = new_node;
        }
        
        else {
            last_new->next = new_node; // insert the next node (untill Playlist is fully copied)
        }
        
        last_new = new_node;
        current_other = current_other->next;
        track_count++;
    }
}

Playlist& Playlist::operator=(const Playlist& other) {
    if (this == &other) {
        return *this;
    }
    
    //distruction of all nodes
    PlaylistNode* current = head;
    while (current) {
        PlaylistNode* next = current->next;
        delete current; 
        current = next;
    }

    head = nullptr;
    track_count = 0;

    playlist_name = other.playlist_name;
    
    if (!other.head) {
        return *this;
    }

    // deep copying
    PlaylistNode* current_other = other.head;
    PlaylistNode* last_new = nullptr;

    while (current_other) {
        AudioTrack* new_track_clone = current_other->track->clone().release(); 
        PlaylistNode* new_node = new PlaylistNode(new_track_clone); 

        if (last_new == nullptr) {
            head = new_node;  
        } 
        
        else {
            last_new->next = new_node;  
        }
        
        last_new = new_node;
        current_other = current_other->next;
        track_count++;
    }

    return *this;
}

Playlist::Playlist(Playlist&& other) noexcept
    : head(other.head), 
      playlist_name(std::move(other.playlist_name)),
      track_count(other.track_count) {
    other.head = nullptr;
    other.track_count = 0;
}

Playlist& Playlist::operator=(Playlist&& other) noexcept {
    if (this != &other) {
        PlaylistNode* curr = head;
        while (curr) {
            PlaylistNode* next = curr->next;
            delete curr;
            curr = next;
        }

        head = other.head;
        playlist_name = std::move(other.playlist_name);
        track_count = other.track_count;

        other.head = nullptr;
        other.track_count = 0;
    }

    return *this;
}

void Playlist::add_track(AudioTrack* track) {
    if (!track) {
        std::cout << "[Error] Cannot add null track to playlist" << std::endl;
        return;
    }

    // Create new node - this allocates memory!
    PlaylistNode* new_node = new PlaylistNode(track);

    // Add to front of list
    new_node->next = head;
    head = new_node;
    track_count++;

    std::cout << "Added '" << track->get_title() << "' to playlist '" 
              << playlist_name << "'" << std::endl;
}

void Playlist::remove_track(const std::string& title) {
    PlaylistNode* current = head;
    PlaylistNode* prev = nullptr;


    // Find the track to remove
    while (current && current->track->get_title() != title) {
        prev = current;
        current = current->next;
    }

    if (current) {
        // Remove from linked list
        if (prev) {
            prev->next = current->next;
        } else {
            head = current->next;
        }

        // preventing memory leaks by deleting the node from the heap
        delete current;

        track_count--;
        std::cout << "Removed '" << title << "' from playlist" << std::endl;

    } else {
        std::cout << "Track '" << title << "' not found in playlist" << std::endl;
    }
}

void Playlist::display() const {
    std::cout << "\n=== Playlist: " << playlist_name << " ===" << std::endl;
    std::cout << "Track count: " << track_count << std::endl;

    PlaylistNode* current = head;
    int index = 1;

    while (current) {
        std::vector<std::string> artists = current->track->get_artists();
        std::string artist_list;

        std::for_each(artists.begin(), artists.end(), [&](const std::string& artist) {
            if (!artist_list.empty()) {
                artist_list += ", ";
            }
            artist_list += artist;
        });

        AudioTrack* track = current->track;
        std::cout << index << ". " << track->get_title() 
                  << " by " << artist_list
                  << " (" << track->get_duration() << "s, " 
                  << track->get_bpm() << " BPM)" << std::endl;
        current = current->next;
        index++;
    }

    if (track_count == 0) {
        std::cout << "(Empty playlist)" << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

AudioTrack* Playlist::find_track(const std::string& title) const {
    PlaylistNode* current = head;

    while (current) {
        if (current->track->get_title() == title) {
            return current->track;
        }
        current = current->next;
    }

    return nullptr;
}

int Playlist::get_total_duration() const {
    int total = 0;
    PlaylistNode* current = head;

    while (current) {
        total += current->track->get_duration();
        current = current->next;
    }

    return total;
}

std::vector<AudioTrack*> Playlist::getTracks() const {
    std::vector<AudioTrack*> tracks;
    PlaylistNode* current = head;
    while (current) {
        if (current->track)
            tracks.push_back(current->track);
        current = current->next;
    }
    return tracks;
}

// This tells std::sort how to compare two Playlist objects
bool Playlist::operator<(const Playlist& other) const {
    // Compares based on the alphabetical order of the playlist_name string
    return this->playlist_name < other.playlist_name; 
}
