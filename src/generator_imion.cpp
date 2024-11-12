#include <stdlib.h>
#include <string>
#include <memory>


class Generator {
  static constexpr std::string imiona[5] = {
    "Alex", "Marta", "Stefan", "Robert", "Julia"
  };
  public:
    static const std::string& losuj_imie() { return imiona[rand() % 5]; }
};
