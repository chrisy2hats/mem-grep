#include "parsing.hpp"

std::vector<ValidTypes> Parsing::ParseMustContains(const QString& must_contain_input){

    const QStringList parts = must_contain_input.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    std::vector<ValidTypes> must_contain;
    //TODO handle ambigous arguments by getting the user to specify the type
    //TODO report error back to user if can't work out type for the input
    for (const auto& i : parts){
        std::cout << i.toStdString() << std::endl;

        bool is_int=false;
        const int integer = i.toInt(&is_int);
        if (is_int){
            std::cout << "Adding " << i.toStdString() << " as a int" << std::endl;
            must_contain.push_back(integer);
            continue;
        }

        bool is_float=false;
        const float flt = i.toFloat(&is_float);
        if (is_float){
            std::cout << "Adding " << i.toStdString() << " as a float" << std::endl;
            must_contain.push_back(flt);
            continue;
        }
    }

    return must_contain;
}

std::vector<Substitution> Parsing::ParseSubstitutions(const QString& substitutions){
    const QStringList parts = substitutions.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    return {};
}

pid_t Parsing::GetPID(const QString& pid_box_contents){

    bool is_pid;
    pid_t pid = pid_box_contents.toInt(&is_pid);
    if (!is_pid){
        pid=0;
        std::cout << "Failed to parse PID box as a PID. Will try use it as a program name" << std::endl;
        pid = pidof(pid_box_contents.toStdString());
    }

    //TODO Handle if PID parsing failed (PID is 0)
    return pid;
}

size_t Parsing::ParseMax(const QString& max_box_contents){
    bool valid_max_size;
    size_t max_size = max_box_contents.toULongLong(&valid_max_size);
    if (!valid_max_size){
        std::cout << "invalid max size defaulting to 8192 bytes" << std::endl;
        valid_max_size=8192;
    }
    return max_size;
}

size_t Parsing::ParseMin(const QString& min_box_contents){
    bool valid_min_size;
    size_t min_size = min_box_contents.toULongLong(&valid_min_size);
    if (!valid_min_size){
        std::cout << "invalid min size defaulting to 0" << std::endl;
        valid_min_size=0;
    }
    return min_size;
}


