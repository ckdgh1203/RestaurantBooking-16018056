#include "pch.h"
#include "../Project5/MailSender.cpp"

class TestableMailSender : public MailSender {
public:
	void sendMail(Schedule* shcedule) override {
		countSendMailMethodIsCalled++;
	}

	int getCountSendMailMethodIsCalled() {
		return countSendMailMethodIsCalled;
	}

private:
	int countSendMailMethodIsCalled = 0;
};