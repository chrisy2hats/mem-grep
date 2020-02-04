#include "../shared/misc/structs.hpp"
#include "../shared/misc/utils.hpp"
#include <QString>
#include <QStringList>

class Parsing{
 public:
    static std::vector<Substitution> ParseSubstitutions(const QString& substitutions_str);
    static std::vector<ValidTypes> ParseMustContains(const QString& must_contain_input);
    static size_t ParseMax(const QString& max_box_contents);
    static size_t ParseMin(const QString& min_box_contents);
    static pid_t GetPID(const QString& pid_box_contents);
 private:
    static ValidTypes ParseNumber(const QString& num_str);
};
