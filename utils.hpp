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

	/**
	 * Count the actual characters of the string
	 * @param utf8_string
	 * @return How many characters the string has.
	 */
	size_t count_chars(const std::string utf8_string);

	/**
	 * Create a file name (path) to store the output
	 * @param f_in The input file
	 * @param f_out The file (path) to be created
	 * @return true if the creation go well, false if not.
	 */
	bool create_out_file(const std::string &f_in, std::string &f_out);
    bool create_out_file_list(const VStrList &in_list, VStrList &out_list);

	void show_info(const std::string &message, const StringList &lst);
	void show_info(const std::string &message, const SizeMap &sm);

	/**
	 * Print an question or error message and waits for the answer
	 * @param error_message The message to print
	 * @return True or False depending to the answer.
	 */
	bool is_ok_to_continue(const std::string &error_message);
	/**
	 * Get the application directory. If it doesn't exist it will try to create it.
	 * This is platform dependent. In Linux systems it is $HOME/.local/wshuffle
	 * In windows it is ...
	 * @return The application settings directory.
	 */
    std::string get_settings_dir();

    /**
     * Makes the proper initializations
     */
    void init_app();

    /**
     * Returns the full path of the file prepending the settings directory path
     * @param filename
     * @return The full path of setting file
     */
    std::string settings_file_path(const std::string &filename);


    /**
     * Shuffle the list lst
     * @tparam T The type of elemetns of the list
     * @param lst The list to shuffle.
     */
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