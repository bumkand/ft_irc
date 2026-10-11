

// current date + time as text, for RPL_CREATED (003)
// gmtime = UTC on purpose, so it doesn't depend on the time zone of the machine
static std::string startTime()
{
	char buff[64];
	time_t now = time(NULL);

	strftime(buff, sizeof(buff), "%a %b %d %Y at %H:%M:%S UTC", gmtime(&now));
	return std::string(buff);
}