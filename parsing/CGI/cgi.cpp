#include "cgi.hpp"


cgiHandling::cgiHandling (){}

cgiHandling::~cgiHandling (){}

void cgiHandling::handelCGI(BaseNode *root, HttpRequest req, std::vector<char> &resp)
{
    setInfo(req);
}