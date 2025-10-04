#include <set>
#include <string>
#include <vector>
#include <iostream>

int main(){

    // std::set<std::string> pairs;

    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8081");
    // pairs.insert("0.0.0.0:8080");
    // pairs.insert("0.0.0.0:8080");

    // for (std::set<std::string>::iterator it = pairs.begin() ; it != pairs.end() ; it++){
    //     std::cout << *it << std::endl;
    // }

    std::vector<std::string> numbers;
    std::string curr;
    std::string ip("127.00.0");

    for(int i = 0 ; i < ip.length() ; i++){
        if(ip[i] == '.' ){
            if(curr.empty())
                return (std::cout << "error 0"<< std::endl ,false) ;
            numbers.push_back(curr);
            curr.clear();
        }else
            curr.push_back(ip[i]);
    }
    if(!curr.empty())
        numbers.push_back(curr);
    if(numbers.size() != 4)
        return (std::cout << "error 1"<< std::endl ,false);
        
    std::cout << numbers[0]<< std::endl;
    std::cout << numbers[1]<< std::endl;
    std::cout << numbers[2]<< std::endl;
    std::cout << numbers[3]<< std::endl;
    return 0;
}