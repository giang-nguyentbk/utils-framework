#include <iostream>
#include <string>

#include <itc.h>
#include "util_framework_tpt_provider.h"
#include <traceIf.h>

#include "threadLocalIf.h"
#include "eventLoopIf.h"
#include "itcPubSubImpl.h"

union itc_msg
{
	uint32_t msgNo;
};

// Same as static function in C, all functions in this anonymous namespace are private and have only this-file scope. 
namespace
{

std::string getMboxName(itc_mbox_id_t mboxId)
{
	char name[ITC_MAX_NAME_LENGTH];

	if(itc_get_name(mboxId, name))
	{
		return std::string(name); // Convert C-string to std::string
	} else
	{
		return std::string("-");
	}
}

}

namespace UtilsFramework
{
namespace ItcPubSub
{
namespace V1
{

using namespace UtilsFramework::ThreadLocal::V1;
using namespace UtilsFramework::EventLoop::V1;

IItcPubSub& IItcPubSub::getInstance()
{
	return ItcPubSubImpl::getInstance();
}

ItcPubSubImpl& ItcPubSubImpl::getInstance()
{
	return IThreadLocal<ItcPubSubImpl>::get();
}

void ItcPubSubImpl::reset()
{
	IThreadLocal<ItcPubSubImpl>::reset();
}

ItcPubSubImpl::ItcPubSubImpl()
	: m_threadId(std::this_thread::get_id()),
	  m_mboxFd(-1)
{
}

ItcPubSubImpl::~ItcPubSubImpl()
{
	if(m_mboxFd != -1)
	{
		(void)IEventLoop::getInstance().removeFdHandler(m_mboxFd);
	}
}

IItcPubSub::ReturnCode ItcPubSubImpl::addItcFd(int fd)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		TPT_TRACE(TRACE_ERROR, "addItcFd - Not a thread local!");
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	if(m_mboxFd != -1)
	{
		TPT_TRACE(TRACE_ABN, "addItcFd - FD %d handler already exists!", fd);
		return IItcPubSub::ReturnCode::ALREADY_EXISTS;
	}

	auto callback = std::bind(&ItcPubSubImpl::handleFdEvent, this);

	if(IEventLoop::getInstance().addFdHandler(fd, IEventLoop::FdEventIn, callback) != IEventLoop::ReturnCode::NORMAL)
	{
		TPT_TRACE(TRACE_ERROR, "addItcFd - Failed to IEventLoop::addFdHandler()!");
		return IItcPubSub::ReturnCode::INTERNAL_FAULT;
	}

	m_mboxFd = fd;

	TPT_TRACE(TRACE_INFO, "addItcFd - Added Mailbox FD %d successfully!", fd);
	return IItcPubSub::ReturnCode::NORMAL;
}

IItcPubSub::ReturnCode ItcPubSubImpl::registerMsg(uint32_t msgNo, const MsgHandler& msgHandler)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		TPT_TRACE(TRACE_ERROR, "registerMsg - Not a thread local!");
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	auto it = m_msgHandlerMap.find(msgNo);
	if(it != m_msgHandlerMap.end())
	{
		TPT_TRACE(TRACE_ABN, "registerMsg - Message number 0x%08x already registered!", msgNo);
		return IItcPubSub::ReturnCode::ALREADY_EXISTS;
	}

	m_msgHandlerMap.emplace(msgNo, msgHandler);

	TPT_TRACE(TRACE_INFO, "registerMsg - Registered message number 0x%08x successfully!", msgNo);
	return IItcPubSub::ReturnCode::NORMAL;
}

IItcPubSub::ReturnCode ItcPubSubImpl::deregisterMsg(uint32_t msgNo)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		TPT_TRACE(TRACE_ERROR, "deregisterMsg - Not a thread local!");
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	auto it = m_msgHandlerMap.find(msgNo);
	if(it == m_msgHandlerMap.end())
	{
		TPT_TRACE(TRACE_ABN, "deregisterMsg - Message number 0x%08x not found!", msgNo);
		return IItcPubSub::ReturnCode::NOT_FOUND;
	}

	m_msgHandlerMap.erase(it);

	TPT_TRACE(TRACE_INFO, "deregisterMsg - Deregistered message number 0x%08x successfully!", msgNo);
	return IItcPubSub::ReturnCode::NORMAL;
}

void ItcPubSubImpl::handleFdEvent()
{
	union itc_msg* rawMsg = itc_receive(ITC_NO_WAIT);

	if(rawMsg)
	{
		// Convert raw pointer to shared_ptr
		std::shared_ptr<union itc_msg> itcMsg(rawMsg, [](union itc_msg* msg)
		{
			itc_free(&msg);
		});

		auto it = m_msgHandlerMap.find(itcMsg->msgNo);
		if(it != m_msgHandlerMap.end())
		{
			dispatchMsgHandler(it->second, itcMsg);
		} else
		{
			TPT_TRACE(TRACE_ABN, "handleFdEvent - No message handler found for msgNo 0x%08x sent from \"%s\" to our mailbox \"%s\"!", itcMsg->msgNo, getMboxName(itc_sender(itcMsg.get())).c_str(), getMboxName(itc_current_mbox()).c_str());
		}
	}
}

void ItcPubSubImpl::dispatchMsgHandler(const MsgHandler& msgHandler, const std::shared_ptr<union itc_msg>& msg)
{
	msgHandler(msg);
}

} // namespace V1

} // namespace ItcPubSub

} // namespace UtilsFramework