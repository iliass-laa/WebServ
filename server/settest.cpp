#include <set>
#include <string>
#include <iostream>

int main(){

    std::set<std::string> pairs;

    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8081");
    pairs.insert("0.0.0.0:8080");
    pairs.insert("0.0.0.0:8080");

    for (std::set<std::string>::iterator it = pairs.begin() ; it != pairs.end() ; it++){
        std::cout << *it << std::endl;
    }
    return 0;
}