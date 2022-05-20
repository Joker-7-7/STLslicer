#ifndef PRINTER_H
#define PRINTER_H

struct Resolution
{
    Resolution(const unsigned int x, const unsigned int y) : x(x), y(y) {}
    const unsigned int x, y;
};
const unsigned int X = 500;
const unsigned int Y = 500;
Resolution printer_resolution(X, Y);
const GLfloat pixel = 0.05; // pixel pitch (um)

#endif