// system headers
#include <filesystem>
#include <cassert>

// library includes
#include <linuxdeploy/log/log.h>
#include <linuxdeploy/util/util.h>
#include <linuxdeploy/subprocess/subprocess.h>

// local includes
#include "translation-deploymant.h"
#include "util.h"

struct TranslationInfo {
    linuxdeploy::core::appdir::AppDir *appDir;
    // Something like /usr/share/qt6/translations
    std::filesystem::path qtTranslationsPath;
    // Dest path in appDir
    std::filesystem::path appDirTranslationsPath;
    TranslationDeploymentType deploymentType;
    // Something like qtbase, qtmultimedia, ...
    std::vector<std::string> knownQmPrefixes;
    // Qt languages to install; install everything if empty
    std::unordered_set<std::string> languages;
    TempDir * tempDir;

    static std::vector<std::string>
    getKnownQmPrefixes(const std::vector<QtModule> & modules) {
        std::unordered_set<std::string> knownPrefixes;

        for (const QtModule &module : modules) {
            if (module.translationFilePrefix.empty())
                continue;
            knownPrefixes.insert(module.translationFilePrefix);
        }

        return std::vector(knownPrefixes.begin(), knownPrefixes.end());
    }
};

struct QmFileInfo {
    std::string libName;
    std::string language;

    bool isValid() const {
        return !libName.empty() && !language.empty();
    }
};

struct TranslationData {
	// All library translations encountered so far (like qtbase, qtmultimedia...).
	// Needed when merging .qm into qt_??.qm.
	std::unordered_set<std::string> usedTranslatedLibs;
	// Needed when merging .qm into qt_??.qm.
	std::unordered_map<
	    std::string, /* language (like cs, pt_BR...) */
        std::vector<std::filesystem::path> /* qm file paths (like qtbase_cs.qm, qtmultimedia_cs.qm) */
    > lang2TranslationMapping;
};

struct TranslationError : public std::exception {
    using std::exception::exception;
};

static bool
isValidTranslationFile(const std::string &fileName) {
    if (fileName.empty())
        return false;
    if (!strEndsWith(fileName, ".qm"))
        return false;
    return true;
}

static bool
isValidTranslationFile(const std::string &fileName, const std::string &prefix) {
    if (!isValidTranslationFile(fileName))
        return false;

    if (!strStartsWith(fileName, prefix))
        return false;
    //    qtbase     _     cs    .qm
    //    |^^^^^     |     |^    |^^
    // prefix.size() 1     |     3
    //  min 2 (with extra specifier, like pt_BR 5)
    if (fileName.size() < (prefix.size() + 1 + 2 + 3))
        return false;
    if (fileName[prefix.size()] != '_')
        return false;
    return true;
}

static QmFileInfo
getModuleTranslation(const std::string & fileName, const std::vector<std::string> &knownQmPrefixes) {
    QmFileInfo result;
    for (const std::string &translationFilePrefix : knownQmPrefixes) {
        if (translationFilePrefix.empty() || !strStartsWith(fileName, translationFilePrefix))
            continue;
        result.libName = translationFilePrefix;

        // We assume filename is reasonable, since it was checked by
        // isValidTranslationFile().

        auto prefixUnderscoreLen = translationFilePrefix.size() + 1;

        // 3 = .qm
        result.language = fileName.substr(prefixUnderscoreLen,
            fileName.size() - prefixUnderscoreLen - 3);

        return result;
    }
    return result;
}

static void
deployTranslationsQtWalkTrDir(const TranslationInfo &ti, TranslationData &translationData) {
	namespace fs = std::filesystem;
    for (fs::directory_iterator i(ti.qtTranslationsPath); i != fs::directory_iterator(); ++i) {
        if (!fs::is_regular_file(*i))
            continue;

        std::string fileName = i->path().filename().string();

        if (!isValidTranslationFile(fileName))
            continue;

        QmFileInfo moduleTranslation =
            getModuleTranslation(fileName, ti.knownQmPrefixes);

        if (moduleTranslation.isValid()) {
            if (!ti.languages.empty() && ti.languages.count(moduleTranslation.language))
                continue;

            translationData.usedTranslatedLibs.insert(moduleTranslation.libName);

            auto & lang2TranslationMapping = translationData.lang2TranslationMapping;

            auto langMapping = lang2TranslationMapping.find(moduleTranslation.language);
            if (langMapping == lang2TranslationMapping.end()) {
                lang2TranslationMapping.try_emplace(
                    moduleTranslation.language,
                    std::vector<fs::path>{i->path()}
                );
            } else {
                langMapping->second.push_back(i->path());
            }
            if (ti.deploymentType & TranslationDeployment::individual)
                ti.appDir->deployFile(*i, ti.appDirTranslationsPath);
        }
    }
}

static void
handleLconvertError(int exitStatus, const std::string & errMsg,
  const std::filesystem::path & lconvertExe, const std::vector<std::filesystem::path> & toMerge)
{
    using namespace linuxdeploy::log;

    std::vector<std::string> files;
    files.reserve(toMerge.size());

    std::transform(
        toMerge.cbegin(), toMerge.cend(), std::back_inserter(files),
        [](const std::filesystem::path & path){ return path.string(); }
    );

    ldLog() << LD_ERROR << "Executing '" << lconvertExe << "' to merge translation files "
        << join(files) << " has failed with exit status " << exitStatus << ": " << errMsg;
}

static void
deployTranslationsQtRunLconvert(const TranslationInfo &ti, const TranslationData &translationData) {
    using namespace linuxdeploy::log;
    using namespace linuxdeploy::subprocess;
	namespace fs = std::filesystem;

    fs::path lconvert = findLconvert();

    if (lconvert.empty()) {
        ldLog() << LD_ERROR << "Could not find 'lconvert' exe to compile qt_??.qm translations!";
        throw TranslationError();
    }

    ti.tempDir->create("linuxdeploy-plugin-qt-lconvert-merged-qm");

    std::vector<std::string> cmdline;

    for (const auto &[language, files] : translationData.lang2TranslationMapping) {
        cmdline.clear();
        cmdline.push_back(lconvert.string());
        cmdline.push_back("-input-format");
        cmdline.push_back("qm");

        for (const std::string &qmFile : files) {
            cmdline.push_back("-input-file");
            cmdline.push_back(qmFile);
        }
        cmdline.push_back("-output-file");
        std::string outputFilename = "qt_" + language + ".qm";
        fs::path outputPath = ti.tempDir->path() / outputFilename;
        cmdline.push_back(outputPath);

        ldLog() << LD_INFO << "Running lconvert:" << shellJoin(cmdline) << std::endl;

        auto result = subprocess(cmdline).run();

        if (result.exit_code() != 0) {
            handleLconvertError(result.exit_code(), result.stderr_string(), lconvert, files);
            throw TranslationError();
        }
        ti.appDir->deployFile(outputPath.string(), ti.appDirTranslationsPath);
    }
}

static void
deployTranslationsQt(const TranslationInfo &ti) {
    using namespace linuxdeploy::log;

	TranslationData translationData;

    deployTranslationsQtWalkTrDir(ti, translationData);

    if (translationData.usedTranslatedLibs.size() == 0) {
        ldLog() << LD_WARNING << "No translations found in " << ti.qtTranslationsPath
            << ", skipping deployment";
        return;
    }

    // TranslationDeployment::individual was already fulfilled above, if we do not
    // need merged qt_??.qm files, we can end here.
    if ((ti.deploymentType & TranslationDeployment::merged) == 0)
        return;

    if (translationData.usedTranslatedLibs.size() == 1) {
        // No need to merge .qm files when there's just one per language.
        for (const auto &[language, translationFiles] : translationData.lang2TranslationMapping) {
            assert(translationFiles.size() == 1);
            std::string destFileName = "qt_" + language + ".qm";
            auto destPath = ti.appDirTranslationsPath / destFileName;
            ti.appDir->deployFile(translationFiles.front().string(), destPath);
        }

        return;
    }

    // We need to lconvert multiple .qm files into single qt_??.qm file.
    deployTranslationsQtRunLconvert(ti, translationData);
}

static void
deployTranslationsApp(const TranslationInfo &ti) {
	namespace fs = std::filesystem;

	bool checkTranslationsDirExistance = false;

    for (auto& i : fs::recursive_directory_iterator(ti.appDir->path())) {
        if (!fs::is_regular_file(i) || pathContainsFile(ti.appDirTranslationsPath, i))
            continue;

        const auto fileName = i.path().filename();

        if (strEndsWith(fileName.string(), ".qm")) {
            if (!checkTranslationsDirExistance) {
                if (!fs::is_directory(ti.appDirTranslationsPath)) {
                    // Symlink below fails if directory doesn't exist, which can very
                    // well happen, since the .qm file deployments are deferred.
                    fs::create_directories(ti.appDirTranslationsPath);
                }
                checkTranslationsDirExistance = true;
            }
            ti.appDir->createRelativeSymlink(i, ti.appDirTranslationsPath / fileName);
        }
    }
}

bool
deployTranslations(linuxdeploy::core::appdir::AppDir &appDir, const std::filesystem::path &qtTranslationsPath,
  const std::vector<QtModule> &modules, TranslationDeploymentType deploymentType,
  const std::vector<std::string> &languages, TempDir & tmpDir)
{
    using namespace linuxdeploy::log;
    using namespace linuxdeploy::util::misc;
	namespace fs = std::filesystem;

    if (qtTranslationsPath.empty() || !fs::is_directory(qtTranslationsPath)) {
        ldLog() << LD_WARNING << "Translation directory does not exist, skipping deployment";
        return true;
    }

    ldLog() << "Qt translations directory:" << qtTranslationsPath << std::endl;

    assert(deploymentType != 0);

    TranslationInfo translationInfo;
    translationInfo.appDir = &appDir;
    translationInfo.appDirTranslationsPath = appDir.path() / "usr/translations/";
    translationInfo.qtTranslationsPath = qtTranslationsPath;
    translationInfo.deploymentType = deploymentType;
    translationInfo.knownQmPrefixes = TranslationInfo::getKnownQmPrefixes(modules);
    translationInfo.languages.insert(languages.begin(), languages.end());
    translationInfo.tempDir = &tmpDir;

    if (deploymentType & (TranslationDeployment::individual | TranslationDeployment::merged)) {
        try {
            deployTranslationsQt(translationInfo);
        }
        catch (const TranslationError &) {
            return false;
        }
    }

    if (deploymentType & TranslationDeployment::user_symlink) {
        deployTranslationsApp(translationInfo);
    }

    return true;
}
