#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../Project5/Customer.cpp"
#include "../Project5/Schedule.cpp"
#include "../Project5/BookingScheduler.cpp"
#include "TestableSmsSender.cpp"
#include "TestableMailSender.cpp"
#include "TestableBookingScheduler.cpp"
#include "test.h"

using namespace testing;

class BookingItem : public testing::Test {
protected:
	void SetUp() override {
		NOT_ON_THE_HOUR = getTime(2021, 3, 26, 9, 5);
		ON_THE_HOUR = getTime(2021, 3, 26, 9, 0);
		SUNDAY_ON_THE_HOUR = getTime(2021, 3, 28, 17, 0);

		bookingScheduler.setSmsSender(&testableSmsSender);
		bookingScheduler.setMailSender(&testableMailSender);
	}

public:
	tm getTime(int year, int mon, int day, int hour, int min) {
		tm result = { 0, min, hour, day, mon - 1, year - 1900, 0, 0, -1 };
		mktime(&result);
		return result;
	}

	tm plusHour(tm base, int hour)
	{
		base.tm_hour += hour;
		mktime(&base);
		return base;
	}

	tm NOT_ON_THE_HOUR;
	tm ON_THE_HOUR;
	tm SUNDAY_ON_THE_HOUR;
	Customer CUSTOMER{ "Fake name", "010-1234-5678" };
	Customer CUSTOMER_WITH_MAIL{ "Fake name", "010-1234-5678" , "test@test.com" };
	const int UNDER_CAPACITY = 1;
	const int CAPACITY_PER_HOUR = 3;

	BookingScheduler bookingScheduler{ CAPACITY_PER_HOUR };
	TestableSmsSender testableSmsSender;
	TestableMailSender testableMailSender;
};

TEST_F(BookingItem, ������_���ÿ���_�����ϴ�_���ð�_�ƴѰ��_����Ұ�) {
	Schedule* schedule = new Schedule{ NOT_ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };

	EXPECT_THROW({ bookingScheduler.addSchedule(schedule);
		}, std::runtime_error);
}

TEST_F(BookingItem, ������_���ÿ���_�����ϴ�_������_���_���డ��) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));
}

TEST_F(BookingItem, �ð��뺰_�ο�������_�ִ�_����_�ð��뿡_Capacity_�ʰ���_���_���ܹ߻�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };
	bookingScheduler.addSchedule(schedule);

	try {
		Schedule* newSchedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };
		bookingScheduler.addSchedule(newSchedule);
		FAIL();
	}
	catch (std::runtime_error& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Number of people is over restaurant capacity per hour" });
	}
}

TEST_F(BookingItem, �ð��뺰_�ο�������_�ִ�_����_�ð��밡_�ٸ���_Capacity_���־_������_�߰�_����) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };
	bookingScheduler.addSchedule(schedule);

	tm differentHour = plusHour(ON_THE_HOUR, 1);
	Schedule* newSchedule = new Schedule{ differentHour, UNDER_CAPACITY, CUSTOMER };
	bookingScheduler.addSchedule(newSchedule);

	EXPECT_EQ(true, bookingScheduler.hasSchedule(schedule));
	EXPECT_EQ(true, bookingScheduler.hasSchedule(newSchedule));
}

TEST_F(BookingItem, ����Ϸ��_SMS��_������_�߼�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, CAPACITY_PER_HOUR, CUSTOMER };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(true, testableSmsSender.isSendMethodIsCalled());
}

TEST_F(BookingItem, �̸�����_����_��쿡��_�̸���_�̹߼�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(0, testableMailSender.getCountSendMailMethodIsCalled());
}

TEST_F(BookingItem, �̸�����_�ִ�_��쿡��_�̸���_�߼�) {
	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_MAIL };

	bookingScheduler.addSchedule(schedule);

	EXPECT_EQ(1, testableMailSender.getCountSendMailMethodIsCalled());
}

TEST_F(BookingItem, ���糯¥��_�Ͽ�����_���_����Ұ�_����ó��) {
	BookingScheduler* bookingScheduler = new TestableBookingScheduler(CAPACITY_PER_HOUR, SUNDAY_ON_THE_HOUR);

	try {
		Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_MAIL };
		bookingScheduler->addSchedule(schedule);
		FAIL();

	}
	catch (std::runtime_error& e) {
		EXPECT_EQ(string{ e.what() }, string{ "Booking system is not available on sunday" });
	}
}

TEST_F(BookingItem, ���糯¥��_�Ͽ�����_�ƴѰ��_���డ��) {
	BookingScheduler* bookingScheduler = new TestableBookingScheduler(CAPACITY_PER_HOUR, ON_THE_HOUR);

	Schedule* schedule = new Schedule{ ON_THE_HOUR, UNDER_CAPACITY, CUSTOMER_WITH_MAIL };
	bookingScheduler->addSchedule(schedule);

	EXPECT_EQ(true, bookingScheduler->hasSchedule(schedule));
}