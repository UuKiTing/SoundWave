#ifndef STYLESHEETUTILS_H
#define STYLESHEETUTILS_H

#include <QString>

class StyleSheetUtils
{
public:
    StyleSheetUtils();

    static QString loadStyleSheet(const QString &filePath);
};

#endif // STYLESHEETUTILS_H
