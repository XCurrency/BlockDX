#ifndef MESSAGE_H
#define MESSAGE_H

#include "../uint256.h"
#include "../sync.h"
#include "../key.h"
#include <vector>
#include <string>
#include <set>
#include "init.h"
#include <boost/thread/recursive_mutex.hpp>

class CNode;

struct Message {
    enum
    {
        maxMessageSize = 1024,
        signatureSize = 65,

        // length of header, 4 + 1 + 8 + 20 + 16 + 33 + 32 + 4 +4
        headerLength   = 122,

        // length of encrypted header
        encryptedHeaderLength = 1+20+65+4
    };

    std::string from;
    std::string to;
    std::string date;
    std::string text;
    std::vector<unsigned char> signature;

    // generated key for encription
    std::vector<unsigned char> publicRKey;
    // encrypted data
    std::vector<unsigned char> encryptedData;
    // message auth code
    std::vector<unsigned char> mac;
    //
    std::vector<unsigned char> iv;
    //
    time_t timestamp;


    ADD_SERIALIZE_METHODS

    template<typename Stream, typename Operation>
    inline void SerializationOp(Stream &s, Operation ser_action, int nType, int nVersion) {
        READWRITE(from);
        READWRITE(to);
        READWRITE(date);
        READWRITE(text);
        READWRITE(signature);
        READWRITE(publicRKey);
        READWRITE(encryptedData);
        READWRITE(mac);
        READWRITE(iv);
    }
    Message() { timestamp = std::time(0); }
    Message(const Message & other) { *this = other; }

    Message & operator = (const Message & other)
    {
        from          = other.from;
        to            = other.to;
        date          = other.date;
        text          = other.text;
        signature     = other.signature;
        publicRKey    = other.publicRKey;
        encryptedData = other.encryptedData;
        mac           = other.mac;
        iv            = other.iv;
        timestamp     = other.timestamp;
        return *this;
    }

    bool operator < (const Message & other) const
    {
        return date < other.date;
    }

    // full hash
    uint256 getNetworkHash() const;
    // hash without timestamp
    uint256 getHash() const;
    // hash without date
    uint256 getStaticHash() const;

    bool appliesToMe() const;

    time_t getTime() const;
    bool isExpired() const;

    bool send();
    bool process(bool & isForMe);

    static bool processReceived(const uint256 & hash);

    bool relayTo(CNode * pnode) const;
    bool broadcast() const;

    bool sign(CKey & key);
    // bool verify(std::vector<unsigned char> sig);

    bool encrypt(const CPubKey & senderPubKey);
    bool decrypt(const CKey & receiverKey,
                 bool & isMessageForMy,
                 CPubKey & senderPubKey);

    bool isEmpty() const;

private:
    static /*boost::recursive_mutex*/CCriticalSection m_knownMessagesLocker;
    static std::set<uint256>      m_knownMessages;
};

class CKeyIdPpn : public CKeyID {
public:
    unsigned int *getPpn() { return pn; }
};

#endif // MESSAGE_H
