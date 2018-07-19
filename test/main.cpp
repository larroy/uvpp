#include <thread>
#include <iostream>
#include <string>
#include "Server.h"

using namespace std;

int main()
{
    Server s;

    s.set_port(12345);
    s.start();

    return 0;
}