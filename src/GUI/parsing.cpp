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

ValidTypes Parsing::ParseNumber(const QString& num_str){

    bool is_int=false;
    const int integer = num_str.toInt(&is_int);
    if (is_int){
        std::cout << "Adding " << num_str.toStdString() << " as a int" << std::endl;
        return integer;
    }

    bool is_float=false;
    const float flt = num_str.toFloat(&is_float);
    if (is_float){
        std::cout << "Adding " << num_str.toStdString() << " as a float" << std::endl;
        return flt;
    }
    //TODO should really report the error to the caller
    return 0;
}
std::vector<Substitution> Parsing::ParseSubstitutions(const QString& substitutions_str){
    //User should express as a string like
    //"10 20 15 17"
    // Which should parse to {{.from=10,to=20},{.from=15,.to=17}}
    std::cout << "RAW sub_str" << substitutions_str.toStdString() << std::endl;

    Substitutions substitutions={};
    const QStringList parts = substitutions_str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if (parts.size()%2==0){
        for(int i=0;i<parts.size();i+=2){
            Substitution s={
                ParseNumber(parts[i]),
                ParseNumber(parts[i+1])
            };
            substitutions.push_back(s);
        }
    }else{
        std::cerr << "Odd number of subsititutions provided("<< parts.size() << "). Not sure what to do?!?!?" << std::endl;
    }

    return substitutions;
}

pid_t Parsing::GetPID(const QString& pid_box_contents){

    bool is_pid;
    pid_t pid = pid_box_contents.toInt(&is_pid);
    if (!is_pid){
        pid=0;
        std::cout << "Failed to parse PID box as a PID. Will try use it as a program name" << std::endl;
        pid = pidof(pid_box_contents.toStdString());
    }

    return pid;
}

size_t* Parsing::ParseSubstituteObjAddr(const QString& sub_obj_addr_contents){
    bool is_valid = false;
    size_t* addr = (size_t*)sub_obj_addr_contents.toULongLong(&is_valid,16);

    if (!is_valid)
        addr = nullptr;

    return addr;
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


