#include <stdio.h>
#include <QString>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QColor>
#include <stdint.h>

int main(int argc, char *argv[])
{
    QTextStream cout(stdout, QIODevice::WriteOnly);

    if (argc < 2)
    {
        cout << "Please give the input file name." << endl;
        return -1;
    }

    QFile ifile;
    ifile.setFileName(argv[1]);
    if (!ifile.exists())
    {
        cout << "Can't find " << ifile.fileName() << endl;
        return -1;
    }

    QImage im(ifile.fileName());
    const int height = im.height() / 8;
    const int width = im.width();
    cout << "Processing...\n" << "Size " << width << "x" << height * 8 << endl;
    QVector<QString> data;
    data.clear();
    data.append("#include <stdint.h>\n\n");
    data.append(QString("const uint8_t img[%1][%2] =\n").arg(height).arg(width));
    data.append("{\n");
    for (int h = 0; h < height; ++h)
    {
        QString line;
        line.append("    { ");
        for (int w = 0; w < width; ++w)
        {
            uint8_t hex = 0;
            int y = 8 * h;
            for (int d = 0; d < 8; ++d)
                if (qGray(im.pixel(w, y + d)) > 0x7F)
                    hex |= 1 << d;
            line.append(QString("0x%1, ").arg(hex, 2, 16, QChar('0')));
        }
        line.append("},\n");
        data.append(line);
    }
    data.append("};\n");

    QFile ofile("output.c");
    ofile.open(QIODevice::WriteOnly);

    for (int i = 0; i < data.size(); ++i)
        ofile.write(data.at(i).toUtf8());
    ofile.close();
    return 0;
}
