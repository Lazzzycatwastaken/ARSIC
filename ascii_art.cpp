#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ascii_art.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <cstring>

namespace ascii_art {

Interpreter::Interpreter(const Config& config) : config_(config) {}

std::string Interpreter::convert(const Image& image) {
    if (image.data.empty() || image.width <= 0 || image.height <= 0) {
        throw std::invalid_argument("Invalid image data");
    }
    
    int target_width = config_.target_width;
    int target_height = config_.target_height;
    
    if (config_.maintain_aspect && target_height == 0) {
        target_height = static_cast<int>(target_width * image.height * config_.char_aspect_ratio / image.width);
    }
    
    Image processed_image = resize_image(image, target_width, target_height);
    
    // Process image (im not doing dithering now because ughghhggg)
    
    std::string result;
    result.reserve((target_width + 1) * target_height);
    
    std::string charset = get_charset();

    for (int y = 0; y < target_height; ++y) {
        for (int x = 0; x < target_width; ++x) {
            float luminance;
            uint8_t r = 0, g = 0, b = 0;

            if (processed_image.channels >= 3) {
                r = get_pixel_value(processed_image, x, y, 0);
                g = get_pixel_value(processed_image, x, y, 1);
                b = get_pixel_value(processed_image, x, y, 2);
                luminance = get_luminance(r, g, b);
            } else {
                luminance = get_pixel_value(processed_image, x, y, 0) / 255.0f;
                r = g = b = static_cast<uint8_t>(luminance * 255.0f);
            }

            if (config_.use_gamma_correction) {
                luminance = apply_gamma_correction(luminance);
            }

            luminance = std::clamp(luminance * config_.contrast + config_.brightness, 0.0f, 1.0f);

            luminance = apply_perceptual_mapping(luminance);

            char ch = map_intensity_to_char(luminance);

            if (config_.use_color) {
                // Use 24-bit foreground color ANSI escape
                result += get_color_escape_code(r, g, b);
                result.push_back(ch);
                result += "\x1b[0m"; // reset
            } else {
                result.push_back(ch);
            }
        }
        result += '\n';
    }
    
    return result;
}


std::string Interpreter::convert_from_file(const std::string& filename) {
    std::string extension = filename.substr(filename.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "ppm") {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        std::string magic;
        int width, height, max_val;
        file >> magic >> width >> height >> max_val;
        file.ignore();
        if (magic != "P6") {
            throw std::runtime_error("Unsupported PPM format");
        }
        Image image(width, height, 3);
        file.read(reinterpret_cast<char*>(image.data.data()), image.data.size());
        return convert(image);
    } else {
        int width, height, channels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + filename);
        }
        Image image(width, height, 3);
        std::memcpy(image.data.data(), data, width * height * 3);
        stbi_image_free(data);
        return convert(image);
    }
}

void Interpreter::set_mode(Mode mode) {
    config_.mode = mode;
}

void Interpreter::set_target_size(int width, int height) {
    config_.target_width = width;
    config_.target_height = height;
}

void Interpreter::set_contrast(float contrast) {
    config_.contrast = contrast;
}

void Interpreter::set_brightness(float brightness) {
    config_.brightness = brightness;
}

void Interpreter::set_color(bool use_color) {
    config_.use_color = use_color;
}

float Interpreter::apply_gamma_correction(float value) const {
    if (value <= 0.0f) return 0.0f;
    if (value >= 1.0f) return 1.0f;
    return std::pow(value, 1.0f / config_.gamma);
}

float Interpreter::apply_perceptual_mapping(float intensity) const {
    float x = intensity;
    return std::clamp(3.0f * x * x - 2.0f * x * x * x, 0.0f, 1.0f);
}

std::string Interpreter::get_color_escape_code(uint8_t r, uint8_t g, uint8_t b) const {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "\x1b[38;2;%u;%u;%um", r, g, b);
    return std::string(buf);
}

std::string Interpreter::get_charset() const {
    switch (config_.mode) {
        case Mode::CLEAN:
            return " .:-=+*#%@";
            
        case Mode::HIGH_FIDELITY:
            return " .'`^\",:;Il!i><~+_-?][}{1)(|\\tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
            
        case Mode::BLOCK:
            return " ░▒▓█";
    }
    return " .:-=+*#%@";
}

float Interpreter::get_luminance(uint8_t r, uint8_t g, uint8_t b) const {
    return 0.299f * r / 255.0f + 0.587f * g / 255.0f + 0.114f * b / 255.0f;
}

char Interpreter::map_intensity_to_char(float intensity) const {
    std::string charset = get_charset();
    int index = static_cast<int>(intensity * (charset.length() - 1));
    index = std::clamp(index, 0, static_cast<int>(charset.length() - 1));
    return charset[index];
}

Image Interpreter::resize_image(const Image& image, int new_width, int new_height) const {
    Image resized(new_width, new_height, image.channels);
    
    float x_ratio = static_cast<float>(image.width) / new_width;
    float y_ratio = static_cast<float>(image.height) / new_height;
    
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            int src_x = static_cast<int>(x * x_ratio);
            int src_y = static_cast<int>(y * y_ratio);
            
            src_x = std::clamp(src_x, 0, image.width - 1);
            src_y = std::clamp(src_y, 0, image.height - 1);
            
            for (int c = 0; c < image.channels; ++c) {
                int src_index = (src_y * image.width + src_x) * image.channels + c;
                int dst_index = (y * new_width + x) * image.channels + c;
                resized.data[dst_index] = image.data[src_index];
            }
        }
    }
    
    return resized;
}



uint8_t Interpreter::get_pixel_value(const Image& image, int x, int y, int channel) const {
    int index = (y * image.width + x) * image.channels + channel;
    return image.data[index];
}

}