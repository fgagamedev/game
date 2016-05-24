#include <ijengine/exception.h>
#include "ije02_game.h"
#include <iostream>

using namespace ijengine;
using namespace std;

int main()
{
    int rc;

    try
    {
        Ije02Game game("Teste", 21 * 64, 11 * 64);
        rc = game.run("red");
    } catch (Exception& ex)
    {
        cout << ex.what() << endl;
    }

    return rc;
}