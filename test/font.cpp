#include <RFormat.h>
#include <rsc/RFont.h>

using namespace Redopera;

int main()
{
    RFont font;
    font.setSize(22);

    fmt::print("========Font Glyph Load Test========\n"
               "Defualt font: SourceCodePro, Font size: {}\n", font.size());

    char c;
    do {
        fmt::print("\nEntry a character ('q' quit): \n");
        std::scanf("%c", &c);
        std::getchar();
        RFont::Glyph glyph = font.getGlyph(c);
        fmt::print("character: {}, width: {}, height: {}, xoff: {}, yoff: {}\n",
                   c, glyph.width, glyph.height, glyph.xoff, glyph.yoff);

        for (int j = 0; j < glyph.height; ++j)
        {
           for (int i = 0; i < glyph.width; ++i)
               printf("%c ", " ':0IM%@"[glyph.data[j*glyph.width+i] >> 5]);
           printf("\n");
        }
        printf("\n");
    } while(c != L'q');

    return 0;
}
