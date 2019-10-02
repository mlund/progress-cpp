#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <memory>

namespace ProgressIndicator {
using std::chrono::milliseconds;

class ProgressTrackerImpl {
    unsigned int ticks = 0;
    const unsigned int total_ticks;
    float speed = 0.0; //!< milliseconds per tick

  protected:
    std::ostream &ostream; //!< output stream
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now(); //!< frozen now to have the same time upon chained calls
    unsigned int time_width = 3; //!< width of a formatted string with time,  e.g., 3 → 1.5, 5 → 630.0

  public:
    ProgressTrackerImpl(unsigned int total, std::ostream &ostream = std::cout) : total_ticks(total),
                                                                                 ostream(ostream) {};

    auto inline operator++() -> decltype(ticks) { return ++ticks; };

    float inline progress() const { return float(ticks) / float(total_ticks); };

    virtual void display() = 0;

    virtual void done() { display(); };

  protected:
    /**
     * Update time-related state. It shall be called before at the beginning of time queries.
     */
    void update() {
        now = std::chrono::steady_clock::now();
        speed = timeElapsed().count() / float(ticks); // update milliseconds per tick
    }

    /**
     * Update time width to prevent unwanted jumping on the screen when the number of digits changes.
     * The format width never decreases. It increases only if the number of digits of estimated
     * total time increases·
     */
    void updateTimeWidth() {
        int number_width = 3; // 0.0
        for (auto t = 10; t <= (timeTotal().count() + 500) * 0.001; t *= 10) {
            ++number_width;
        }
        if (number_width > time_width) {
            time_width = number_width;
        }
    };

    //! time elapsed since the beginning; @see update()
    milliseconds timeElapsed() const { return std::chrono::duration_cast<milliseconds>(now - start_time); };

    //! estimated time left to finish; @see update()
    milliseconds timeLeft() const { return milliseconds((int) (speed * (total_ticks - ticks))); };

    //! estimated total runtime; @see update()
    milliseconds timeTotal() const { return milliseconds((int) (speed * total_ticks)); };
};

/**
 * Show progress in a form of growing bar with time information.
 * Suitable for display on screen only as it redraws a single line of repeatedly.
 */
class ProgressBar : public ProgressTrackerImpl {
    const unsigned int bar_width;
    const char complete_char;
    const char incomplete_char;

  public:
    ProgressBar(unsigned int total, std::ostream &ostream = std::cout,
                unsigned int bar_width = 60, char complete = '=', char incomplete = ' ') :
            ProgressTrackerImpl(total, ostream), bar_width(bar_width), complete_char(complete),
            incomplete_char(incomplete) {}

    // compatible constructor signature
    ProgressBar(unsigned int total, unsigned int bar_width, char complete = '=', char incomplete = ' ') :
            ProgressBar(total, std::cout, bar_width, complete, incomplete) {};

    void display() override {
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

    void done() override {
        display();
        ostream << std::endl;
    }
};

/**
 * Show progress by printing out a new line with progress and time information.
 * Suitable for both screen and file.
 */
class ProgressLog : public ProgressTrackerImpl {
  public:
    ProgressLog(unsigned int total, std::ostream &ostream = std::cout) : ProgressTrackerImpl(total, ostream) {};

    void display() override {
        using std::chrono::steady_clock;
        using std::chrono::system_clock;
        update();
        auto millisec_elapsed = timeElapsed().count();
        auto millisec_total = timeTotal().count();
        auto system_now = system_clock::now();
        // auto time_now = system_clock::to_time_t(system_now);
        auto time_end = system_clock::to_time_t(
                system_now + std::chrono::duration_cast<system_clock::duration>(start_time + timeTotal() - now));

        ostream << std::fixed << std::setw(3) << std::setprecision(0) << 100 * progress() << "%";
        updateTimeWidth();
        ostream << std::setprecision(1)
                << " " << std::setw(time_width) << float(millisec_elapsed) / 1000.0
                << "/" << std::setw(time_width) << float(millisec_total) / 1000.0 << "s";
        // ostream << " [" << std::put_time(std::localtime(&time_now), "%F %T %z") << "]";
        ostream << " ETA [" << std::put_time(std::localtime(&time_end), "%F %T %z") << "]" << std::endl;
        ostream.flush();
    }
};
} // end namespace ProgressIndicator
