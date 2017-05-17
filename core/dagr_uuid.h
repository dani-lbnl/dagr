#ifndef dagr_uuid_h
#define dagr_uuid_h

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


// a universally uniquer identifier
class dagr_uuid : public boost::uuids::uuid
{
public:
    dagr_uuid() : boost::uuids::uuid(boost::uuids::random_generator()())
    {}

    explicit
    dagr_uuid(boost::uuids::uuid const& u) : boost::uuids::uuid(u)
    {}

    operator boost::uuids::uuid() {
        return static_cast<boost::uuids::uuid&>(*this);
    }

    operator boost::uuids::uuid() const {
        return static_cast<boost::uuids::uuid const&>(*this);
    }
};

#endif
