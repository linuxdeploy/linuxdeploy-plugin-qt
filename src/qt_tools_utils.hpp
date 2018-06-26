/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_TOOLS_UTILS
#define QT_TOOLS_UTILS

#include <string>
#include <fstream>

#include <iostream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "../lib/linuxdeploy/include/linuxdeploy/core/log.h"

using namespace std;
using namespace boost::filesystem;
using namespace linuxdeploy::core::log;

vector<char> readFileContent(const path& path, string* errorMessage);
void writeFileContent(const path& path, const vector<char>& content, string* errorMessage);

// Search for "qt_prfxpath=xxxx" in \a path, and replace it with "qt_prfxpath=."
void patchQtCore(const path& path, string* errorMessage = nullptr)
{
    ldLog() << "Patching " << path.filename() << "...\n";

    auto content = readFileContent(path, errorMessage);
    if (content.empty()) {
        *errorMessage = "Unable to patch"+path.string()+": Could not read file content";
        return;
    }

    char prfxpath[] = "qt_prfxpath=";
    size_t len = strlen(prfxpath);

    auto startPos = std::search(content.begin(), content.end(), prfxpath, prfxpath+len);
    if (startPos==content.end()) {
        *errorMessage = "Unable to patch "+path.string()+": Could not locate pattern \"qt_prfxpath=\"";
        return;
    }

    startPos += len;
    auto endPos = startPos;
    while (endPos!=content.end() && *endPos!='\0')
        endPos++;

    if (endPos==content.end()) {
        *errorMessage = "Unable to patch "+path.string()+": Internal error";
        return;
    }

    auto oldValue = vector<char>(startPos, endPos);
    vector<char> newValue(static_cast<unsigned long>(endPos-startPos), char(0));
    newValue[0] = '.';
    ldLog() << "Replaced prfxpath: " << oldValue << " by " << newValue;

    for (auto i = startPos; i<endPos; i++)
        *i = newValue[i-startPos];

    writeFileContent(path, content, errorMessage);
}
void writeFileContent(const path& path, const vector<char>& content, string* errorMessage)
{
    std::ofstream outfile(path.string(), std::ofstream::binary);
    outfile.write(content.data(), content.size());
    if (outfile.fail())
        *errorMessage = "Unable to write : "+path.string();

    outfile.close();
}
vector<char> readFileContent(const path& path, string* errorMessage)
{
    std::ifstream inFileStream(path.string(), std::ifstream::binary);
    if (inFileStream) {
        // get length of file:
        inFileStream.seekg(0, std::ifstream::end);
        auto length = static_cast<unsigned long>(inFileStream.tellg());
        inFileStream.seekg(0, std::ifstream::beg);
        auto* buffer = new char[length];

        inFileStream.read(buffer, length);

        if (inFileStream.fail())
            *errorMessage = "Unable to read : "+path.string();

        inFileStream.close();
        vector<char> output(buffer, buffer+length);
        delete[] buffer;
        return output;
    }
}

#endif QT_TOOLS_UTILS