#ifndef UTILS_HPP
#define UTILS_HPP

#include <list>
#include <string>
#include <codecvt>
#include <locale>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <random>
//#include <experimental/filesystem>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>



namespace WShuffleUtils {

    using StringList = std::list<std::string>;
    using VStrList = std::vector<std::string>;
	using SizeMap = std::map<std::string, size_t>;
	//using BoolList = std::vector<bool>;

	std::wstring to_wstring(const std::string& str);

	std::string to_string(const std::wstring& wstr);

	size_t count_chars(const std::string utf8_string);

	bool create_out_file(const std::string &f_in, std::string &f_out);
    bool create_out_file_list(const VStrList &in_list, VStrList &out_list);

	void show_info(const std::string &message, const StringList &lst);
	void show_info(const std::string &message, const SizeMap &sm);

	bool is_ok_to_continue(const std::string &error_message);

	template < typename T > void shuffle(std::list<T>& lst) // shuffle contents of a list
	{
		// create a vector of (wrapped) references to elements in the list
		// http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper
		std::vector< std::reference_wrapper< const T > > vec(lst.begin(), lst.end());

		// shuffle (the references in) the vector
		std::shuffle(vec.begin(), vec.end(), std::mt19937{ std::random_device{}() });

		// copy the shuffled sequence into a new list
		std::list<T> shuffled_list{ vec.begin(), vec.end() };

		// swap the old list with the shuffled list
		lst.swap(shuffled_list);
	}

	
    #ifdef _WIN32
    void systemInfo(bool verbose=false);
    double getSysOpType();
    #endif

#ifdef __GNUC__
    void systemInfo(bool verbose=false);
    std::vector<std::string> read_outline(std::string & file);

#endif
};

// A helper function to simplify the main part.
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& sl)
{
    //copy(v.begin(), v.end(), std::ostream_iterator<T>(os, "\n"));
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

std::ostream &operator << (std::ostream &os, const WShuffleUtils::StringList &sl);
std::ostream &operator<<(std::ostream &os, const WShuffleUtils::SizeMap &sm);



#endif