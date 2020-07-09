#include "utils.hpp"

#include <iostream>
#include  <numeric>

namespace WShuffleUtils
{
	using convert_t = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_t, wchar_t> strconverter;

	std::wstring to_wstring(const std::string &str)
	{
		return strconverter.from_bytes(str);
	}
	
	std::string to_string(const std::wstring& wstr)
	{
		return strconverter.to_bytes(wstr);
	}

	size_t count_chars(const std::string utf8_string)
	{
		//std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(utf8_string).size();
		//return cvt.from_bytes(str).size();
	}

    bool create_out_file(const std::string &f_in, std::string &f_out) {
		auto pos = f_in.find_last_of(".");
		if (pos == std::string::npos) {
			f_out = f_in;
			f_out += "_out";
			return true;
		}
		else {
			f_out = f_in.substr(0, pos);
			std::string ext = f_in.substr(pos);
			f_out += "_out" + ext;
			return true;
		}
    }

	
#ifdef __GNUC__
    void systemInfo(bool verbose) {
        std::ifstream file("/proc/version");
        std::string str;
        while (std::getline(file, str))
        {
            std::cout << str;
        }
        std::cout << '\n';
    }

    std::vector<std::string> read_outline(std::string &file) {
        namespace bp = boost::process;
        bp::ipstream is; //reading pipe-stream
        bp::child c(bp::search_path("nm"), file, bp::std_out > is);

        std::vector<std::string> data;
        std::string line;

        while (c.running() && std::getline(is, line) && !line.empty())
            data.push_back(line);

        c.wait();

        return data;
    }

#endif

    bool create_out_file_list(const VStrList &in_list, VStrList &out_list) {
        uint32_t N = in_list.size();
        std::vector<bool> inter_res(N, true);
        out_list.resize(N, std::string(""));
        for (int i = 0; i < N; ++i) {
            inter_res[i] = create_out_file(in_list[i], out_list[i]);
        }
        auto inter_sum = std::accumulate(inter_res.begin(), inter_res.end(),0);
        //std::cout << inter_res << std::endl;

        //std::cout << "File In size = " << N << "\n" << "Inter Sum = " << inter_sum << "\n";
        if (inter_sum == N )
            return true;
        else {
            std::cout << "Errors have been found in the following tranformations : " << '\n';
            for (int i = 0; i < N; i++) {
                if (!inter_res[i]) {
                    std::cout << i+1 << ". " << in_list[i] << '\n';
                }
            }
            return false;
        }
    }

	void show_info(const std::string & message, const StringList & lst)
	{
		std::cout << "* ***************************************  *" << '\n';
		std::cout << '\t' << message << '\n';
		std::cout << lst << std::endl;
	}

	void show_info(const std::string & message, const SizeMap & sm)
	{
		std::cout << "* ***************************************  *" << '\n';
		std::cout << '\t' << message << '\n';
		std::cout << sm << std::endl;
	}

	bool is_ok_to_continue(const std::string & error_message)
	{
		char ans;
		std::cout << error_message << std::endl;
		//std::cout << "Do you want to continue anyway (y/n) ?";
		// Ignore to the end of line
		std::cin >> ans;
		//std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		while (ans != 'y' && ans != 'Y' && ans != 'n' && ans != 'N' )
		{
			std::cout << "Please press 'y' or 'n' : ";
			std::scanf("%c", &ans);
		}
		if (ans == 'y' || ans=='Y')
			return true;
		
		return false;
	}

    std::string get_settings_dir() {
	    namespace fs = boost::filesystem;
	    auto user_dir = fs::path(getenv("HOME"));
	    auto settings_dir = user_dir += fs::path("/.local/share/wshuffle");
	    //std::cout << "Posible settings dir : " << settings_dir.string() << '\n';
	    if (!fs::is_directory(settings_dir)) {
	        fs::create_directory(settings_dir);
	        std::cout << "Settings directory created succesfully : " << settings_dir.string() << '\n';
	    }
	    //std::cout << "Settings directory : " << fs::canonical(settings_dir).string() << std::endl;
        return fs::canonical(settings_dir).string();
    }

    void init_app() {
        namespace fs = boost::filesystem;
	    auto settings_p = get_settings_dir();
	    std::vector<std::string> cfg_files = {"editor.json", "wshuffle.cfg"};
	    for(auto &x : cfg_files) {
	        fs::path p(settings_p + "/"+x);
	        if (!fs::exists(p)){
	            std::string src_path = WSHSRC;
	            src_path += "/" + x;
                fs::copy_file(fs::path( src_path),fs::path(p));
	        }
	    }
    }

    std::string settings_file_path(const std::string &filename) {
	    namespace fs = boost::filesystem;
	    fs::path p(get_settings_dir() + "/" + filename);
	    //std::cout << fs::canonical(p).string() << std::endl;
        return fs::canonical(p).string();
    }


#ifdef _WIN32
#include <Windows.h>
#include  <ntstatus.h>
#include <LM.h>
#pragma comment(lib, "user32.lib")
	
    double getSysOpType()
    {
        double ret = 0.0;
        NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
        OSVERSIONINFOEXW osInfo;

        *(FARPROC *)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

        if (NULL != RtlGetVersion)
        {
            osInfo.dwOSVersionInfoSize = sizeof(osInfo);
            RtlGetVersion(&osInfo);
            ret = osInfo.dwMajorVersion;
        }
        return ret;
    }

    void systemInfo(bool verbose)
    {
        std::printf("\tOS Info : Windows %5.2f\n", getSysOpType());
        if (verbose)
        {
            SYSTEM_INFO siSysInfo;
            GetSystemInfo(&siSysInfo);
            std::printf("Hardware information: \n");
            std::printf("  OEM ID: %u\n", siSysInfo.dwOemId);
            std::printf("  Number of processors: %u\n",
                        siSysInfo.dwNumberOfProcessors);
            std::printf("  Page size: %u\n", siSysInfo.dwPageSize);
            std::printf("  Processor type: %u\n", siSysInfo.dwProcessorType);
            std::printf("  Minimum application address: %lx\n",
                        siSysInfo.lpMinimumApplicationAddress);
            std::printf("  Maximum application address: %lx\n",
                        siSysInfo.lpMaximumApplicationAddress);
            std::printf("  Active processor mask: %u\n",
                        siSysInfo.dwActiveProcessorMask);
        }
    }
#endif


} // namespace WShuffleUtils
std::ostream &operator<<(std::ostream &os, const WShuffleUtils::StringList &sl)
{
	int i = 0;
	for (const auto &item : sl)
	{
		os << '\t';
		os << std::left;
		std::ostringstream oss;
		oss << i + 1 << ".";
		os << std::setw(6) << oss.str() << item << '\n';
		++i;
	}
	return os;
}

std::ostream & operator<<(std::ostream & os, const WShuffleUtils::SizeMap & sm)
{
	int i = 0;
	for (const auto &item : sm)
	{
		os << '\t';
		os << std::left;
		std::ostringstream oss;
		oss << i + 1 << ".";
		os << std::setw(6) << oss.str() << item.first << " --> " << item.second << '\n';
		++i;
	}
	return os;
}
