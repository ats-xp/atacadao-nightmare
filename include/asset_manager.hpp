#pragma once

#include <filesystem>

#include "base.h"

#include "sokol_gfx.h"

struct Texture;

namespace AssetManager {

void init();

// Textures
Texture loadTexture(const char *filename, u8 tex_type);
void addTexture(const char *path);
void addTextureOnThread(const char *path);
void destroyTexture(std::string id);

bool isTexture(const std::string &id);

const u16 &getTextureRequests(void);
Texture &getTextureFromID(const std::string &id);
const std::string getTextureIDFromPath(const std::string &path);
std::filesystem::path &getRoot();
const std::string getPath(const std::string &path);

void setTextureFilter(sg_filter min, sg_filter max);
void setTextureWrap(sg_wrap u, sg_wrap v);
void setTextureVerticalFlip(bool flip);

}; // namespace AssetManager
