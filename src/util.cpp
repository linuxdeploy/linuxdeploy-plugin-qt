// system headers
#include <assert.h>
#include <random>

// library headers
#include <linuxdeploy/log/log.h>
#include <linuxdeploy/util/util.h>
#include <linuxdeploy/subprocess/subprocess.h>

// local headers
#include "util.h"

using namespace linuxdeploy::subprocess;

constexpr auto tempDirCreationAttempts = 50;
constexpr std::string_view tempDirAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
constexpr auto randomPartLength = 10;

TempDir::TempDir(std::string_view name) {
    create(name);
}

void TempDir::create(std::string_view name) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, tempDirAlphabet.size());

    std::array<char, randomPartLength> randomPart;

    if (!tmpDirPath.empty()) {
        throw std::runtime_error("Temporary directory exists already!");
    }

    namespace fs = std::filesystem;
    for (unsigned int i = 0; i < tempDirCreationAttempts; ++i) {
        for (int i = 0; i < randomPart.size(); ++i)
            randomPart[i] = tempDirAlphabet[distrib(gen)];

        std::string filename = std::string(name) + '-';
        filename.append(randomPart.data(), randomPart.size());

        fs::path newTmpDirPath = fs::temp_directory_path() / filename;

        bool success = fs::create_directory(newTmpDirPath);
        if (success) {
            tmpDirPath = newTmpDirPath;
            return;
        }
    }

    throw std::runtime_error("Couldn't create temporary directory for " + std::string(name) + '!');
}

TempDir::TempDir(TempDir && other) noexcept : tmpDirPath(other.tmpDirPath) {
    other.tmpDirPath.clear();
}

TempDir &TempDir::operator=(TempDir && other) noexcept {
    if (this == &other)
        return *this;
    tmpDirPath = std::move(other.tmpDirPath);

    other.tmpDirPath.clear();
    return *this;
}

TempDir::~TempDir() {
    using namespace linuxdeploy::log;
    namespace fs = std::filesystem;

    if (tmpDirPath.empty())
        return;
    try {
        if (fs::remove_all(tmpDirPath) == 0) {
            throw fs::filesystem_error(
                "Tried to remove directory, but nothing was deleted (was the directory "
                "deleted already?).", {}
            );
        }
    }
    catch (const fs::filesystem_error & exc) {
        ldLog() << LD_WARNING << "Couldn't delete temporary directory " << tmpDirPath
            << ": " << exc.what() << std::endl;
    }
}

std::filesystem::path TempDir::path() const {
    return tmpDirPath;
}

std::map<std::string, std::string> queryQmake(const std::filesystem::path& qmakePath) {
    auto qmakeCall = subprocess({qmakePath.string(), "-query"}).run();

    using namespace linuxdeploy::log;

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

std::filesystem::path findQmake() {
    using namespace linuxdeploy::log;

    std::filesystem::path qmakePath;

    // allow user to specify absolute path to qmake
    if (getenv("QMAKE")) {
        qmakePath = linuxdeploy::util::which(getenv("QMAKE"));
        ldLog() << "Using user specified qmake:" << qmakePath << std::endl;
    } else {
        // search for qmake
        qmakePath = linuxdeploy::util::which("qmake-qt5");

        if (qmakePath.empty())
            qmakePath = linuxdeploy::util::which("qmake");

        if (qmakePath.empty())
            qmakePath = linuxdeploy::util::which("qmake6");
    }

    return qmakePath;
}

std::filesystem::path findQmlImportScanner() {
    using linuxdeploy::util::which;

    // Calling plain which("qmlimportscanner") is problematic, because it
    // is symlinked to qtchooser on some distros. qtchooser's Qt6 support
    // is less than ideal, qmlimportscanner used to be in
    // /usr/lib/qt5/bin/qmlimportscanner, but it was moved to
    // /usr/lib/qt6/libexec/qmlimportscanner in Qt6. qtchooser is capable
    // of checking only a single directory for executables at a time,
    // and it usually checks the bin/ one, so qmlimportscanner cannot
    // be executed on Qt6 (if you are flabbergasted by this, remember that
    // current latest release of qtchooser, 66_3, doesn't even include a
    // qt6 config lookup file).
    // Either way, QT_INSTALL_LIBEXECS/QT_INSTALL_BINS lookup is the more
    // robust solution.
    auto qmakeVars = queryQmake(findQmake());
    auto path = which(qmakeVars["QT_INSTALL_LIBEXECS"] + "/qmlimportscanner");
    if (path.empty())
        path = which(qmakeVars["QT_INSTALL_BINS"] + "/qmlimportscanner");
    if (path.empty())
        path = which("qmlimportscanner");

    return path;
}

std::filesystem::path findLconvert() {
    using linuxdeploy::util::which;

    // lconvert remained in bin/ dir even in Qt6 (it is not in libexec).
    auto qmakeVars = queryQmake(findQmake());
    auto path = which(qmakeVars["QT_INSTALL_BINS"] + "/lconvert");
    if (path.empty())
        path = which("lconvert");

    return path;
}

bool pathContainsFile(std::filesystem::path dir, std::filesystem::path file) {
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

std::string shellJoin(const std::vector<std::string> &arguments) {
    const auto &npos = std::string::npos;

    auto containsUnsafeCharacters = [](const std::string & arg){
        return arg.find_first_of(" \t$`\"'\\\n!") != npos;
    };

    std::string result;

    bool first = true;

    for (const std::string & arg : arguments) {
        if (!first)
            result += ' ';
        first = false;

        if (!containsUnsafeCharacters(arg)) {
            result.append(arg);
            continue;
        }

        result += "'";

        std::string::size_type start = 0, next;
        while ((next = arg.find('\'', start)) != npos) {
            result += arg.substr(start, next - start);
            result += R"--('"'"')--";
            start = next + 1;
        }
        result += arg.substr(start);
        result += "'";
    }

    return result;
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

bool isQtDebugSymbolFile(const std::string& filename) {
    // the official Qt build pipeline calls those <library name>.so.debug, so we just filter that suffix
    return strEndsWith(filename, ".debug");
}

bool isQtDebugSymbolFile(const std::filesystem::path& path) {
    return strEndsWith(path.filename().string(), ".debug");
}
