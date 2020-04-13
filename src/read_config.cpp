#include <read_config.h>

string ReadConfig::getValue (string key_)
{
	return args[key_];
}

void ReadConfig::display ()
{
#if _HAS_CXX17
	for (auto& [key, value] : this->args) {
		cout << key << ":" << value << endl;
	}
#else
	for (auto& p : this->args) {
		cout << p.first << ":" << p.second << endl;
	}

#endif
}

void ReadConfig::readConfig_help ()
{
	fstream f (this->filepath, ios::in);
	if (!f.is_open ()) {
		std::cerr << "文件不存在" << endl;
		return;
	}
	string buf;
	bool flag = true;
	while (flag) {
		while (isblank (f.peek ()))
			f.get ();
		if (getline (f, buf).eof ())
			flag = false;
		if (buf[0] == '#')
			continue;
		auto index = buf.find_first_of ('=');
		if (index != buf.size())
			// get arg and value
			args[buf.substr (0, index)] = buf.substr (index + 1, buf.size());
	}

}
