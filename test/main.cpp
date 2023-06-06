#include "bstream.h"

#include <libdrawable.hpp>
#include <drawable.hpp>
#include <drawabledictionary.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>

#include "json.hpp"

void LoadDrawable();
void LoadDrawableDictionary();
void DumpShaderData();

int main(int argc, char* argv[]) {
    //LoadDrawable();
    //LoadDrawableDictionary();
    DumpShaderData();
}

void LoadDrawable() {
    UDrawable* drw = ImportYdr("D:\\RedDead\\s_van_menuclipboard01.ydr");
    delete drw;
}

void LoadDrawableDictionary() {
    UDrawableDictionary* dict = ImportYdd("D:\\RedDead\\feet_tr1_001.ydd");
    delete dict;
}

void DumpShaderData() {
    std::filesystem::path rootDir = "D:\\RedDead\\ydr";
    std::filesystem::path shaderDir = "D:\\RedDead\\ydr\\shader";
    UDrawable* drawable = nullptr;

    uint64_t fileCount = 0;
    for (const auto& p : std::filesystem::directory_iterator(rootDir)) {
        if (std::filesystem::is_directory(p) || p.path().extension() != ".ydr") {
            continue;
        }

        try {
            drawable = ImportYdr(p.path().generic_u8string());

            // For each valid LOD...
            for (auto* lod : drawable->Lods) {
                if (lod == nullptr) {
                    continue;
                }

                // For each valid model...
                for (auto* model : lod->Models) {
                    if (model == nullptr) {
                        continue;
                    }

                    // For each valid geometry with a shader...
                    for (auto* geom : model->Geometries) {
                        if (geom == nullptr || geom->Shader == nullptr) {
                            continue;
                        }

                        // Grab shader and check if a JSON file already exists for it
                        UShader* shader = geom->Shader;
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

        delete drawable;
        drawable = nullptr;

        fileCount++;
        std::cout << fileCount << std::endl;
    }
}
