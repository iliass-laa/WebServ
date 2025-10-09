#include"EventLoop.hpp"

EventLoop::EventLoop(){}
EventLoop::~EventLoop(){}
EventLoop::EventLoop(const EventLoop& obj){
    (void)obj;
}
EventLoop& EventLoop::operator=(const EventLoop& obj){
    (void)obj;
    return *this;
}

void EventLoop::addSocket(int fd, short events){
    // create new pollfd , and set fd and event(pollin or pollout)
    pollfd tmp;
    tmp.events = events;
    tmp.fd = fd;

    poll_fds.push_back(tmp);
    // add the new fd with the index of it in the pollfd vector
    fd_to_index[fd] = poll_fds.size() -1;
}

// void EventLoop::editSocket(int fd, short events){
//     std::vector<pollfd>::iterator client ;
//     for(; client != poll_fds.end(); client++){
//         if(client->fd == fd)
//             break;   
//     }
//     client->events =  events; 
// }


void EventLoop::removeSocket(int fd){
    std::map<int, int>::iterator it = fd_to_index.find(fd);
    int index = it->second ;
    if(index != static_cast<int>(poll_fds.size()) -1)
    {
        poll_fds[index] = poll_fds.back(); // switch pollfd[index] wit the last elem
        fd_to_index[poll_fds[index].fd] = index ; // add the index for the elem swithched
    }
    poll_fds.pop_back();
    fd_to_index.erase(fd);
}

void EventLoop::updateSocketEvents(int fd, short events)
{
    std::map<int, int>::iterator it = fd_to_index.find(fd);
    if(it != fd_to_index.end())
        poll_fds[it->second].events = events ;
}

std::vector<std::pair<int, short> > EventLoop::waitForEvents(int timeout){
    std::vector<std::pair<int , short> > ready_fds;
    // passing ptr to 1st poll_fd , poll does understand vecctor and c++ huh
    // and the number of fds to monitor obligatoire
    // last param for time out 
    int ready = poll(&poll_fds[0], poll_fds.size(), timeout);  
    std::cout << "poll() ==>" << std::endl;
    // ready = the number of structures which have nonzero revents > 0 in success
    if(ready > 0)
    {
        for(
        std::vector<pollfd>::const_iterator it = poll_fds.begin() ;
        it != poll_fds.end() ;
        it++){
            if(it->revents != 0)
            ready_fds.push_back(std::make_pair(it->fd, it->revents));    
        }
    }

    return ready_fds;
}
