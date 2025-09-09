#include "asset_manager.hpp"
#include "mesh.hpp"

#include "sokol_fetch.h"
#include "stb_image.h"

#include <unordered_map>

static struct TextureInfo {
  std::unordered_map<std::string, Texture> hash;

  sg_filter min_filter;
  sg_filter max_filter;

  sg_wrap wrap_u;
  sg_wrap wrap_v;

  bool flip_uv;

  u16 total_requests; // tmp
  u16 total;
} texture_info;

static u8 io_texture_buffer[(1024 * 1024) * 1024];

static std::filesystem::path root;

static void initTextures() {
  texture_info.min_filter = SG_FILTER_LINEAR;
  texture_info.max_filter = SG_FILTER_LINEAR;

  texture_info.wrap_u = SG_WRAP_REPEAT;
  texture_info.wrap_v = SG_WRAP_REPEAT;

  AssetManager::addTexture("assets/textures/default.png");
}

static void responseCallback(const sfetch_response_t *response) {
  if (response->fetched) {
    const int *data = (const int *)response->user_data;

    AssetManager::setTextureFilter((sg_filter)data[0], (sg_filter)data[1]);
    AssetManager::setTextureWrap((sg_wrap)data[2], (sg_wrap)data[3]);
    AssetManager::setTextureVerticalFlip(data[4]);
    AssetManager::addTexture(response->path);
  }

  if (response->finished) {
    if (response->failed) {
      LogError("Load Texture Failed: %s | %d", response->path,
               response->error_code);
    }

    texture_info.total_requests--;
  }
}

std::filesystem::path &AssetManager::getRoot() { return root; }
const std::string AssetManager::getPath(const std::string &path) {
  return root / path;
}

void AssetManager::init() {
  // Linux
  char exe_path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", exe_path, PATH_MAX);
  (void)count;
  root = exe_path;
  root = root.parent_path() / "../";

  initTextures();
}

Texture AssetManager::loadTexture(const char *filename, u8 tex_type) {
  Texture t = {};
  std::string f = std::string(filename);
  t.path = f.substr(f.find_last_of("/") + 1).c_str();

  t.type = static_cast<TextureType>(tex_type);

  int w, h, nch;
  stbi_uc *data =
      stbi_load((root / filename).string().c_str(), &w, &h, &nch, 4);

  if (data) {
    LogInfo("Loading texture: %s", filename);

    {
      sg_image_desc desc = {};
      desc.type = SG_IMAGETYPE_2D;
      desc.width = w;
      desc.height = h;
      desc.pixel_format = SG_PIXELFORMAT_RGBA8;
      desc.data.subimage[0][0].ptr = data;
      desc.data.subimage[0][0].size = w * h * 4;
      t.img = sg_make_image(&desc);
    }

    {
      sg_sampler_desc desc = {};
      desc.min_filter = texture_info.min_filter;
      desc.mag_filter = texture_info.max_filter;
      desc.wrap_u = texture_info.wrap_u;
      desc.wrap_v = texture_info.wrap_v;
      t.smp = sg_make_sampler(&desc);
    }

    stbi_image_free(data);
    return t;
  } else {
    LogError("Texture loading error %s file: %s", stbi_failure_reason(),
             filename);
  }

  stbi_image_free(data);
  return t;
}

void AssetManager::addTexture(const char *path) {
  std::string id = getTextureIDFromPath(path);
  if (id == "default" && AssetManager::isTexture("default"))
    return;

  Texture tex = AssetManager::loadTexture(path, TextureType::DIFFUSE);

  texture_info.hash.insert({id, tex});
  texture_info.total++;
}

void AssetManager::addTextureOnThread(const char *path) {
  int data[5] = {texture_info.min_filter, texture_info.max_filter,
                 texture_info.wrap_u, texture_info.wrap_v,
                 texture_info.flip_uv};

  sfetch_request_t desc = {};

  std::string p = root / path;

  desc.path = p.c_str();
  desc.callback = responseCallback;
  desc.buffer = SFETCH_RANGE(io_texture_buffer);
  desc.user_data = SFETCH_RANGE(data);
  sfetch_send(&desc);

  texture_info.total_requests++;
}

void AssetManager::destroyTexture(std::string id) {
  Texture &t = texture_info.hash.at(id);
  t.destroy();
  texture_info.hash.erase(id);
}

void AssetManager::setTextureFilter(sg_filter min, sg_filter max) {
  texture_info.min_filter = min;
  texture_info.max_filter = max;
}

void AssetManager::setTextureWrap(sg_wrap u, sg_wrap v) {
  texture_info.wrap_u = u;
  texture_info.wrap_v = v;
}

void AssetManager::setTextureVerticalFlip(bool flip) {
  texture_info.flip_uv = flip;
  stbi_set_flip_vertically_on_load(flip);
}

const std::string AssetManager::getTextureIDFromPath(const std::string &path) {
  std::string id = path;
  id = id.substr(id.find_last_of("/") + 1);
  id = id.substr(0, id.find_first_of("."));
  return id;
}

const u16 &AssetManager::getTextureRequests(void) {
  return texture_info.total_requests;
}

Texture &AssetManager::getTextureFromID(const std::string &id) {
  if (!texture_info.hash.contains(id)) {
    // LogError("Texture ID not founded in hash map: %s", id.c_str());
    return texture_info.hash.at("default");
  }
  return texture_info.hash.at(id);
}

bool AssetManager::isTexture(const std::string &id) {
  return texture_info.hash.contains(id);
}
