// system includes
#include <iostream>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

// library includes
#include <boost/filesystem.hpp>
#include <subprocess.hpp>
#include <args.hxx>
#include <linuxdeploy/core/log.h>

#pragma once

typedef struct {
    bool success;
    int retcode;
    std::string stdoutOutput;
    std::string stderrOutput;
} procOutput;

static procOutput check_command(const std::vector<std::string> &args) {
    auto command = subprocess::util::join(args);
    subprocess::Popen proc(command, subprocess::bufsize{100*1024*1024}, subprocess::output(subprocess::PIPE), subprocess::error(subprocess::PIPE));

    auto returnCode = proc.wait();

    std::string out;
    std::vector<char> buff(1024);
    while (fgets (buff.data(), 1024 , proc.output()) != NULL) {
        auto end = std::find(buff.begin(), buff.end(), '\0');
        out += std::string(buff.begin(), end);
    }

    std::string err;
    while (fgets (buff.data(), 1024 , proc.error()) != NULL) {
        auto end = std::find(buff.begin(), buff.end(), '\0');
        err += std::string(buff.begin(), end);
    }

    return {returnCode == 0, returnCode, out, err};
}

static boost::filesystem::path which(const std::string &name) {
    subprocess::Popen proc({"which", name.c_str()}, subprocess::output(subprocess::PIPE));
    auto output = proc.communicate();

    using namespace linuxdeploy::core::log;

    ldLog() << LD_DEBUG << "Calling 'which" << name << LD_NO_SPACE << "'" << std::endl;

    if (proc.retcode() != 0) {
        ldLog() << LD_DEBUG << "which call failed, exit code:" << proc.retcode() << std::endl;
        return "";
    }

    std::string path = output.first.buf.data();

    while (path.back() == '\n') {
        path.erase(path.end() - 1, path.end());
    }

    return path;
}

template<typename Iter>
std::string join(Iter beg, Iter end) {
    std::stringstream rv;

    if (beg != end) {
        rv << *beg;

        for_each(++beg, end, [&rv](const std::string &s) {
            rv << " " << s;
        });
    }

    return rv.str();
}

static std::map<std::string, std::string> queryQmake(const boost::filesystem::path& qmakePath) {
    auto qmakeCall = check_command({qmakePath.string(), "-query"});

    using namespace linuxdeploy::core::log;

    if (!qmakeCall.success) {
        ldLog() << LD_ERROR << "Call to qmake failed:" << qmakeCall.stderrOutput << std::endl;
        return {};
    }

    std::map<std::string, std::string> rv;

    std::stringstream ss;
    ss << qmakeCall.stdoutOutput;

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

static boost::filesystem::path findQmake() {
    using namespace linuxdeploy::core::log;

    boost::filesystem::path qmakePath;

    // allow user to specify absolute path to qmake
    if (getenv("QMAKE")) {
        qmakePath = getenv("QMAKE");
        ldLog() << "Using user specified qmake:" << qmakePath << std::endl;
    } else {
        // search for qmake
        qmakePath = which("qmake-qt5");

        if (qmakePath.empty())
            qmakePath = which("qmake");
    }

    return qmakePath;
}

static bool pathContainsFile(boost::filesystem::path dir, boost::filesystem::path file) {
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

