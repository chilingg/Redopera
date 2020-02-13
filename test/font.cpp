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
        std::cout << "\nEntry a character ('q' quit): ";
        std::cin >> c;
        const RFont::Glyph *glyph = font.getFontGlyph(c);
        rDebug << "\ncharacter:" << c << " width:" << glyph->width << " height:" << glyph->height
               << " xoff:" << glyph->xoff << " yoff:" << glyph->yoff << '\n';

        for (int j = 0; j < glyph->height; ++j)
        {
           for (int i = 0; i < glyph->width; ++i)
               std::cout << " ':0IM%@"[glyph->data[j*glyph->width+i] >> 5] << ' ';
           std::cout << '\n';
        }
        std::cout  << std::endl;
    } while(c != L'q');

    return 0;
}
