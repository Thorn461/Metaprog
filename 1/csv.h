//
// Created by andrey on 20.10.19.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <tuple>
#include <sstream>
#include <string>

template <size_t Id, typename T>
struct RowParser {
  void parse(const std::vector<std::string> &vec, T &tuple) {
    std::istringstream(vec[Id]) >> std::get<Id>(tuple);
    RowParser<Id - 1, T> parser;
    parser.parse(vec, tuple);
  }
};

template <typename T>
struct RowParser<0, T> {
  void parse(const std::vector<std::string> &vec, T &tuple) {
    std::istringstream(vec[0]) >> std::get<0>(tuple);
  }
};

std::vector<std::string> split(const std::string &s, char delim) {
  std::istringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

template <typename... Parsers>
std::vector<std::tuple<Parsers...>> ReadCSV(const std::string& csv_file) {

  std::ifstream file;
  file.open(csv_file, std::ifstream::in);

  const size_t size = std::tuple_size<std::tuple<Parsers...>>::value - 1;
  RowParser<size, std::tuple<Parsers...>> parser;

  std::tuple<Parsers...> curr_result;
  std::vector<std::tuple<Parsers...>> result;
  std::string curr_line;

  while (std::getline(file, curr_line)) {
    std::vector<std::string> vec = split(curr_line, ',');
    parser.parse(vec, curr_result);
    result.push_back(curr_result);
  }
  return result;
}
