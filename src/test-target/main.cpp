#include <iostream>
#include <thread>
#include <chrono>

int main() {
  int32_t counter = 0;

  std::wcout << L"Test target running. Counter address: " << &counter
             << std::endl;
  std::wcout << L"Press Enter to stop." << std::endl;

  std::thread counterThread([&counter]() {
    while (true) {
      counter++;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  std::cin.get();

  return 0;
}