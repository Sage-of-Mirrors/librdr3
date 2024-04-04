#include "util/bstream.h"

#include <librdr3.hpp>
#include <drawable/drawable.hpp>
#include <drawable/drawabledictionary.hpp>
#include <navmesh/navmeshdata.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>

#include "json.hpp"

void LoadDrawable();
void LoadDrawableDictionary();
void LoadNavmesh();
void DumpShaderData();
void DumpNavmeshes();

int main(int argc, char* argv[]) {
    //LoadDrawable();
    //LoadDrawableDictionary();
    LoadNavmesh();
    //DumpShaderData();
    //DumpNavmeshes();
}

void LoadDrawable() {
    std::shared_ptr<CDrawable> drw = librdr3::ImportYdr("D:\\RedDead\\miy_fort.ydr");
    librdr3::ExportYdr("D:\\RedDead\\ydr_export_test.ydr", drw);
}

void LoadDrawableDictionary() {
    std::shared_ptr<CDrawableDictionary> dict = librdr3::ImportYdd("D:\\RedDead\\feet_tr1_001.ydd");
}

void LoadNavmesh() {
    std::shared_ptr<CNavmeshData> nv = librdr3::ImportYnv("D:\\RedDead\\Navmesh\\nav_std_train_station_es_navmesh[366][291].ynv");
    bStream::CFileStream st("D:\\RedDead\\Navmesh\\test.ynv", bStream::Little, bStream::Out);
    nv->Serialize(&st);
}

void DumpShaderData() {
    std::filesystem::path rootDir = "D:\\RedDead\\ydr";
    std::filesystem::path shaderDir = "D:\\RedDead\\ydr\\shader";

    uint64_t fileCount = 0;
    for (const auto& p : std::filesystem::directory_iterator(rootDir)) {
        if (std::filesystem::is_directory(p) || p.path().extension() != ".ydr") {
            continue;
        }

        std::cout << p.path() << std::endl;
        std::shared_ptr<CDrawable> drawable;

        try {
            drawable = librdr3::ImportYdr(p.path().generic_u8string());

            if (drawable == nullptr) {
                continue;
            }

            // For each valid LOD...
            for (auto lod : drawable->Lods) {
                if (lod == nullptr) {
                    continue;
                }

                // For each valid model...
                for (auto model : lod->Models) {
                    if (model == nullptr) {
                        continue;
                    }

                    // For each valid geometry with a shader...
                    for (auto geom : model->Geometries) {
                        if (geom == nullptr || geom->Shader == nullptr) {
                            continue;
                        }

                        // Grab shader and check if a JSON file already exists for it
                        std::shared_ptr<CShader> shader = geom->Shader;
                        std::filesystem::path shaderTemplatePath = shaderDir / (shader->Name + ".json");
                        if (std::filesystem::exists(shaderTemplatePath)) {
                            continue;
                        }

                        // Create JSON object for shader template
                        nlohmann::json shaderInfo;
                        shaderInfo["name"] = shader->Name;
                        shaderInfo["hash"] = shader->Hash;

                        for (int i = 0; i < shader->Uniforms.size(); i++) {
                            shaderInfo["uniforms"][i]["name"] = shader->Uniforms[i]->Name;
                            shaderInfo["uniforms"][i]["hash"] = shader->Uniforms[i]->Hash;
                            shaderInfo["uniforms"][i]["size"] = shader->Uniforms[i]->Data.size();

                            for (int j = 0; j < shader->Uniforms[i]->Data.size(); j++) {
                                shaderInfo["uniforms"][i]["default_value"][j] = 0.0f;
                            }
                        }

                        // Output template JSON to file
                        {
                            std::ofstream shaderTemplate(shaderTemplatePath);
                            shaderTemplate << std::setw(4) << shaderInfo << std::endl;
                            shaderTemplate.close();
                        }
                    }
                }
            }
        }
        catch (std::exception e) {
            std::cout << "Failed to properly load YDR " << p.path().filename() << std::endl;
        }

        fileCount++;
    }
}

void DumpNavmeshes() {
    std::filesystem::path rootDir = "D:\\RedDead\\Navmesh";
    std::filesystem::path objDir = "D:\\RedDead\\Navmesh\\obj_single";

    uint64_t fileCount = 0;
    for (const auto& p : std::filesystem::directory_iterator(rootDir)) {
        if (std::filesystem::is_directory(p) || p.path().extension() != ".ynv") {
            continue;
        }

        std::cout << p.path() << std::endl;
        try {
            std::shared_ptr<CNavmeshData> data = librdr3::ImportYnv(p.path().generic_u8string());
            if (data == nullptr) {
                continue;
            }

            //std::filesystem::path dumpPath = objDir / p.path().stem();
            //dumpPath.replace_extension(".obj");
            //data->Debug_DumpToObj(dumpPath.string());
        }
        catch (std::exception e) {
            std::cout << "Failed to properly load YNV " << p.path().filename() << std::endl;
        }

        fileCount++;
    }
}
