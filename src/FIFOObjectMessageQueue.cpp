#include "Utility.hpp"
#include "Network.hpp"
#include "Server.hpp"
#include "FIFOServerMessageQueue.hpp"
#include "FIFOObjectMessageQueue.hpp"
#include "Message.hpp"
#include "Options.hpp"

namespace CBR {

FIFOObjectMessageQueue::FIFOObjectMessageQueue(ServerMessageQueue* sm, ObjectSegmentation* oseg, uint32 bytes_per_second, Trace* trace)
 : ObjectMessageQueue(sm, oseg, trace),
   mQueue(GetOption(OBJECT_QUEUE_LENGTH)->as<uint32>() * 32), // FIXME * numObjects?
   mLastTime(Time::null()),
   mRate(bytes_per_second),
   mRemainderBytes(0)
{
}

bool FIFOObjectMessageQueue::beginSend(CBR::Protocol::Object::ObjectMessage* msg, ObjMessQBeginSend & fromBegin)
{
    fromBegin.data = (void*)NULL;
    fromBegin.dest_uuid = msg->dest_object();

    Network::Chunk chunk;
    ObjectMessage obj_msg(mServerMessageQueue->getSourceServer(), *msg);
    obj_msg.serialize(chunk, 0);

    ServerMessagePair* smp = new ServerMessagePair(NULL, chunk, obj_msg.id());
    bool success = mQueue.push(msg->source_object(),smp)==QueueEnum::PushSucceeded;

    if (!success)
        delete smp;
    else
        fromBegin.data = (void*)smp;

    return success;
}

void FIFOObjectMessageQueue::endSend(const ObjMessQBeginSend& fromBegin, ServerID dest_server_id)
{
    ((ServerMessagePair*)fromBegin.data)->dest(dest_server_id);
}






void FIFOObjectMessageQueue::service(const Time& t){
    uint64 bytes = mRate * (t - mLastTime).toSeconds() + mRemainderBytes;

    while( bytes > 0) {
        ServerMessagePair* next_msg = mQueue.front(&bytes);
        if (next_msg == NULL) break;
        if (next_msg->dest() == NullServerID) break; // FIXME head of line blocking...

        bool sent_success = mServerMessageQueue->addMessage(next_msg->dest(), next_msg->data());
        if (!sent_success) break;

        mTrace->serverDatagramQueued(t, next_msg->dest(), next_msg->id(), next_msg->data().size());

        ServerMessagePair* next_msg_popped = mQueue.pop(&bytes);
        assert(next_msg_popped == next_msg);
        delete next_msg;
    }

    mRemainderBytes = mQueue.empty() ? 0 : bytes;
    mLastTime = t;
}

void FIFOObjectMessageQueue::registerClient(const UUID& sid, float weight) {
}

void FIFOObjectMessageQueue::unregisterClient(const UUID& sid) {
}


}
