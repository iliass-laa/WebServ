#include "Session.hpp"

Session::Session(unsigned int ttl_seconds)
: ttl(ttl_seconds) {}

time_t Session::now() {
    return time(NULL);
}

std::string Session::gen_sid_hex() {
    unsigned char buf[16];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        for (int i = 0; i < 16; ++i) buf[i] = (unsigned char)(rand() & 0xff);
    } else {
        ssize_t r = read(fd, buf, sizeof(buf));
        (void)r;
        close(fd);
    }
    std::ostringstream ss;
    ss.setf(std::ios::hex);
    ss.fill('0');
    for (int i = 0; i < 16; ++i) {
        ss << std::setw(2) << (int)buf[i];
    }
    return ss.str();
}

std::string Session::create_session(std::string sid) {

    time_t t = now();
    Session_t s;
    s.id = sid;
    s.created = t;
    s.lastAccess = t;
    s.expirey = t + ttl;
    sessions[sid] = s;
    return sid;
}

Session_t* Session::lookup_session(const std::string& id) {
    std::map<std::string, Session_t>::iterator it = sessions.find(id);
    if (it == sessions.end())
        return NULL;
    time_t t = now();
    if (t > it->second.expirey) {
        sessions.erase(it);
        return NULL;
    }
    it->second.lastAccess = t;
    it->second.expirey = t + ttl;
    return &it->second;
}

void Session::gc() {
    time_t t = now();
    std::vector<std::string> to_remove;
    for (std::map<std::string, Session_t>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
        if (t > it->second.expirey) to_remove.push_back(it->first);
    }
    for (size_t i = 0; i < to_remove.size(); ++i) sessions.erase(to_remove[i]);
}

size_t Session::store_size() const {
    return sessions.size();
}