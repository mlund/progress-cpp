#include "progress_tracker.h"

using std::chrono::steady_clock;
using std::chrono::system_clock;
using std::chrono::milliseconds;

// ========== ProgressTrackerImpl ==========

ProgressTrackerImpl::ProgressTrackerImpl(unsigned int total, std::ostream &ostream) :
        total_ticks(total), ostream(ostream) {}

void ProgressTrackerImpl::update() {
    now = steady_clock::now();
    speed = timeElapsed().count() / float(ticks); // update milliseconds per tick
}

void ProgressTrackerImpl::updateTimeWidth() {
    int number_width = 3; // 0.0
    for (auto t = 10; t <= (timeTotal().count() + 500) * 0.001; t *= 10) {
        ++number_width;
    }
    if (number_width > time_width) {
        time_width = number_width;
    }
}

milliseconds ProgressTrackerImpl::timeElapsed() const {
    return std::chrono::duration_cast<milliseconds>(now - start_time);
}

milliseconds ProgressTrackerImpl::timeLeft() const {
    return milliseconds((int) (speed * (total_ticks - ticks)));
}

milliseconds ProgressTrackerImpl::timeTotal() const {
    return milliseconds((int) (speed * total_ticks));
}

void ProgressTrackerImpl::done() { display(); }

// ========== ProgressBar ==========

ProgressBar::ProgressBar(unsigned int total, std::ostream &ostream,
                         unsigned int bar_width, char complete, char incomplete) :
        ProgressTrackerImpl(total, ostream),
        bar_width(bar_width), complete_char(complete), incomplete_char(incomplete) {}

ProgressBar::ProgressBar(unsigned int total, unsigned int bar_width, char complete, char incomplete) :
        ProgressBar(total, std::cout, bar_width, complete, incomplete) {}

void ProgressBar::display() {
    update();
    auto millisec_elapsed = timeElapsed().count();
    auto millisec_total = timeTotal().count();
    int pos = (int) (bar_width * progress());

    ostream << "[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos)
            ostream << complete_char;
        else if (i == pos)
            ostream << ">";
        else
            ostream << incomplete_char;
    }
    ostream << "]";
    ostream << std::fixed;
    ostream << " " << std::setw(3) << std::setprecision(0) << 100 * progress() << "%";
    updateTimeWidth();
    ostream << std::setprecision(1)
            << " " << std::setw(time_width) << float(millisec_elapsed) / 1000.0
            << "/" << std::setw(time_width) << float(millisec_total) / 1000.0 << "s\r";
    ostream.flush();
}

void ProgressBar::done() {
    display();
    ostream << std::endl;
}

// ========== ProgressLog ==========

ProgressLog::ProgressLog(unsigned int total, std::ostream &ostream) : ProgressTrackerImpl(total, ostream) {}

void ProgressLog::display() {
    update();
    auto millisec_elapsed = timeElapsed().count();
    auto millisec_total = timeTotal().count();
    auto system_now = system_clock::now();
    // auto time_now = system_clock::to_time_t(system_now);
    auto time_end = system_clock::to_time_t(system_now + (start_time + timeTotal() - now));

    ostream << std::fixed << std::setw(3) << std::setprecision(0) << 100 * progress() << "%";
    updateTimeWidth();
    ostream << std::setprecision(1)
            << " " << std::setw(time_width) << float(millisec_elapsed) / 1000.0
            << "/" << std::setw(time_width) << float(millisec_total) / 1000.0 << "s";
    // ostream << " [" << std::put_time(std::localtime(&time_now), "%F %T %z") << "]";
    ostream << " ETA [" << std::put_time(std::localtime(&time_end), "%F %T %z") << "]" << std::endl;
    ostream.flush();
}
