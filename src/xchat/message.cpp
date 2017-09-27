
#include "util.h"
#include "key.h"
#include "net.h"
#include "ui_interface.h"
#include "base58.h"
#include "init.h"
#include "lz4/lz4.h"
#include "main.h"
#include "message_db.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

static CCriticalSection knownMessagesLocker_;
std::set<uint256>      Message::knownMessages_;


//*****************************************************************************
//*****************************************************************************
uint256 Message::getNetworkHash() const
{
    std::string hashstr = from + to + date = boost::lexical_cast<std::string>(timestamp);
    return Hash(hashstr.begin(), hashstr.end(),
                encryptedData.begin(), encryptedData.end());
}

//*****************************************************************************
//*****************************************************************************
uint256 Message::getHash() const
{
    std::string hashstr = from + to + date;
    return Hash(hashstr.begin(), hashstr.end(),
                encryptedData.begin(), encryptedData.end());
}

//*****************************************************************************
//*****************************************************************************
uint256 Message::getStaticHash() const
{
    std::string hashstr = from + to;
    return Hash(hashstr.begin(), hashstr.end(),
                encryptedData.begin(), encryptedData.end());
}

bool Message::appliesToMe() const {
    // check broadcast message
    if (to.empty()) {
        return true;
    }

    CBitcoinAddress address(to);
    if (!address.IsValid()) {
        return false;
    }

    CKeyID id;
    if (!address.GetKeyID(id)) {
        return false;
    }

//    return pwalletMain->HaveKey(id);

    return true;

}

time_t Message::getTime() const {
//    try {
//        // date is "yyyy-MM-dd hh:mm:ss"
//        boost::posix_time::ptime pt = boost::posix_time::time_from_string(date);

//        // local time adjustor
//        static boost::date_time::c_local_adjustor<boost::posix_time::ptime> adj;

//        std::tm t = boost::posix_time::to_tm(adj.utc_to_local(pt));
//        return std::mktime(&t);
//    }
//    catch (std::exception &)
//    {
//    }
    return 0;
}

bool Message::isExpired() const {
    int secs = std::time(0) - getTime();
//    // +-2 days
    return (secs < -60 * 60 * 24 * 2) || (secs > 60 * 60 * 24 * 2);


}

bool Message::send() {
    return broadcast();
}

bool Message::process(bool &isForMe) {
    isForMe = false;

    if (appliesToMe()) {
        isForMe = true;

        {
            LOCK(knownMessagesLocker_);

            uint256 hash = getHash();
            if (knownMessages_.count(hash) > 0) {
                // already received and processed
                return true;
            }
            knownMessages_.insert(hash);
        }
        uiInterface.NotifyNewMessage(*this);

        // send message received
        LOCK(cs_vNodes);
        BOOST_FOREACH(CNode *pnode, vNodes) {
                        uint256 hash = getStaticHash();
                        pnode->PushMessage("msgack", hash);
                    }
    }
    return true;


}

bool Message::relayTo(CNode *pnode) const {
    uint256 hash = getHash();

    // returns true if wasn't already contained in the set
    if (pnode->setKnown.insert(hash).second)
    {
        pnode->PushMessage("message", *this);
        return true;
    }
    return false;
}

bool Message::broadcast() const {
    LOCK(cs_vNodes);
    BOOST_FOREACH(CNode *pnode, vNodes) {
                    relayTo(pnode);
                }

    return false;
}

bool Message::sign(CKey &key) {
    signature.clear();

    if (!key.IsValid()) {//TODO: add isNull method
        // TODO alert
        return false;
    }

    CDataStream ss(SER_GETHASH, 0);
    ss << strMessageMagic << text;

    if (!key.SignCompact(Hash(ss.begin(), ss.end()), signature)) {
        // TODO alert
        return false;
    }
    return true;
}

bool Message::encrypt(const CPubKey &senderPubKey) {
    // TODO crypto
    return true;
}

bool Message::decrypt(const CKey &_receiverKey, bool &isMessageForMy,
                      CPubKey &senderPubKey) {
    // TODO crypto
    return true;
}


bool Message::isEmpty() const {
    return encryptedData.empty();
}

bool Message::processReceived(const uint256 &hash) {
    ChatDb &db = ChatDb::instance();
    UndeliveredMap map;
    if (db.loadUndelivered(map)) {
        if (map.erase(hash)) {
            // it is my message, return processed
            db.saveUndelivered(map);
            return true;
        }
    }
    return false;
}

