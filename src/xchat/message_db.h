#ifndef MESSAGE_DB_H
#define MESSAGE_DB_H

#include "../db.h"
#include "message.h"

#include <string>
#include <vector>
#include <map>

typedef std::map<uint256, Message> UndeliveredMap;

//*****************************************************************************
//*****************************************************************************
class ChatDb : public CDB
{
protected:
    ChatDb();

public:
    static ChatDb & instance();

public:
    bool load(const std::string & address, std::vector<Message> & messages);
    bool save(const std::string & address, const std::vector<Message> & messages);
    bool erase(const std::string & address);

    bool loadUndelivered(UndeliveredMap & messages);
    bool saveUndelivered(const UndeliveredMap & messages);

    bool loadAddresses(std::vector<std::string> & addresses);

private:
    CCriticalSection m_cs;

    static std::string m_undelivered;
};

//*****************************************************************************
//*****************************************************************************

#endif // MESSAGE_DB_H
