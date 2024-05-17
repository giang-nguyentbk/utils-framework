/*
*        ________________           ________                                                    ______  
* ____  ___  /___(_)__  /_______    ___  __/____________ _______ ___________      _________________  /__
* _  / / /  __/_  /__  /__  ___/    __  /_ __  ___/  __ `/_  __ `__ \  _ \_ | /| / /  __ \_  ___/_  //_/
* / /_/ // /_ _  / _  / _(__  )     _  __/ _  /   / /_/ /_  / / / / /  __/_ |/ |/ // /_/ /  /   _  ,<   
* \__,_/ \__/ /_/  /_/  /____/      /_/    /_/    \__,_/ /_/ /_/ /_/\___/____/|__/ \____//_/    /_/|_|  
*                                                                                                       
*/

#pragma once

#include <thread>
#include <unordered_map>

#include "itcPubSubIf.h"

namespace UtilsFramework
{
namespace ItcPubSub
{
namespace V1
{

class ItcPubSubImpl : public IItcPubSub
{
public:
	static ItcPubSubImpl& getInstance();
	static void reset();

	ReturnCode addItcFd(int fd) override;
	ReturnCode registerMsg(uint32_t msgNo, const MsgHandler& msgHandler) override;
	ReturnCode deregisterMsg(uint32_t msgNo) override;

	ItcPubSubImpl();
	virtual ~ItcPubSubImpl();

	// First prevent copy/move construtors
	ItcPubSubImpl(const ItcPubSubImpl&)               = delete;
	ItcPubSubImpl(ItcPubSubImpl&&)                    = delete;
	ItcPubSubImpl& operator=(const ItcPubSubImpl&)    = delete;
	ItcPubSubImpl& operator=(ItcPubSubImpl&&)         = delete;

private:
	void handleFdEvent();
	void dispatchMsgHandler(const MsgHandler& msgHandler, const std::shared_ptr<union itc_msg>& msg);

	std::thread::id m_threadId;
	int m_mboxFd;
	using MsgHandlerMap = std::unordered_map<uint32_t /* msgNo */, MsgHandler /* msgHandler */>;
	MsgHandlerMap m_msgHandlerMap;

}; // class ItcPubSubImpl

} // namespace V1

} // namespace ItcPubSub

} // namespace UtilsFramework