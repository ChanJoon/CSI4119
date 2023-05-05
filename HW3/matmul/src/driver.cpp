#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "matmul.h"

using namespace std;

int main(int argc, char** argv) {
  // Read arguments
  string inputPath = "data/input.txt";
  string outputPath = "data/output.txt";
  bool sanityCheck = false;
  if (argc == 3) {
    inputPath = argv[1];
    outputPath = argv[2];
  } else {
    cout << "./matmul inputPath outputPath" << endl;
    exit(-1);
  }
  // Read the size of matrices
  ifstream inputFile(inputPath.data());
  ifstream outputFile(outputPath.data());
  if (!inputFile.is_open()) {
    cout << "Open inputFile(" << inputPath << ") failed" << endl;
    exit(-1);
  }
  if (!outputFile.is_open()) {
    cout << "Open outputFile(" << outputPath << ") failed" << endl;
    exit(-1);
  }
  int nFromInput;
  int nFromOutput;
  inputFile >> nFromInput;
  outputFile >> nFromOutput;
  if (nFromInput != nFromOutput) {
    cout << "nFromInput(" << nFromInput << "), nFromOutput(" << nFromOutput
         << ") is not matched" << endl;
    exit(-1);
  }
  assert(nFromInput == nFromOutput);
  int n = nFromInput;
  // Print information
  cout << "=====================================" << endl;
  cout << "         Matrix Multiplication       " << endl;
  cout << "=====================================" << endl;
  cout << "The size of Matrix: " << n << endl;
  cout << "=====================================" << endl << endl;
  // Allocate memory
  int* matrixA = new int[n * n];
  int* matrixB = new int[n * n];
  int* matrixC = new int[n * n];
  int* matrixRef = new int[n * n];
  // Initialize
  cout << "Read input file(" << inputPath << ")..." << endl;
  for (int i = 0; i < n * n; i++)
    inputFile >> matrixA[i];
  for (int i = 0; i < n * n; i++)
    inputFile >> matrixB[i];
  cout << "Read output file(" << outputPath << ")..." << endl;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) outputFile >> matrixRef[i * n + j];
  // Close files
  inputFile.close();
  outputFile.close();
  // Check ref if sanityCheck true


  cout << endl << "Run your solution..." << endl << endl;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) matrixC[i * n + j] = 0;

    std::chrono::duration<double> diff;
    auto fill_start = std::chrono::steady_clock::now();
    matmul_optimized((const int* const)matrixA, (const int* const)matrixB, (int* const)matrixC, n, n); 
    auto fill_end = std::chrono::steady_clock::now();
    diff = fill_end - fill_start;
    std::cout << "matmul_optimal took " << diff.count() << " sec" << std::endl;
    // Check sanity
    bool flag = true;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (matrixC[i * n + j] != matrixRef[i * n + j]) {
          flag = false;
          break;
        }
      }
      if (!flag) break;
    }
    if (flag) {
      cout << "Correct" << endl << endl;
    } else {
      cout << "!!!!!!!Incorrect!!!!!!!" << endl << endl;
    }



  // Free memory
  delete[] matrixA;
  delete[] matrixB;
  delete[] matrixC;
  delete[] matrixRef;
}
