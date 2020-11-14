#ifndef _BAKERY_H_
#define _BAKERY_H_
#include <string>
#include <unordered_map>
#include <vector>

struct Item {
  std::string name;
  std::string price;
};

struct Order {
  std::string employee;
  std::vector<std::pair<std::string, std::string>> items;
};

struct Bakery {
  std::vector<std::string> employees;
  std::vector<Item> items;
  std::vector<Order> orders;
};

void print_bakery(const Bakery& bakery);
Bakery text_deserializer(std::string file_path);
void text_serializer(const Bakery& bakery, std::string file_path);
Bakery binary_deserializer(std::string file_path);
void binary_serializer(const Bakery& bakery, std::string file_path);

#endif  // _BAKERY_H