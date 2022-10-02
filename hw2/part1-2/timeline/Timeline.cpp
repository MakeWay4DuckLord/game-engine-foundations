#include "Timeline.h"

unsigned int getMiliSinceEpoch() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000;
}

/**
 * Timeline constructor, sets fields as given, sets beginning to miliseconds since epoch based on chrono::high_resolution_clock
 * @param basis pointer to a Timline, if NULL is passed the Timeline will base itself on std::chrono
 * @param tic integer for tic size, i.e. number of tics of basis to make up one tic of this one
 */
Timeline::Timeline(Timeline *basis, int tic) {
    this->paused = false;
    this->tic = tic;
    this->basis = basis;
    if(basis == NULL) {
        this->beginning = getMiliSinceEpoch();
    } else {
        this->beginning = basis->getTime();
    }
}


unsigned int Timeline::getTime() {
    if(this->isPaused()) {
        return this->pauseTime;
    }
    unsigned int time;
    if(basis == NULL) {
        time = getMiliSinceEpoch() - beginning;
    } else {
        time = basis->getTime() - beginning;
    }
    time /= tic;
    return time;
}

bool Timeline::isPaused() {
    return this->paused;
}

void Timeline::pause() {
    if(isPaused()) {
        paused = false;
        beginning += getTime() - pauseTime;
    } else {
        paused = true;
        this->pauseTime = getTime();
    }
}

