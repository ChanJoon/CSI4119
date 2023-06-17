#include "util.h"

/*
Summary:
    Return label dict
*/
std::map<int, std::string> get_label_dict() {
  std::map<int, std::string> label_dict;
  label_dict.insert(std::pair<int, std::string>(0, "airplane"));
  label_dict.insert(std::pair<int, std::string>(1, "automobile"));
  label_dict.insert(std::pair<int, std::string>(2, "bird"));
  label_dict.insert(std::pair<int, std::string>(3, "cat"));
  label_dict.insert(std::pair<int, std::string>(4, "deer"));
  label_dict.insert(std::pair<int, std::string>(5, "dog"));
  label_dict.insert(std::pair<int, std::string>(6, "frog"));
  label_dict.insert(std::pair<int, std::string>(7, "horse"));
  label_dict.insert(std::pair<int, std::string>(8, "ship"));
  label_dict.insert(std::pair<int, std::string>(9, "truck"));
  return label_dict;
}

/*
Summary:
    Read image from data file, this is only working for CIFAR-10
Parameters:
    data_path: (input) path to data, e.g. data/test_batch.bin
    index: (input) index of row, e.g. 0 ~ 9999
    index: (input) batch size, e.g. 0 ~ 9999
    image: (output) loaded image
    label: (output) loaded label, check data/batches.meta.txt for mapping info
*/
bool read_image(const std::string data_path, const int index, const int batch,
                uint8_t* const image, int* const label) {
  // Open file
  std::ifstream file;
  file.open(data_path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file) {
    std::cout << "Error opening file: " << data_path << std::endl;
    return false;
  }
  // Read byte stream
  auto file_size = file.tellg();
  std::unique_ptr<char[]> buffer(new char[file_size]);
  file.seekg(0, std::ios::beg);
  file.read(buffer.get(), file_size);
  file.close();
  // Copy head for buffer
  int head = index * ROW_SIZE;
  for (int b = 0; b < batch; b++) {
    label[b] = buffer[head++];
    for (int c = 0; c < IMG_CHANNEL; c++)
      for (int i = 0; i < IMG_HEIGHT; i++)
        for (int j = 0; j < IMG_WIDTH; j++) {
          image[b * (IMG_CHANNEL * IMG_WIDTH * IMG_HEIGHT) +
                c * (IMG_WIDTH * IMG_HEIGHT) + i * (IMG_WIDTH) + j] =
              buffer[head++];
        }
  }
  return true;
}
