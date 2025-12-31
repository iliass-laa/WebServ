#pragma once

#include<string>
#include<iostream>
#include<map>
#include<ctime>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <time.h>

typedef struct Session_strct {

    std::string id;
    std::map<std::string, std::string> data;
    time_t created;
    time_t lastAccess;
    time_t expirey;
} Session_t;

class Session{
    private :
        unsigned int ttl;
        std::map<std::string, Session_t> sessions;
        
        static std::string gen_sid_hex();
        static time_t now();
    public  :
        Session(unsigned int ttl_seconds = 3600);
        std::string create_session(std::string sid);
        Session_t* lookup_session(const std::string& id);
        void gc();
        size_t store_size() const;
};