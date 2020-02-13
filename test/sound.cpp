#include <RDebug.h>
#include <RAudioStream.h>

using namespace Redopera;

void message()
{
    prError("Please enter a path for mp3 file!");
    exit(EXIT_SUCCESS);
}

int main(int argc, const char *argv[])
{
    if(argc != 2)
        message();

    RMp3 mp3(argv[1]);
    if(!mp3.isValid())
        message();

    RAudioStream audio(mp3);
    if(!audio.startStream())
        return 0;

    rDebug << "Press Q to quit play";
    char c;
    do {
        std::cin >> c;
    }
    while(c != 'q' && c != 'Q');

    return 0;
}
