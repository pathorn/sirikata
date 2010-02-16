/*  Sirikata liboh -- Object Host
 *  MonoObjectScript.cpp
 *
 *  Copyright (c) 2009, Daniel Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "oh/Platform.hpp"
#include "MonoVWObjectScriptManager.hpp"
#include "MonoVWObjectScript.hpp"
#include "MonoSystem.hpp"
#include "MonoClass.hpp"
#include "MonoAssembly.hpp"
#include "MonoArray.hpp"
#include "MonoException.hpp"
#include "util/RoutableMessageHeader.hpp"
#include "MonoContext.hpp"
namespace Sirikata {

namespace {
/** Gets an argument from the argument map if it exists, removes it from the
 *  map, and returns its value (or the empty string if it doesn't exist).
 */
String GetAndFilterArgument(ObjectScriptManager::Arguments& args, const String& argname) {
    if (args.find(argname) == args.end())
        return "";
    String retval = args[argname];
    args.erase(argname);
    return retval;
}
} // namespace

MonoVWObjectScript::MonoVWObjectScript(Mono::MonoSystem*mono_system, HostedObjectPtr ho, const ObjectScriptManager::Arguments&args)
 : mParent(ho),
   mDomain(mono_system->createDomain()),
   mObject()
{
    // Filter out reserved parameters
    ObjectScriptManager::Arguments filtered_args = args;
    String assembly_name = GetAndFilterArgument(filtered_args, "Assembly");
    String namespace_name = GetAndFilterArgument(filtered_args, "Namespace");
    String class_name = GetAndFilterArgument(filtered_args, "Class");
    // Build the arg array to pass into the constructor
    std::vector<String> flattened_args;
    for(ObjectScriptManager::Arguments::const_iterator arg_it = filtered_args.begin(); arg_it != filtered_args.end(); arg_it++) {
        flattened_args.push_back(arg_it->first);
        flattened_args.push_back(arg_it->second);
    }

    if (!assembly_name.empty())
        mono_system->loadAssembly(assembly_name);

    try {
        Mono::Assembly ass = mDomain.getAssembly(assembly_name);

        MonoContext::getSingleton().push(MonoContextData(mDomain, ho));
        try {
            Mono::Class class_type = ass.getClass(namespace_name, class_name);
            Mono::Array mono_args = mDomain.StringArray(flattened_args);
            try {
                mObject=class_type.instance(mono_args);
            }catch(Mono::Exception&e) {
                SILOG(mono,warning,"Making new object: Cannot locate constructor in class "<<namespace_name<<"::"<<class_name<<"with "<<mono_args.length()<<" arguments."<<e);
            }
        } catch (Mono::Exception&e) {
            SILOG(mono,warning,"Making new object: Cannot locate class "<<namespace_name<<"::"<<class_name<<"."<<e);
        }
        MonoContext::getSingleton().pop();
    } catch (Mono::Exception&e) {
        SILOG(mono,warning,"Making new object: Cannot locate assembly "<< assembly_name <<"."<<e);
    }
}
MonoVWObjectScript::~MonoVWObjectScript() {
    //mono_jit_cleanup(mDomain.domain());
}

bool MonoVWObjectScript::forwardMessagesTo(MessageService*){
    NOT_IMPLEMENTED(mono);
    return false;
}

bool MonoVWObjectScript::endForwardingMessagesTo(MessageService*){
    NOT_IMPLEMENTED(mono);
    return false;
}

bool MonoVWObjectScript::processRPC(const RoutableMessageHeader &receivedHeader, const std::string &name, MemoryReference args, MemoryBuffer &returnValue){
    MonoContext::getSingleton().push(MonoContextData(mDomain, mParent));
    std::string header;
    receivedHeader.SerializeToString(&header);
    try {
        Mono::Object retval=mObject.send("processRPC",mDomain.ByteArray(header.data(),(unsigned int)header.size()),mDomain.String(name),mDomain.ByteArray((const char*)args.data(),(int)args.size()));
        if (!retval.null()) {
            returnValue=retval.unboxByteArray();
            MonoContext::getSingleton().pop();
            return true;
        }
        MonoContext::getSingleton().pop();
        return false;
    }catch (Mono::Exception&e) {
        SILOG(mono,debug,"RPC Exception "<<e);
        MonoContext::getSingleton().pop();
        return false;
    }
    MonoContext::getSingleton().pop();
    return true;
}

void MonoVWObjectScript::processMessage(const RoutableMessageHeader& receivedHeader, MemoryReference body){
    std::string header;
    receivedHeader.SerializeToString(&header);
    MonoContext::getSingleton().push(MonoContextData(mDomain, mParent));
    try {
        Mono::Object retval=mObject.send("processMessage",mDomain.ByteArray(header.data(),(unsigned int)header.size()),mDomain.ByteArray((const char*)body.data(),(unsigned int)body.size()));
    }catch (Mono::Exception&e) {
        SILOG(mono,debug,"Message Exception "<<e);
    }
    MonoContext::getSingleton().pop();
}

bool MonoVWObjectScript::valid() const {
    return !mObject.null();
}


}
