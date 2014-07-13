#include "UserData.h"

UserData *UserData::s_instance = nullptr;

UserData * UserData::getInstance()
{
	if (!s_instance)
	{
		s_instance = new UserData;
	}
	return s_instance;
}

UserData::UserData()
{

}

UserData::~UserData()
{

}
