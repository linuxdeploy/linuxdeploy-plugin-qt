#pragma once

// library includes
#include <linuxdeploy/core/appdir.h>

// local includes
#include "qt-modules.h"
#include "util.h"

namespace TranslationDeployment
{
    enum TranslationDeploymentEnum
    {
        // Copy over individual Qt library .qm files.
        individual = 1 << 0,
        // Copy/Concatenate a full Qt qt_??.qm file.
        merged = 1 << 1,
        // Add a symlink to user .qm files to Qt's translation dir.
        user_symlink = 1 << 2,
    };
}
using TranslationDeploymentType = std::underlying_type_t<TranslationDeployment::TranslationDeploymentEnum>;

bool
deployTranslations(linuxdeploy::core::appdir::AppDir &appDir, const std::filesystem::path &qtTranslationsPath,
  const std::vector<QtModule> &modules, TranslationDeploymentType deploymentType,
  const std::vector<std::string> &languages, TempDir & tmpDir
);
