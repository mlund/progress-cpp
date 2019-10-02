Progress-CPP
===

A flexible ASCII progress bar for your console based C++ projects.

## Usage
Progress is a header-only and library and can be used by simply including the `progress_tracker.hpp` header file.
Alternatively, you can use it as a static library (`progress_tracker.h` and `libprogresstracker.a`) with a greater
set of features.

### As a header-only library
The header-only library provides two progress trackers
- `ProgressBar`: suitable for showing a progress on a screen,
- `ProgressLog`: when writing a progress either on a screen or into a file

The bar takes the following options at initialization
- limit: the total number of ticks that need to be completed
- width: width of the bar
- complete char: the character to indicate completion (defaults to `=`)
- incomplete char: the character to indicate pending. (defaults to ' ')

```c++
#include "progress_tracker.hpp"

int main() {

    const int limit = 10000;

    // initialize the bar
    ProgressIndicator::ProgressBar progress_bar(limit, 70);

    for (int i = 0; i < limit; ++i) {
        // record the tick
        ++progress_bar;

        // display the bar
        progress_bar.display();
    }

    // tell the bar to finish
    progress_bar.done();
}
```
The above code results in the following output

```
[===================>                                                 ]  29% 0.8/2.8s
```

### As a static library
You can easily build the library using `cmake` and `make`.
```
$ mkdir build && cd build
$ cmake ..
$ make library # produce libprogresstracker.a
```
The static library contains – beside the progress trackers – also decorators which alter the progress trackers'
behaviour.

```c++
#include "progress_tracker.h"
using namespace ProgressIndicator;

int main() {

    const int limit = 10000;

    // use a decorator to limit output frequency
    TaciturnDecorator progressLog(
         // print line by line
         std::make_shared<ProgressLog>(limit),
         // at most every 0.2 seconds or after 20% of progress, whatever comes first
         std::chrono::milliseconds(200), 0.2
    );
    for (int i = 0; i < limit; i++) {
        ++progressLog; // record the tick
        usleep(200); // simulate work
        // display a new line only at certain steps;
        // actually even less frequently
        if (i % 10 == 0)
            progressLog.display();
    }
    progressLog.done();
    std::cout << "Done!" << std::endl;
}
```

## Example
Refer to [main.cpp](main.cpp) file for an example usage. To run it,

```
$ mkdir build && cd build
$ cmake ..
$ make
$ ./example     # using the static library
$ ./example_hpp # from the header library
```

Or without `cmake`
```
$ g++ -O3 -I. -DUSE_HPP main.cpp -Wall -std=c++14 -o example_hpp
$ ./example_hpp
```

## Tips

You can test if the standard output in unix-like systems is console or not by this snippet of code
```c
#include <unistd.h>

if (isatty(fileno(stdout))) {
    // console! let's use ProgressBar!
} else {
   // better to use ProgressLog
}
```
## License
MIT
