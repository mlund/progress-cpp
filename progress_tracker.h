#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <memory>

using std::chrono::milliseconds;

/**
 * A generic abstract implementation of a ProgressTracker to be derived from.
 * Implement display and optionally done method in the derived class.
 */
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
    ProgressTrackerImpl(unsigned int total, std::ostream &ostream = std::cout);

    unsigned int inline operator++() { return ++ticks; };

    float inline progress() const { return float(ticks) / float(total_ticks); };

    virtual void display() = 0;

    virtual void done();

  protected:
    /**
     * Update time-related state. It shall be called before at the beginning of time queries.
     */
    void update();

    /**
     * Update width of time representation to prevent unwanted jumping on the screen when the number of digits changes.
     * The width never decreases. It increases only if the number of digits of estimated total time increases·
     */
    void updateTimeWidth();

    milliseconds timeElapsed() const; //!< time elapsed since the beginning; @see update()

    milliseconds timeLeft() const;    //!< estimated time left to finish; @see update()

    milliseconds timeTotal() const;   //!< estimated total runtime; @see update()
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
                unsigned int bar_width = 60, char complete = '=', char incomplete = ' ');

    // compatible constructor signature
    ProgressBar(unsigned int total, unsigned int bar_width, char complete = '=', char incomplete = ' ');

    void display() override;

    void done() override;
};

/**
 * Show progress by printing out a new line with progress and time information.
 * Suitable for both screen and file.
 */
class ProgressLog : public ProgressTrackerImpl {
  public:
    ProgressLog(unsigned int total, std::ostream &ostream = std::cout);

    void display() override;
};
