#include <iostream>
#include <unistd.h>

#ifdef USE_HPP
#include "progress_tracker.hpp"
#else
#include "progress_tracker.h"
#endif

/* Example usage of ProgressBar */
int main() {
    const int total = 10000;

    /*
     * Define a progress bar that has a total of 100,
     * a width of 70, shows `=` to indicate completion
     * and a blank space for incomplete
     */
    ProgressBar progressBar(total, 70, '#', '-');
    for (int i = 0; i < total; i++) {
        ++progressBar; // record the tick
        usleep(200); // simulate work
        // display the bar only at certain steps
        if (i % 100 == 0)
            progressBar.display();
    }
    // tell the bar to finish
    progressBar.done();
    std::cout << "Done!" << std::endl;

    /*
     * Define a progress log.
     */
    ProgressLog progressLog(total);
    for (int i = 0; i < total; i++) {
        ++progressLog; // record the tick
        usleep(200); // simulate work
        // display the bar only at certain steps
        if (i % 1000 == 0)
            progressLog.display();
    }
    progressLog.done();
    std::cout << "Done!" << std::endl;

    return 0;
}