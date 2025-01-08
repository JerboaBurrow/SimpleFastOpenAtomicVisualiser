#ifndef ID_H
#define ID_H

#include "uuid.h"

#include <ostream>
#include <time.h>

namespace jGL
{

    struct Id 
    {
        Id()
        : id(Id::NULL_ID_CODE)
        {}

        Id(uint64_t i)
        : id(i)
        {}

        static uint64_t next(){uint64_t thisId = nextId; nextId++; return thisId;}
        static uuids::uuid getRunUUID() {return runUUID;}

        size_t hash() const {return std::hash<uint64_t>{}(id);}

        uint64_t id;

        bool operator==( Id const & rhs ) const {return this->id == rhs.id;}
        
        bool operator<(const Id & rhs) const {return this->id < rhs.id;}

        Id & operator=(const Id & j)
        {
            id = j.id;
            return *this;
        }

        static const uint64_t NULL_ID_CODE = 0;

    private:

        static std::random_device rd;
        static std::mt19937 generator;
        static uuids::uuid_random_generator genUUID;

        static const uuids::uuid runUUID;

        static uint64_t nextId;
    };

    std::ostream & operator<<(std::ostream & os, Id const & value);
    std::string to_string(const Id & i);

    const Id NULL_ID = Id();

}

template<> struct std::hash<jGL::Id> 
{
    std::size_t operator()(const jGL::Id & i) const {return i.hash();}
};

#endif /* ID_H */
