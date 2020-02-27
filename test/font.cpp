#include <RDebug.h>
#include <rsc/RFont.h>

using namespace Redopera;

int main()
{
    RFont font;
    font.setSize(22);

    rDebug << "========Font Glyph Load Test========\n"
              "Defualt font: SourceCodePro, Font size :" << font.size();

    char c;
    do {
        rDebug << "\nEntry a character ('q' quit): ";
        std::scanf("%c", &c);
        std::getchar();
        const RFont::Glyph *glyph = font.getFontGlyph(c);
        rDebug << "\ncharacter:" << c << " width:" << glyph->width << " height:" << glyph->height
               << " xoff:" << glyph->xoff << " yoff:" << glyph->yoff << '\n';

        for (int j = 0; j < glyph->height; ++j)
        {
           for (int i = 0; i < glyph->width; ++i)
               printf("%c ", " ':0IM%@"[glyph->data[j*glyph->width+i] >> 5]);
           printf("\n");
        }
        printf("\n");
    } while(c != L'q');

    return 0;
}
