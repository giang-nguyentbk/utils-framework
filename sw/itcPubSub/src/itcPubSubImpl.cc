#include <string>
#include <itc.h>

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
	  m_mboxFd(-1);
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
		std::cout << "\tDEBUG: addItcFd - Not a thread local!" << std::endl;
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	if(m_mboxFd != -1)
	{
		std::cout << "\tDEBUG: addItcFd - Mailbox FD " << fd << " already exists!" << std::endl;
		return IItcPubSub::ReturnCode::ALREADY_EXISTS;
	}

	auto callback = std::bind(&ItcPubSubImpl::handleFdEvent, this);

	if(IEventLoop::getInstance().addFdHandler(fd, IEventLoop::FdEventIn, callback) != IEventLoop::ReturnCode::NORMAL)
	{
		std::cout << "\tDEBUG: addItcFd - Failed to IEventLoop::addFdHandler()!" << std::endl;
		return IItcPubSub::ReturnCode::INTERNAL_FAULT;
	}

	m_mboxFd = fd;

	std::cout << "\tDEBUG: addItcFd - Added Mailbox FD " << fd << " successfully!" << std::endl;
	return IItcPubSub::ReturnCode::NORMAL;
}

IItcPubSub::ReturnCode ItcPubSubImpl::registerMsg(uint32_t msgNo, const MsgHandler& msgHandler)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		std::cout << "\tDEBUG: registerMsg - Not a thread local!" << std::endl;
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	auto it = m_msgHandlerMap.find(msgNo);
	if(it != m_msgHandlerMap.end())
	{
		std::cout << "\tDEBUG: registerMsg - Message number " << msgNo << " already exists!" << std::endl;
		return IItcPubSub::ReturnCode::ALREADY_EXISTS;
	}

	m_msgHandlerMap.emplace(msgNo, msgHandler);

	std::cout << "\tDEBUG: registerMsg - Registered message number " << msgNo << " successfully!" << std::endl;
	return IItcPubSub::ReturnCode::NORMAL;
}

IItcPubSub::ReturnCode ItcPubSubImpl::deregisterMsg(uint32_t msgNo)
{
	if(std::this_thread::get_id() != m_threadId)
	{
		std::cout << "\tDEBUG: deregisterMsg - Not a thread local!" << std::endl;
		return IItcPubSub::ReturnCode::NOT_THREAD_LOCAL;
	}

	auto it = m_msgHandlerMap.find(msgNo);
	if(it == m_msgHandlerMap.end())
	{
		std::cout << "\tDEBUG: deregisterMsg - Message number " << msgNo << " not found!" << std::endl;
		return IItcPubSub::ReturnCode::NOT_FOUND;
	}

	m_msgHandlerMap.erase(it);

	std::cout << "\tDEBUG: registerMsg - Deregistered message number " << msgNo << " successfully!" << std::endl;
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
			std::cout << "\tDEBUG: handleFdEvent - Message handler for msgNo " << itcMsg->msgNo << " not found!" << std::endl;
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