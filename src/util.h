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

static procOutput check_command(const std::vector<std::string>& args) {
    subprocess::Popen proc(subprocess::util::join(args), subprocess::output(subprocess::PIPE), subprocess::error(subprocess::PIPE));

    auto output = proc.communicate();

    std::string out, err;

    if (output.first.length > 0)
        out = output.first.buf.data();

    if (output.second.length > 0)
        err = output.second.buf.data();

    return {proc.retcode() == 0, proc.retcode(), out, err};
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
