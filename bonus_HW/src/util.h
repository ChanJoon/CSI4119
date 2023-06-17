#ifndef UTIL_H
#define UTIL_H

#include "common.h"
#include <map>
#include <memory>  // unique_ptr

// CIFAR10 Utils
std::map<int, std::string> get_label_dict();
// Image Utils
bool read_image(const std::string data_path, const int index, const int batch,
                uint8_t* const image, int* const label);

#endif
