#include "TChainManager.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <filesystem>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"

namespace ROOThelpers {

namespace {

bool inputFileExist(const std::string &filename) {
    return std::filesystem::is_regular_file(filename);
}

bool inputFileReadable(const std::unique_ptr<TFile> &file) {
    return file && file->IsOpen() && !file->IsZombie();
}

bool inputFileContainsTree(const std::unique_ptr<TFile> &file,
                                   const std::string &tree_name) {
    if (!file) {
        return false;
    }

    TTree *tree = nullptr;
    file->GetObject(tree_name.c_str(), tree);
    return tree != nullptr;
}

}


TChainManager::TChainManager(std::vector<std::string> files, 
                                std::string tree_name, TChainBuildOptions options)
    : files_(std::move(files)), tree_name_(std::move(tree_name)), options_(options) {
        
    if (files_.empty()) {
        throw std::invalid_argument("TChainManager: input file list is empty");
    }

    for (const auto& file : files_) {
        if (file.empty()) {
            throw std::invalid_argument(
            "TChainManager: input file list contains an empty path");
        }
    }

    if (tree_name_.empty()) {
        throw std::invalid_argument("TChainManager: tree name is empty");
    }

    if (options_.duplicate_sample_count == 0 && options_.check_duplicates) {
        throw std::invalid_argument(
            "TChainManager: duplicate_sample_count must be > 0 when duplicate checking is enabled");
    }

    }

std::unique_ptr<TChain> TChainManager::build() const {
    for (const auto &filename : files_){

        if (!inputFileExist(filename)){
            throw std::invalid_argument("TChainManager: input file does not exist or isn't a regular file: " + filename);
        }
        
        std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
        if (!inputFileReadable(file)){
            throw std::invalid_argument("TChainManager: input file is not readable by ROOT: " + filename);
        }
        if (!inputFileContainsTree(file, tree_name_)){
            throw std::invalid_argument("TChainManager: input file: " + filename + " does not contain a TTree named: " + tree_name_);
        }
    }

    auto chain = std::make_unique<TChain>(tree_name_.c_str());
    for (const auto &filename : files_) {
        chain->Add(filename.c_str());
    }

    return chain;
}
} // namespace ROOThelpers