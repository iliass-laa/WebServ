#pragma once

#include<string>
#include<map>
#include<ctime>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <iomanip>
#include <errno.h>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <time.h>
// #include<string>

typedef struct Session_strct {

    std::string id;
    std::map<std::string, std::string> data; // key value of cookie data 
    time_t created;
    time_t lastAccess;
    time_t expirey;
} Session_t;

class Session{
    private :
        unsigned int ttl; // time to live , session expirey time in secondes , it my comment btw , flawless english
        std::map<std::string, Session_t> sessions; // all the created , updated sessions here , SID : Session
        
        static std::string gen_sid_hex();               // internal: generate 16-byte random SID hex
        static time_t now();                            // internal: current time
        // static std::string format_rfc1123_time(time_t t);   // internal: format Expires header (RFC1123) for given time_t
    public  :
        Session(unsigned int ttl_seconds = 3600); // 1 hour default

        // Create a new session and return its ID
        // std::string create_session(std::map<std::string, std::string>&);
        std::string create_session(std::string sid);


        // Lookup session by id. Returns pointer to Session or NULL if not found/expired.
        // If found, updates last_access and extends expires_at (sliding ttl).
        Session_t* lookup_session(const std::string& id);

        // Destroy session by id (logout)
        // void destroy_session(const std::string& id);

        // Regenerate session id: create new id, move data, remove old id.
        // Returns new id, or empty string on failure.
        // std::string regenerate_session(const std::string& old_id);

        // Run garbage collection to remove expired sessions
        void gc();

        /* fidriss should use those on responce !!!!!!!!!!!!!!!!!!!!!!
            Build Set-Cookie header value for given SID (HTTP/1.1 friendly):
            includes Max-Age and Expires (RFC1123), HttpOnly, SameSite, optional Secure.
        */
        // std::string make_set_cookie(const std::string& sid, unsigned int max_age) const;

        // Build Set-Cookie header value to delete a cookie (Max-Age=0 + Expires in past)
        // std::string make_delete_cookie(const std::string& name) const;

        /*
            Parse Cookie header string and return value of cookie named name ("" if not found)
            Example header: "a=1; b=2; SID=abcd"
        */ 
        // static std::string parse_cookie_header(const std::string& cookie_header, const std::string& name);

        // Expose map size for tests/monitoring
        // size_t store_size() const;
};