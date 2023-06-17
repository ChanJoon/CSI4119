# Bonux_HW: VGG with CPU and CUDA

## 0 Introduction

This is bonus HW for 'Multi-core and GPU Programming 2023 Spring' at Yonsei University. 

- Professor: [Yongjun Park](https://sites.google.com/view/asolabysu/home)
- Teaching Assistant: Taehyeong Park
- Teaching Assistant: Hyejun Kim

## 1 Related Things

### 1-1 VGG16 Architectures

There are 13 convolution layers and 4 max-pooling layers. Then one fully connected layer are applied. You will implement these layers in CUDA version.

![VGG16-architecture.jpg](image/vgg16.png)

### 1-2 CIFAR 10

The CIFAR-10 dataset consists of 60000 32x32 colour images in 10 classes, with 6000 images per class. You will use 10000 test images to inference the class of image.

![cifar10_samples.PNG](image/cifar10_samples.PNG)

## 2 Explanation about the structure of skeleton code

```bash
user@acsys:/HW6/CUDA_VGG$ tree .
.
├── build                         # objective file will be there
│                                 
├── image                         # Images used by README.md
│   ├── cifar10_samples.PNG       # CIFAR-10 samples
│   ├── vgg16.PNG				  # VGG16-architecture
│   └── LeNet5-architecture.jpg   # LeNet5-architecture 
├── Makefile                      # Makefile
├── model                         # Provided pre-trained model
│   └── main_vgg.py               # Used code to train
│                                 # test accuracy 81%
├── README.md                     # What you are reading :3
├── src                           # Source Code
    ├── common.h                  # Common
    ├── vgg16.h                   # Base VGG16
    ├── vgg16.cpp
    ├── vgg16_cpu.h               # CPU VGG16
    ├── vgg16_cpu.cpp
    ├── vgg16_cuda.h              # CUDA VGG16
    ├── vgg16_cuda.cu
    ├── main.cpp                  # main program
    ├── util.h                    # Util
    └── util.cpp

4 directories, 15 files
```

`./predict help` will print followings description about arguments.(You should compile first!)

```bash
user@acsys:~/bonus_HW/$ ./predict help
[ERROR] Invalid arguments
Usage: ./predict INPUT_PATH DATA_OFFSET BATCH IMG_PATH_TEMPLATE
    INPUT_PATH: path to input data, e.g. /vgg_data/cifar10/test_batch.bin
    DATA_OFFSET: data_offset for input data, e.g. 0
    BATCH: batch size to inference, e.g. 1
    PARAMETER_PATH: path to parameter, e.g. /vgg_data/vgg_weight/vgg_values.txt
```

## 3 What you have todo

### Step 1: Check the data

!!!!!Don't write or update the data!!!!!

```bash
user@acsys:~/bonus_HW/$ ls /vgg_data/cifar10/
batches.meta.txt  data_batch_2.bin  data_batch_4.bin  download_cifar10.sh  test_batch.bin
data_batch_1.bin  data_batch_3.bin  data_batch_5.bin  readme.html
```

### Step 2: Implement vgg16_cuda

Implement `normalize`, `conv`, `relu`, `pool`, `fc` with CUDA at `src/vgg16_cuda.cu`. There is the naive(1 thread, 1 block) implementation already.

You should use 32 bit float or 64 bit double on CUDA. In other words, you can't use 8 bit, 16 bit quantization. 
Also, `tensorcore` is not allowed on this homework.

You should NOT modify any of main.cpp.
We will give ycppou significant penalty if it is modified from the original code.
If you think some modification is necessary, please contact us.

In general, we have designed our skeleton code so that you only have to fill vgg16_cuda.cu:predict() with your own kernels.
However, you can change the class structure at your will if you are not happy with what we've provided, as long as you don't touch main.cpp.

Check `model/main.py` to see original python code. 
Check `src/vgg16_cpu.cpp` to see converted c++ referenced code.

### Step 3: Compile and Submit to run on condor server(NOT SUBMIT to PROFESSOR)

```bash
user@acsys:~/bonus_HW/$ make 
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o build/util.o -c src/util.cpp
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o build/vgg16.o -c src/vgg16.cpp
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o build/vgg16_cpu.o -c src/vgg16_cpu.cpp
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o build/vgg16_cuda.o -c src/vgg16_cuda.cu
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o build/main.o -c src/main.cpp
/usr/local/cuda/bin/nvcc -std=c++11 -O3 -arch=sm_86 -o predict build/main.o build/util.o build/vgg16_cpu.o build/vgg16_cuda.o build/vgg16.o
mkdir -p build
./predict /vgg_data/cifar10/test_batch.bin 0 128 /vgg_data/vgg_weight/values_vgg.txt
[INFO] Arguments will be as following:
    INPUT_PATH: /vgg_data/cifar10/test_batch.bin
    DATA_OFFSET: 0
    BATCH: 128
    PARAMETER_PATH: /vgg_data/vgg_weight/values_vgg.txt
[INFO] Initialize variables
[INFO] Allocate memories
[INFO] Read image from data_offset 0 at /vgg_data/cifar10/test_batch.bin

```

### Step 4: Check the result

```bash
[INFO] Arguments will be as following: 
    INPUT_PATH: test_batch.bin
    DATA_OFFSET: 0
    BATCH: 128
    PARAMETER_PATH: /vgg_data/vgg_weight/values_vgg.txt
[INFO] Initialize variables
[INFO] Allocate memories
[INFO] Read image from data_offset 0 at test_batch.bin
[INFO] CPU  elapsed time is @#$%@#$%# msec
[INFO] CUDA elapsed time is #$%^$#^#$ msec
[INFO] CUDA predict is as following:
CPU:CLASS(NUMBER,T/F),CUDA:CLASS(NUMBER,T/F),Label:CLASS(NUMBER)
CPU:        cat(3,1), CUDA:        cat(3,1), Label:        cat(3)
CPU:       ship(8,1), CUDA:       ship(8,1), Label:       ship(8)
CPU:       ship(8,1), CUDA:       ship(8,1), Label:       ship(8)
CPU:   airplane(0,1), CUDA:   airplane(0,1), Label:   airplane(0)
CPU:       frog(6,1), CUDA:       frog(6,1), Label:       frog(6)
CPU:      truck(9,0), CUDA:      truck(9,0), Label:       frog(6)
CPU: automobile(1,1), CUDA: automobile(1,1), Label: automobile(1)
CPU:       bird(2,0), CUDA:       bird(2,0), Label:       frog(6)
CPU:        cat(3,1), CUDA:        cat(3,1), Label:        cat(3)
CPU: automobile(1,1), CUDA: automobile(1,1), Label: automobile(1)

Correct

CPU error:20% GPU error:20%
```

### Step 5: Have fun speeding up!

- Matrix Multiplication Techniques
- Im2Col
- Relu, FC, etc

## 4 Criteria
- You should implement the entire VGG16 with cuda 
- Your kernels should be functionally equivalent to the cpu version
- The trained model is supposed to give around 80% accuracy. 
We have set a huge margin of +-5% for the difference of the cuda version and the reference C++ version.
- If you fail to implement some part of the kernel in cuda, you can use the CPU version. 
However, it is your job to make sure to cudaMemcpy() so that the function still works correctly.
- No external libraries. Especially cuDNN. If you think you need something and it's not about cuda programming, contact us before doing so.
- As in HW3, we will measure the performance of batchsize=1 and batchsize=128, and use the product of the two.

## 5 Report
No report!

## 6 Grading
No grading!

## 7 Plus Alpha

### 7-1 Pretrained Model

There is a pre-trained model at `/vgg_data/vgg_weight/values_vgg.txt`. Loading pre-trained model from the txt file is already implemented in `src/vgg16.cpp`.

You can refer to code(`model/main_vgg.py`).

Activations are result of each layer with index 0 image(cat) at test data.


## 8 References

- Lecture Slides
- [Very Deep Convolutional Networks for Large-Scale Image Recognition](https://arxiv.org/abs/1409.1556)
- [CIFAR-10 dataset](https://www.cs.toronto.edu/~kriz/cifar.html)
- [Neural Network Tutorials - Pytorch](https://pytorch.org/tutorials/beginner/blitz/neural_networks_tutorial.html)
