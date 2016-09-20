#ifndef __RE_REGEX_FILE_PREDICATE_H__
#define __RE_REGEX_FILE_PREDICATE_H__

#include "sys/FileFinder.h"
#include "re/Regex.h"

namespace re
{

struct RegexPredicate : public sys::FilePredicate
{
public:
    RegexPredicate(const std::string& match)
    {
        mRegex.compile(match);
    }
    
    bool operator()(const std::string& filename) const
    {
        return mRegex.matches(filename);
    }

private:
    re::Regex mRegex;

};

}
#endif
