#include "h360storage.h"

void H360Storage::begin() {
    nvs_flash_init();
}

bool H360Storage::saveConfig(const H360Config& cfg) {
    String json = cfg.toJson();
    size_t size = json.length() + 1;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;

    err = nvs_set_blob(handle, KEY, json.c_str(), size);
    if (err != ESP_OK) {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

bool H360Storage::loadConfig(H360Config& cfg) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;

    size_t required = 0;
    err = nvs_get_blob(handle, KEY, NULL, &required);
    if (err != ESP_OK || required == 0) {
        nvs_close(handle);
        return false;
    }

    char* buffer = (char*) malloc(required);
    if (!buffer) {
        nvs_close(handle);
        return false;
    }

    err = nvs_get_blob(handle, KEY, buffer, &required);
    nvs_close(handle);

    if (err != ESP_OK) {
        free(buffer);
        return false;
    }

    bool ok = cfg.fromJson(String(buffer));
    free(buffer);

    return ok;
}
