#include "random.h"
#include <array>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>

class Creature {
public:
  Creature(std::string_view name, char symbol, int health, int ad,
           int goldWorth)
      : m_name{name}, m_symbol{symbol}, m_health{health}, m_attackDamage{ad},
        m_goldWorth{goldWorth} {}

  std::string getName() const { return m_name; }
  char getSymbol() const { return m_symbol; }
  int getHealth() const { return m_health; }
  int getAttackDamage() const { return m_attackDamage; }
  int getGold() const { return m_goldWorth; }

  void reduceHealth(int dmg) { m_health -= dmg; }
  void increaseHealth(int health) { m_health += health; }
  void increaseAttackDmg(int dmg) { m_attackDamage += dmg; }

  bool isDead() const { return m_health <= 0; }

  void addGold(int gold) { m_goldWorth += gold; }

protected:
  std::string m_name{};
  char m_symbol{};
  int m_health{};
  int m_attackDamage{};
  int m_goldWorth{};
};

class Player : public Creature {

public:
  Player(std::string_view name) : Creature{name, '@', 10, 1, 0} {}

  int getLevel() const { return m_level; }

  void levelUp() {
    m_level++;
    m_attackDamage++;
  }

  bool hasWon() { return m_level >= 20; }

private:
  int m_level{1};
};

class Monster : public Creature {
public:
  enum Type { dragon, orc, slime, maxTypes };

  static inline Creature monsterData[]{

      Creature{"dragon", 'D', 20, 4, 100},
      Creature{"orc", 'O', 4, 2, 25},
      Creature{"slime", 'S', 1, 1, 10},
  };

  Monster(Type type) : Creature{monsterData[type]} {}

  static Monster getRandomMonster() {
    int num{Random::get(0, maxTypes - 1)};

    return Monster{static_cast<Type>(num)};
  }
};

class Potion {
public:
  enum Type { health, strength, poison, maxPotions };
  std::array<std::string, maxPotions> potionNamesArr{"health", "strength",
                                                     "poison"};

  Potion() {
    int num{Random::get(0, 2)};
    m_name = potionNamesArr[num];
    m_effect = handleEffectStrength(m_name);
  }

  int handleEffectStrength(std::string_view name) {

    int num{Random::get(1, 3)};
    if (name == "health") {
      switch (num) {
      case 1:
        return 2;
      case 2:
        return 3;
      case 3:
        return 4;
      }
    } else if (name == "strength") {
      switch (num) {
      case 1:
        return 1;
      case 2:
        return 2;
      case 3:
        return 4;
      }
    } else if (name == "poison") {
      switch (num) {
      case 1:
        return -1;
      case 2:
        return -1;
      case 3:
        return -2;

      default:
        std::cout << "issue wiith effect init";
        break;
      }
    }
    std::cout << "Error: Invalid effect name passed!\n";
    return 0;
  }

  friend std::ostream& operator<<(std::ostream& out, Potion& p) {

    return out << "Potion name: " << p.m_name
               << " Potion effect strength: " << p.m_effect << '\n';
  }

  std::string_view getPotionName() const { return m_name; }
  int getPotionEffectVal() const { return m_effect; }

private:
  std::string m_name;
  int m_effect;
};

bool handlePotion() {
  int num{Random::get(1, 3)};
  if (num == 2) {
    return true;
  } else {
    return false;
  }
}

bool handlePotionDrink(Potion& p, Player& player) {

  std::string_view pName{p.getPotionName()};
  int pVal{p.getPotionEffectVal()};

  if (pName == "health") {
    player.increaseHealth(pVal);
    std::cout << "You found a " << p.getPotionName()
              << " potion your health increased by " << p.getPotionEffectVal()
              << '\n';
    return true;
  } else if (pName == "strength") {
    player.increaseAttackDmg(pVal);
    std::cout << "You found a " << p.getPotionName()
              << " potion your attack damage increased by "
              << p.getPotionEffectVal() << '\n';
    return true;
  } else if (pName == "poison") {
    player.reduceHealth(pVal);
    std::cout << "You found a " << p.getPotionName()
              << " potion your health decreased by " << p.getPotionEffectVal()
              << '\n';
    return true;
  }
  std::cout << "Problem handling Potion drink";
  return false;
}

bool handleFight(Player& p, Monster& m) {
  std::cout << "You hit the " << m.getName() << " for " << p.getAttackDamage()
            << " hp " << '\n';
  m.reduceHealth(p.getAttackDamage());
  if (m.isDead()) {
    std::cout << "You killed the " << m.getName() << '\n';
    p.levelUp();
    std::cout << "You leveled up to " << p.getLevel() << '\n';
    bool odds{handlePotion()};
    if (odds) {
      Potion potion{};
      char answer{};
      std::cout << "You found a mythical Potion! Do you want to drink it? "
                   "(Y)es (N)o: ";
      std::cin >> answer;
      if (answer == 'n') {
        return false;
      }
      if (answer == 'y') {
        handlePotionDrink(potion, p);
        return false;
      }
    }
    return false;
  } else {
    std::cout << "The " << m.getName() << " hit you for " << m.getAttackDamage()
              << " hp " << '\n';

    p.reduceHealth(m.getAttackDamage());
    std::cout << "Your health: " << p.getHealth() << '\n';
    if (p.isDead()) {
      return false;
    } else {
      return true;
    }
  }
}

bool handleRun(Player& p, Monster& m) {
  int num{Random::get(0, 1)};
  if (num == 0) {
    std::cout << "You failed to flee" << '\n';
    std::cout << "The " << m.getName() << " hit you for " << m.getAttackDamage()
              << '\n';
    p.reduceHealth(m.getAttackDamage());
    return false;
  } else if (num == 1) {
    std::cout << "Phew! You escaped!" << '\n';
    return true;
  } else {
    return true;
  }
}

bool handleEncounter(Player& p) {
  while (p.getHealth() > 0 && p.getLevel() < 20) {
    std::cout << '\n';
    Monster m{Monster::getRandomMonster()};

    std::cout << "You have encountered a " << m.getName() << "("
              << m.getSymbol() << ")" << '\n';

    bool isFighting{true};
    bool hasEscaped{false};
    while (isFighting && !hasEscaped) {
      char choice{};
      std::cout << "(R)un or (F)ight ";
      std::cin >> choice;
      if (choice == 'f') {
        isFighting = handleFight(p, m);
      }
      if (choice == 'r') {
        hasEscaped = handleRun(p, m);
      }
    }
    return true;
  }
  return false;
}

int main() {
  Potion potion{};
  std::cout << potion << '\n';

  std::cout << "Enter your name: ";
  std::string playerName;
  std::cin >> playerName;

  Player p{playerName};
  std::cout << "Welcome, " << p.getName() << ".\n";

  std::cout << "You have " << p.getHealth()
            << " health and are level: " << p.getLevel() << "\nGood luck!"
            << '\n';

  while (handleEncounter(p)) {
  }
  if (p.getLevel() >= 20) {
    std::cout << "You won by getting level " << p.getLevel()
              << "Good job adventurer!\n";
    return 0;
  }
  if (p.getHealth() <= 0) {

    std::cout << "Oh No! You died at level " << p.getLevel() << " with "
              << p.getGold() << " gold\n";
    return 0;
  }
}
