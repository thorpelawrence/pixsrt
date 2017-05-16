#include "sortpixels.h"
#include "ui_mainwindow.h"

sortpixels::sortpixels(Mode sortMode, QImage *image, double edgeThreshold, int brightnessValue)
{
    this->sortMode = sortMode;
    this->image = image;
    this->edgeThreshold = edgeThreshold;
    this->brightnessValue = brightnessValue;
}

bool colorBrightnessLessThan(QRgb &c1, QRgb &c2) {
    int maxC1 = qMax(qRed(c1), qMax(qGreen(c1), qBlue(c1)));
    int maxC2 = qMax(qRed(c2), qMax(qGreen(c2), qBlue(c2)));
    return maxC1 < maxC2;
}

int row = 0;
int column = 0;

int width;
int height;

int sortpixels::brightness(int x, int y) {
    QRgb pixel = image->pixel(x,y);
    return qMax(qRed(pixel), qMax(qGreen(pixel), qBlue(pixel)));
}

int sortpixels::getFirstBrightX(int _x, int _y) {
    int x = _x;
    int y = _y;

    while (brightness(x,y) < brightnessValue) {
        x++;
        if (x >= width) return -1;
    }
    return x;
}

int sortpixels::getNextDarkX(int _x, int _y) {
    int x = _x + 1;
    int y = _y;

    if (x >= width) return width-1;

    while (brightness(x,y) > brightnessValue) {
        x++;
        if (x >= width) return width-1;
    }
    return x-1;
}

int sortpixels::getFirstBrightY(int _x, int _y) {
    int x = _x;
    int y = _y;

    if (y < height) {
        while (brightness(x,y) < brightnessValue) {
            y++;
            if (y >= height) return -1;
        }
    }
    return y;
}

int sortpixels::getNextDarkY(int _x, int _y) {
    int x = _x;
    int y = _y + 1;

    if (y < height) {
        while (brightness(x,y) > brightnessValue) {
            y++;
            if (y >= height) return height-1;
        }
    }
    return y-1;
}

void sortpixels::sortRow() {
    int x = 0;
    int y = row;
    int xend = 0;

    while (xend < width-1) {
        x = getFirstBrightX(x, y);
        xend = getNextDarkX(x, y);

        if (x < 0) break;

        int sortLength = xend - x;

        QVector<QRgb> sortedColors;

        QRgb *pixel = reinterpret_cast<QRgb*>(image->scanLine(y)) + x;
        for (int i = 0; i < sortLength; i++) {
            sortedColors.append(*pixel++);
        }

        std::sort(sortedColors.begin(), sortedColors.end(), colorBrightnessLessThan);

        pixel = reinterpret_cast<QRgb*>(image->scanLine(y)) + x;
        for (int i = 0; i < sortLength; i++) {
            *pixel++ = sortedColors[i];
        }

        x = xend + 1;
    }
}

void sortpixels::sortColumn() {
    int x = column;
    int y = 0;
    int yend = 0;

    while (yend < height-1) {
        y = getFirstBrightY(x, y);
        yend = getNextDarkY(x, y);

        if (y < 0) break;

        int sortLength = yend - y;

        QVector<QRgb> sortedColors;

        for (int i = 0; i < sortLength; i++) {
            QRgb *pixel = reinterpret_cast<QRgb*>(image->scanLine(y+i)) + x;
            sortedColors.append(*pixel);
        }

        std::sort(sortedColors.begin(), sortedColors.end(), colorBrightnessLessThan);

        for (int i = 0; i < sortLength; i++) {
            QRgb *pixel = reinterpret_cast<QRgb*>(image->scanLine(y+i)) + x;
            *pixel = sortedColors[i];
        }

        y = yend + 1;
    }
}

void sortpixels::sortPixels()
{
    width = image->width();
    height = image->height();
    row = 0;
    column = 0;

    if (sortMode == Mode::ASDF) {
        while (column < width-1) {
            int percentage = (int)((float)column / width * 50);
            emit progressChanged(percentage);
            sortColumn();
            column++;
        }

        while (row < height-1) {
            int percentage = 50 + (int)((float)row / height * 50);
            emit progressChanged(percentage);
            sortRow();
            row++;
        }
    }

    if (sortMode == Mode::Horizontal || sortMode == Mode::Vertical)
    {
//        if (sortMode == Mode::Vertical) {
//            QTransform transform;
//            transform.rotate(90);
//            image = new QImage(image->transformed(transform));
//            width = image->width();
//            height = image->height();
//        }

        QVector<QVector<QRgb>> sortedPixels;

        for (int y = 0; y < height; y++)
        {
            QHash<int, QRgb> row;
            QHash<int, QRgb> rowPart;
            for (int x = 0; x < width; x++) {
                if (x > 0) {
                    float leftPixelBrightness = brightness(x-1,y);
                    if (leftPixelBrightness - edgeThreshold > brightness(x,y)
                            || leftPixelBrightness + edgeThreshold < brightness(x,y))
                    {
                        rowPart.insert(x, image->pixel(x,y));
                    }
                    else {
                        row.insert(x, image->pixel(x,y));
                    }
                }
                else {
                    row.insert(x, image->pixel(x,y));
                }
            }

            QHashIterator<int, QRgb> rowPartIterator(rowPart);
            QVector<int> rowPartKeyList;
            QVector<QRgb> rowPartValList;
            while (rowPartIterator.hasNext()) {
                rowPartIterator.next();
                rowPartKeyList.append(rowPartIterator.key());
                rowPartValList.append(rowPartIterator.value());
            }

            std::sort(rowPartValList.begin(), rowPartValList.end(), colorBrightnessLessThan);

            for (int part = 0; part < rowPartValList.length(); part++) {
                row.insert(rowPartKeyList[part], rowPartValList[part]);
            }

            QVector<QRgb> rowValList;
            QHashIterator<int, QRgb> rowIterator(row);
            while (rowIterator.hasNext()) {
                rowIterator.next();
                rowValList.append(rowIterator.value());
            }

            sortedPixels.append(rowValList);

            int percentage = (int)((float)y / height * 100);
            emit progressChanged(percentage);
        }

        for (int y = 0; y < height; y++) {
            QRgb *pixel = reinterpret_cast<QRgb*>(image->scanLine(y));
            for (int x = 0; x < width; x++) {
                pixel++;
                *pixel = sortedPixels[y][x];
            }
        }

//        if (sortMode == Mode::Vertical) {
//            QTransform transform;
//            transform.rotate(-90);
//            image = new QImage(image->transformed(transform));
//            width = image->width();
//            height = image->height();
//        }
    }
}
