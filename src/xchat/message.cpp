
#include "util.h"
#include "key.h"
#include "net.h"
#include "ui_interface.h"
#include "base58.h"
#include "init.h"
#include "lz4/lz4.h"
#include "main.h"
#include "message_db.h"
#include "utiltime.h"
#include "messagecrypter.h"
#include "addressbookpage.h"
#include "guiutil.h"
#include "walletmodel.h"

#include "bip38.h"
#include "init.h"
#include "wallet.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>



//*****************************************************************************
//*****************************************************************************
// static
CCriticalSection Message::m_knownMessagesLocker;
std::set<uint256>      Message::m_knownMessages;

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

//*****************************************************************************
//*****************************************************************************
bool Message::appliesToMe() const
{
    // check broadcast message
    if (to.size() == 0)
    {
        return true;
    }

    CBitcoinAddress addr(to);
    if (!addr.IsValid())
    {
        return false;
    }

    CKeyID id;
    if (!addr.GetKeyID(id))
    {
        return false;
    }

    if (!pwalletMain->HaveKey(id))
    {
        return false;
    }

    return true;
}

//*****************************************************************************
//*****************************************************************************
time_t Message::getTime() const
{
    try
    {
        // date is "yyyy-MM-dd hh:mm:ss"
        boost::posix_time::ptime pt = boost::posix_time::time_from_string(date);

        // local time adjustor
        static boost::date_time::c_local_adjustor<boost::posix_time::ptime> adj;

        std::tm t = boost::posix_time::to_tm(adj.utc_to_local(pt));
        return std::mktime(&t);
    }
    //    catch (boost::bad_lexical_cast &)
    //    {
    //    }
    catch (std::exception &)
    {
    }
    return 0;
}

//*****************************************************************************
//*****************************************************************************
bool Message::isExpired() const
{
    int secs = std::time(0) - getTime();

    // +-2 days
    if (secs < -60*60*24*2 || secs > 60*60*24*2)
    {
        return true;
    }
    return false;
}

//*****************************************************************************
//*****************************************************************************
bool Message::send()
{
    return broadcast();
}

//*****************************************************************************
//*****************************************************************************
bool Message::process(bool & isForMe)
{
    isForMe = false;

    if (appliesToMe())
    {
        isForMe = true;

        {
            LOCK(m_knownMessagesLocker);

            uint256 hash = getHash();
            if (m_knownMessages.count(hash) > 0)
            {
                // already received and processed
                return true;
            }

            m_knownMessages.insert(hash);
        }

        uiInterface.NotifyNewMessage(*this);

        // send message received
        LOCK(cs_vNodes);
        BOOST_FOREACH(CNode * pnode, vNodes)
        {
            uint256 hash = getStaticHash();
            pnode->PushMessage("msgack", hash);
        }
    }
    return true;
}

//*****************************************************************************
//*****************************************************************************
// static
bool Message::processReceived(const uint256 & hash)
{
    ChatDb & db = ChatDb::instance();

    UndeliveredMap map;
    if (db.loadUndelivered(map))
    {
        if (map.erase(hash))
        {
            // it is my message, return processed
            db.saveUndelivered(map);
            return true;
        }
    }
    return false;
}

//*****************************************************************************
//*****************************************************************************
bool Message::relayTo(CNode * pnode) const
{
    uint256 hash = getHash();

    // returns true if wasn't already contained in the set
    if (pnode->setKnown.insert(hash).second)
    {
        pnode->PushMessage("message", *this);
        return true;
    }
    return false;
}

//*****************************************************************************
//*****************************************************************************
bool Message::broadcast() const
{
    LOCK(cs_vNodes);
    BOOST_FOREACH(CNode * pnode, vNodes)
    {
        relayTo(pnode);
    }

    return true;
}

extern const std::string strMessageMagic;

//*****************************************************************************
//*****************************************************************************
bool Message::sign(CKey & key)
{
    signature.clear();

    if (!key.IsValid())
    {
        // TODO
        // alert
        return false;
    }

    CDataStream ss(SER_GETHASH, 0);
    ss << strMessageMagic << text;

    // std::vector<unsigned char> vchSig;
    if (!key.SignCompact(Hash(ss.begin(), ss.end()), signature))
    {
        // TODO
        // alert
        return false;
    }

    return true;
}

//*****************************************************************************
//*****************************************************************************
//bool Message::verify(std::vector<unsigned char> sig)
//{
//    if (key.IsNull())
//    {
//        // TODO
//        // alert
//        return false;
//    }

//    CDataStream ss(SER_GETHASH, 0);
//    ss << strMessageMagic << text;

//    // std::vector<unsigned char> vchSig;
//    if (!key.SignCompact(Hash(ss.begin(), ss.end()), signature))
//    {
//        // TODO
//        // alert
//        return false;
//    }

//    return true;
//}


//*****************************************************************************
//*****************************************************************************
bool Message::encrypt(const CPubKey & senderPubKey)
{

    return true;
}

//*****************************************************************************
//*****************************************************************************
bool Message::decrypt(const CKey & _receiverKey, bool & isMessageForMy,
                      CPubKey & senderPubKey)
{
    return true;
}

//*****************************************************************************
//*****************************************************************************
bool Message::isEmpty() const
{
    return encryptedData.size() == 0;
}
