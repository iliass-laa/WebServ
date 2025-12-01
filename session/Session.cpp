#include "Session.hpp"


// Constructor
Session::Session(unsigned int ttl_seconds)
: ttl(ttl_seconds) {}

// get current time
time_t Session::now() {
    return time(NULL);
}

// // RFC1123 formatting for Expires header (HTTP/1.1 friendly)
// std::string Session::format_rfc1123_time(time_t t) {
//     // Example: Wed, 09 Jun 2021 10:18:14 GMT
//     char buf[64];
//     struct tm gm;
//     gmtime_r(&t, &gm);
//     // strftime with RFC1123 format
//     if (strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gm) == 0) {
//         return std::string();
//     }
//     return std::string(buf);
// }

// generate 16 random bytes from /dev/urandom and hex-encode
std::string Session::gen_sid_hex() {
    unsigned char buf[16];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        // fallback to rand() (not ideal) - keep simple fallback
        for (int i = 0; i < 16; ++i) buf[i] = (unsigned char)(rand() & 0xff);
    } else {
        ssize_t r = read(fd, buf, sizeof(buf));
        (void)r;
        close(fd);
    }
    // hex encode
    std::ostringstream ss;
    ss.setf(std::ios::hex);
    ss.fill('0');
    for (int i = 0; i < 16; ++i) {
        ss << std::setw(2) << (int)buf[i];
    }
    return ss.str();
}

std::string Session::create_session(std::string sid) {
    // std::string sid;
    // ensure uniqueness (loop a few times)
    // for (int tries = 0; tries < 5; ++tries) {
    //     sid = gen_sid_hex();
    //     if (sessions.find(sid) == sessions.end()) break;
    //     sid.clear();
    // }
    // if (sid.empty()) return std::string();

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
        // expired
        sessions.erase(it);
        return NULL;
    }
    // sliding expiration: update last_access and expires_at
    it->second.lastAccess = t;
    it->second.expirey = t + ttl;
    return &it->second;
}

// void Session::destroy_session(const std::string& id) {
//     sessions.erase(id);
// }

// std::string Session::regenerate_session(const std::string& old_id) {
//     std::map<std::string, Session_t>::iterator it = sessions.find(old_id);
//     if (it == sessions.end()) return std::string();
//     // copy data
//     Session_t copy = it->second;
//     // generate new sid
//     std::string new_sid;
//     for (int tries = 0; tries < 5; ++tries) {
//         new_sid = gen_sid_hex();
//         if (sessions.find(new_sid) == sessions.end()) break;
//         new_sid.clear();
//     }
//     if (new_sid.empty()) return std::string();
//     // move
//     copy.id = new_sid;
//     time_t t = now();
//     copy.lastAccess = t;
//     copy.expirey = t + ttl;
//     sessions.erase(it);
//     sessions[new_sid] = copy;
//     return new_sid;
// }

void Session::gc() {
    time_t t = now();
    std::vector<std::string> to_remove;
    for (std::map<std::string, Session_t>::iterator it = sessions.begin(); it != sessions.end(); ++it) {
        if (t > it->second.expirey) to_remove.push_back(it->first);
    }
    for (size_t i = 0; i < to_remove.size(); ++i) sessions.erase(to_remove[i]);
}

// std::string Session::make_set_cookie(const std::string& sid, unsigned int max_age) const {
//     // Build Set-Cookie header value. For HTTP/1.1 include Max-Age and Expires
//     time_t expiry = now() + (time_t)max_age;
//     std::ostringstream s;
//     s << "SID=" << sid;
//     s << "; Path=/";
//     s << "; Max-Age=" << max_age;
//     std::string exp = format_rfc1123_time(expiry);
//     if (!exp.empty()) {
//         s << "; Expires=" << exp;
//     }
//     s << "; HttpOnly";
//     s << "; SameSite=Lax";
//     return s.str();

//     /*
//     SID=value;
//     Path=/";
//     Max-Age=max_age;
//     Expires=exp;
//     HttpOnly ;
//     SameSite=Lax;
//     */
// }

// std::string Session::make_delete_cookie(const std::string& name) const {
//     // Expires in the past and Max-Age=0 per HTTP/1.1/Set-Cookie semantics
//     time_t past = 1; // 1970-01-01T00:00:01Z
//     std::ostringstream s;
//     s << name << "=; Path=/";
//     s << "; Max-Age=0";
//     s << "; Expires=" << format_rfc1123_time(past);
//     s << "; HttpOnly";
//     s << "; SameSite=Lax";
//     return s.str();
// }

// Parse Cookie header: "name1=val1; name2=val2"
// return value for cookie name or empty string
// std::string Session::parse_cookie_header(const std::string& cookie_header, const std::string& name) {
//     size_t pos = 0;
//     while (pos < cookie_header.size()) {
//         // skip whitespace and separators
//         while (pos < cookie_header.size() && (cookie_header[pos] == ' ' || cookie_header[pos] == '\t' || cookie_header[pos] == ';')) pos++;
//         if (pos >= cookie_header.size()) break;
//         // find '='
//         size_t eq = cookie_header.find('=', pos);
//         if (eq == std::string::npos) break;
//         std::string n = cookie_header.substr(pos, eq - pos);
//         // trim trailing spaces from name
//         while (!n.empty() && (n[n.size()-1] == ' ' || n[n.size()-1] == '\t')) n.erase(n.size()-1);
//         pos = eq + 1;
//         // value up to semicolon
//         size_t semi = cookie_header.find(';', pos);
//         std::string v;
//         if (semi == std::string::npos) {
//             v = cookie_header.substr(pos);
//             pos = cookie_header.size();
//         } else {
//             v = cookie_header.substr(pos, semi - pos);
//             pos = semi + 1;
//         }
//         // trim whitespace around value
//         size_t start = 0;
//         while (start < v.size() && (v[start] == ' ' || v[start] == '\t')) start++;
//         size_t end = v.size();
//         while (end > start && (v[end-1] == ' ' || v[end-1] == '\t')) end--;
//         std::string val = v.substr(start, end - start);
//         if (n == name) return val;
//     }
//     return std::string();
// }

size_t Session::store_size() const {
    return sessions.size();
}