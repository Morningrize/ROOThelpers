#include "TChainManager.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <filesystem>
#include <set>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"

#include "ROOTObjectManager.h"

namespace ROOThelpers {

namespace {

bool inputFileExist(const std::string& filename) {
    return std::filesystem::is_regular_file(filename);
}

bool inputFileReadable(const TFile* file) {
    return file!=nullptr && file->IsOpen() && !file->IsZombie();
}

bool inputFileContainsTree(TFile* file,
                                   const std::string &tree_name) {
    if (file == nullptr){
        return false;
    }
    TTree *tree = nullptr;
    file->GetObject(tree_name.c_str(), tree);
    return tree != nullptr;
}

std::set<std::string> collectSubBranchNames(TBranch* branch){
    std::set<std::string> branch_names;
    if (branch == nullptr){
        throw std::invalid_argument("TChainManager: TBranch is null");
    }
    branch_names.insert(branch->GetName());
    auto* subbranches = branch->GetListOfBranches();
    if (subbranches==nullptr){
        throw std::runtime_error(
            "TChainManager: branch list is null for branch " + 
            std::string((branch->GetName() != nullptr) ? branch->GetName() : "<unknown>"));
    }

    for (auto* subbranch: *subbranches ){
        auto * subbranch_cast=dynamic_cast<TBranch*>(subbranch);
        if ( subbranch_cast == nullptr){
            throw std::runtime_error(
                "TChainManager: invalid entry while processing branch " +
                std::string((branch->GetName()!= nullptr) ? branch->GetName() : "<unknown>"));
        }
        branch_names.merge(collectSubBranchNames(subbranch_cast));
    }
    return branch_names;
}

std::set<std::string> collectBranchNames(TTree* tree){
    std::set<std::string> branch_names;
    if (tree == nullptr){
        throw std::invalid_argument("TChainManager: TTree is null");
    }
    auto* branches = tree->GetListOfBranches();
    if (branches==nullptr){
        throw std::runtime_error(
            "TChainManager: branch list is null for TTree");
    }
    for (auto* branch: *branches ){
        auto * branch_cast=dynamic_cast<TBranch*>(branch);
        if ( branch_cast == nullptr){
            throw std::runtime_error(
                "TChainManager: TBranch is null while processing TTree ");
        }
        branch_names.merge(collectSubBranchNames(branch_cast));
    }
    return branch_names;
}

bool validateTreeStructure(TTree* reference_tree,
                            TTree* candidate_tree){
    if (reference_tree == nullptr || candidate_tree == nullptr) {
        return false;
    }
    return collectBranchNames(reference_tree) == collectBranchNames(candidate_tree);
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

ROOTObjectManager<TChain> TChainManager::build() const {

    std::set<std::string> reference_branch_set;

    for (const auto &filename : files_){

        if (!inputFileExist(filename)){
            throw std::invalid_argument("TChainManager: input file does not exist or isn't a regular file: " + filename);
        }

        std::unique_ptr<TFile> file(TFile::Open(filename.c_str(), "READ"));
        if (!inputFileReadable(file.get())){
            throw std::invalid_argument("TChainManager: input file is not readable by ROOT: " + filename);
        }
        if (!inputFileContainsTree(file.get(), tree_name_)){
            throw std::invalid_argument("TChainManager: input file: " + filename + " does not contain a TTree named: " + tree_name_);
        }

        if (options_.validate_tree_structure){
            TTree *tree = nullptr;
            file->GetObject(tree_name_.c_str(), tree);

            if (reference_branch_set.empty()){
                reference_branch_set = collectBranchNames(tree);
            }
            else {
                auto branch_set = collectBranchNames(tree);
                if (reference_branch_set != branch_set){
                    throw std::invalid_argument(
                        "TChainManager: branch structure mismatch in file: " + filename);
                }
            }
        }
    }
    

    ROOTObjectManager<TChain> chain(tree_name_.c_str());
    for (const auto &filename : files_) {
        if (chain->Add(filename.c_str()) == 0){
            throw std::runtime_error(
                "TChainManager: failed to add input file to chain: " + filename);
        }
    }
    return chain;
}
} // namespace ROOThelpers


