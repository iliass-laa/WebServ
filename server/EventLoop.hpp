#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP
#include<vector>
#include<map>
#include<poll.h>

class EventLoop {
    private :
        std::vector<pollfd> poll_fds;
        std::map<int,int> fd_to_index;
    public:
        EventLoop();
        ~EventLoop();
        EventLoop(const EventLoop& );
        EventLoop& operator=(const EventLoop& );

        void addSocket(int fd, short events);
        void removeSocket(int fd);
        void updateSocketEvents(int fd, short events);
        std::vector<std::pair<int, short> > waitForEvents(int timeout);

};

#endif