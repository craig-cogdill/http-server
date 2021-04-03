# http-server

## Requirements:
- This code is compatible with Linux x86_64 architechture
    - It has only been tested with Ubuntu 19.10 (my personal machine)
- CMake >= 3.0 (latest Stable release)
    - https://cmake.org/download/
    - It has only been tested with CMake v3.16.5 for Linux x86_64
- g++ with C++11 support
    - Tested with g++ v9.2.1 for Ubuntu
- Active internet connection at build time
    - This is necessary to download googletest v1.7.0. It will not be
      installed system-wide, only locally.

## Building and Running:
cd http-server
cmake .
make

This will generate two executable artifacts: ./http-server and ./unit-test-runner

### Running the Unit Test Suite:
./unit-test-runner

### Running the http-server program:
./http-server

The http-server is listening for connections on port 8000, and will not print to the terminal.
The http-server can be halted with a SIGTERM or a SIGINT signal.

## Design Decisions:
### Building / Running
- Building the project should be simple and heavily repeatable
    - I used CMake for this part of the project, so it could also be portable (across Linux distributions, anyway)
    - The `CMakeFiles.txt.in` pattern, and a section of the `CMakeList.txt` file have been taken directly from the
      googletest documentation, which is open source. I do not claim them as my own, and I have noted such in the
      files themselves.
- Use only Linux built-ins and the C++ STL for production code
    - I wanted to use as few dependencies as possible, so that the project was easy to navigate, build, and run.
      The only dependency is googletest (https://github.com/google/googletest), a C++ Unit Testing framework.

### Code Design Choices
- A priority on ease-of-use for future applications
    - The HttpServer has a minimal public interface to make its usage straightforward. I debated having the `while(continueRunning)`
      loop of the `main` class essentially be what is inside the `HttpServer::ProcessRequest` function. I ultimately decided against
      that because in theory, a user who wants to spin up this small, simple server should be able to do so without knowing the socket
      programming or request/response model of the class. It should be as simple as instantiating it and processing requests.

    - I also debated putting the `while(continueRunning)` loop inside the `HttpServer::ProcessRequest` function, and renaming it
      `HttpServer::Run()`. I decided against that for a simple reason: the user has finer control on when the server should process
      requests. If it should do so indefinitely (as in this project), the user simply needs to call `HttpServer::ProcessRequest` in a
      loop. In other applications, perhaps the server only needs to be listening for a selectied amount of time, or perhaps even a single
      request. I think this makes incorporating the server into other projects simpler.

- Socket connection acceptance is non-blocking
    - This was chosen in tandem with the above information. If the socket is blocking, calls to accept() block until a new request.
      In an application where the user wanted control over when the server handles requests, a blocking call would take this capability
      away. Again, handling interrupt signals becomes more difficult as well. The tradeoff is that in a long-running loop, checking
      the connection pool repeatedly becomes a poll, and can increase the programs CPU usage significantly. I incorporated a slight
      `sleep` in the main processing loop to avoid this overhead. A real-world program might have more code between calls to check
      for requests, and the sleep might not be necessary.

### Technology Choices
- Show what I know
    - I am most familiar with Linux programming (and that is all I have access to at home), so I wrote the project for Linux x86_64.
    - I know that newer versions of googletest have built in mocking through GMock. I have not used GMock before, so I instead built
      an inheritance-based Mock model for unit testing non-public functions.

### Data Storage Choice
- The 'Database' of my project is an internal C++ unordered_map. I made this choice for two reasons:
    1) As stated above, I wanted to use only C++11 compliant code, and have as few dependencies as possible.
    2) Bringing in a full fledged database for a simple server like this seemed like more effort than it was worth.

    A true production version of an HTTP server would of course have an actual database behind it's API layer to store data. It just
    seemed like too much for this project.

- The server is not capable of handling request data larger than 10MB.
    - This is clearly not a reasonable assumption for an actual HTTP server. For the sake of this project, it seems an appropriate
      amount of data to cache in memory if we aren't handling more than a few dozen PUT requests. By choosing to do the database
      as a C++ structure, I had to draw the line somewhere. 

## Coding Principles / Goals:
- As a good C++ coding practice, no raw pointers, unless unavoidable.

- Don't throw, unless necessary
    - Personally, I find throwing messy, and only had one instance in this code that would require a 'hard stop' on processing.

- Favor verbosity over brevity
    - I think clean code should aim to explain itself, and reserve comments for tricky, unintuitive sections, or to break up longer
      blocks of code.
    - Be explicit with variable types. I prefer prefixing every std:: call with 'std::' as a good habit, so
      that when calls are not coming from the std:: namespace, readers do not have to navigate across files to find out which
      namespace they're in. Just personal preference.

- Maximum code coverage through unit tests
    - Only one function is not unit tested - `HttpServer::ProcessRequest`. This function returns void and relies heavily on system
      calls for socket functionality. I could have used the `MockHttpServer` class to track call counts of system calls, but the
      existing unit tests gave me high confidence that ProcessRequest would work as intended.

## Assumptions:
- Each string in the first line of the request is separated by a single space
- Any request that doesn't specify HTTP version 1.1 is invalid
- Request will be generated by a valid HTTP client
    - Examples of things the code assumes:
        - Header strings with malformed such as "Content-Length= 8" or "Content-Length:8" will not be used
        - Header strings with mismatched key-values such as "Content-Length: eighteen" will not be used
        - Headers will not be duplicated. I.E. having multiple "Content-Length" headers specified in one request
    - This doesn't include otherwise valid, but errant requests, such as a GET on a resource that doesn't exist
- Requests won't contain data larger than 10MB. They will be truncated if so.