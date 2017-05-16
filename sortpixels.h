#ifndef SORTPIXELS_H
#define SORTPIXELS_H

#include <QMainWindow>

class sortpixels : public QObject
{
    Q_OBJECT

public:
    enum Mode {
        Horizontal, Vertical, ASDF
    };

    sortpixels(Mode, QImage*, double, int);

    void sortPixels();

signals:
    void progressChanged(int);

private:
    Mode sortMode;

    QImage *image;

    double edgeThreshold;

    int brightnessValue;

    int brightness(int, int);
    int getFirstBrightX(int, int);
    int getNextDarkX(int, int);
    int getFirstBrightY(int, int);
    int getNextDarkY(int, int);
    void sortRow();
    void sortColumn();
};

#endif // SORTPIXELS_H
