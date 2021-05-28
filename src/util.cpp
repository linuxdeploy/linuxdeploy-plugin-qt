// library headers
#include <linuxdeploy/util/util.h>
#include <linuxdeploy/subprocess/subprocess.h>

// local headers
#include "util.h"

using namespace linuxdeploy::subprocess;

std::map<std::string, std::string> queryQmake(const boost::filesystem::path& qmakePath) {
    auto qmakeCall = subprocess({qmakePath.string(), "-query"}).run();

    using namespace linuxdeploy::core::log;

    if (qmakeCall.exit_code() != 0) {
        ldLog() << LD_ERROR << "Call to qmake failed:" << qmakeCall.stderr_string() << std::endl;
        return {};
    }

    std::map<std::string, std::string> rv;

    std::stringstream ss;
    ss << qmakeCall.stdout_string();

    std::string line;

    auto stringSplit = [](const std::string& str, const char delim = ' ') {
        std::stringstream ss;
        ss << str;

        std::string part;
        std::vector<std::string> parts;

        while (std::getline(ss, part, delim)) {
            parts.push_back(part);
        }

        return parts;
    };

    while (std::getline(ss, line)) {
        auto parts = stringSplit(line, ':');

        if (parts.size() != 2)
            continue;

        rv[parts[0]] = parts[1];
    }

    return rv;
};

boost::filesystem::path findQmake() {
    using namespace linuxdeploy::core::log;

    boost::filesystem::path qmakePath;

    // allow user to specify absolute path to qmake
    if (getenv("QMAKE")) {
        qmakePath = getenv("QMAKE");
        ldLog() << "Using user specified qmake:" << qmakePath << std::endl;
    } else {
        // search for qmake
        qmakePath = linuxdeploy::util::which("qmake-qt5");

        if (qmakePath.empty())
            qmakePath = linuxdeploy::util::which("qmake");
    }

    return qmakePath;
}

bool pathContainsFile(boost::filesystem::path dir, boost::filesystem::path file) {
    // If dir ends with "/" and isn't the root directory, then the final
    // component returned by iterators will include "." and will interfere
    // with the std::equal check below, so we strip it before proceeding.
    if (dir.filename() == ".")
        dir.remove_filename();
    // We're also not interested in the file's name.
    assert(file.has_filename());
    file.remove_filename();

    // If dir has more components than file, then file can't possibly
    // reside in dir.
    auto dir_len = std::distance(dir.begin(), dir.end());
    auto file_len = std::distance(file.begin(), file.end());
    if (dir_len > file_len)
        return false;

    // This stops checking when it reaches dir.end(), so it's OK if file
    // has more directory components afterward. They won't be checked.
    return std::equal(dir.begin(), dir.end(), file.begin());
};

std::string join(const std::vector<std::string> &list) {
    return join(list.begin(), list.end());
}

std::string join(const std::set<std::string> &list) {
    return join(list.begin(), list.end());
}

bool strStartsWith(const std::string &str, const std::string &prefix) {
    if (str.size() < prefix.size())
        return false;

    return strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}

bool strEndsWith(const std::string &str, const std::string &suffix) {
    if (str.size() < suffix.size())
        return false;

    return strncmp(str.c_str() + (str.size() - suffix.size()), suffix.c_str(), suffix.size()) == 0;
}

