#include <typeinfo>
#include <sstream>
#include <iostream>
#include "shufflerbase.h"

WShuffle::WShuffle(const VStrList & filenames, int repetitions, int linesbetween, 
	bool shuffle):
    file_in(filenames),
    repNum(repetitions), shuffle_(shuffle),
    linesbtw(linesbetween)
{
    file_out.resize(file_in.size(), "");
	spEditor = create_editor();
}

WShuffle::~WShuffle()
{
}

void WShuffle::setLinesBetween(int lines) {
    linesbtw = lines;
}

void WShuffle::setRepetitions(int rp) {
    repNum = rp;
}

bool WShuffle::write_to() {
    return false;
}

bool WShuffle::write_to(const std::string &f_name, const StringList& w_list) {
	std::ofstream ofs(f_name);
	if (!ofs.is_open())
	{
		std::cout << "Error writing " << f_name << std::endl;
		return false;
	}

	std::cout << "Writing to file : " << f_name << '\n';

	size_t bn = 1;
	const size_t n_of_ch = count_chars(w_list.front());
	auto separator = [&bn](int line_size)->std::string {
		std::string label = "Block ";
		label += std::to_string(bn);
		if (line_size < count_chars(label)) {
			bn++;			
		} else {
			int label_len = count_chars(label);
			int dist = line_size - label_len;
			std::string gt(dist / 2, '>');
			std::string lt(dist / 2, '<');
			label = gt + label + lt;
			bn++;
		}
		return label;
	};	
	ofs << separator(n_of_ch) <<  '\n'<<'\n';
	
	for (const auto &fw : w_list) {
		if (fw[0] != '>') {
			ofs << fw << '\n';
			ofs << '\n';
		} else {
			
			std::string sep(repNum, '\n');
			ofs << sep;
			ofs << separator(n_of_ch) << '\n' << '\n';
		}
	}
    return true;
}

void WShuffle::setFilenames(const VStrList &file_list) {
    file_in = file_list;    
}

bool WShuffle::write_to(const VStrList &file_list) {
    return false;
}

void WShuffle::format_(const StringList& str_list, StringList& out_list)
{
	get_size_map(str_list, s_map_);
	format(s_map_, out_list);
}

void WShuffle::format(const SizeMap & in_list, StringList & out_list)
{
	SizeMap::const_iterator p_max_size =
		std::max_element(in_list.begin(), in_list.end(),
			[](const std::pair<std::string, size_t> &a, const std::pair<std::string, size_t> &b) -> bool { return a.second < b.second; });

//	std::cout << "String with max size : " << '\n';
//	std::cout << p_max_size->first << " , " << p_max_size->second << "\n";

	std::string equalto = "->_______________________";
	std::transform(in_list.begin(), in_list.end(), std::back_inserter(out_list),
		[p_max_size, equalto](const std::pair<std::string, size_t> &in_pair) -> std::string {
		std::string res;
		res += in_pair.first;
		for (int i = 0; i < p_max_size->second - in_pair.second + 2; i++) {
			res += ".";
		}
		res += equalto;
		return res;
	});
	//set the lines between repetitions
	//StringList sep = std::list<std::string>(linesbtw, std::string("\n"));
	std::string sep = ">>>>SEPARATOR<<<<";
	//out_list.insert(out_list.end(), sep.begin(), sep.end());
	//out_list.push_back(sep);
	//Copy the contents of out_list so we can get insert them back to out_list
	//StringList help_list = { out_list.begin(), out_list.end() };
	if (!shuffle_) {
		///TODO : shuffle the formatted list repNum times and add the result at the the end.
		///
		StringList help_list = { out_list.begin(), out_list.end() };
		WShuffleUtils::shuffle(help_list);
		out_list.assign(help_list.begin(), help_list.end());
		out_list.push_back(sep);
		for (int i = 1; i < repNum; i++) {
			WShuffleUtils::shuffle(help_list);
			out_list.insert(out_list.end(), help_list.begin(), help_list.end());
			out_list.push_back(sep);
		}
	} else {
		///TODO : Add repNum copies of the formatted list at end of it.
		out_list.push_back(sep);
		StringList help_list = { out_list.begin(), out_list.end() };
		for (int i =1; i <= repNum-1; i++) {
			out_list.insert(out_list.end(), help_list.begin(), help_list.end());
		}
	}
	out_list.pop_back();
}

void WShuffle::get_size_map(const StringList & w_list, SizeMap & s_map)
{
	if (!s_map.empty()) {
		s_map.clear();
	}

	for (auto const &wl : w_list)	{
		s_map[wl] = count_chars(wl);
	}
}

int WShuffle::exec()
{
	VStrList real_file_input; // Ignores the files where errors occurred
	for (const auto &fn : file_in)
	{
		if (!read_from(fn, w_list_)) {
			if(!is_ok_to_continue(std::string("Skip this file?")))
			{
				::exit(-10);
			}
			else
				continue;
		}
		//show_info(fn + " contents : ", w_list);
		//Success. Push it back in production files
		real_file_input.push_back(fn);
		get_size_map(w_list_, s_map_);
		//show_info(fn + " contents : ", s_map_);
		StringList out_list;
		format(s_map_, out_list);
		//show_info("Formatted translations :", out_list);
		
		std::string out_filename;
		create_out_file(fn, out_filename);
		file_out.push_back(out_filename);
		if(!write_to(out_filename, out_list)) {
			if( !is_ok_to_continue(std::string("Skip this file?"))) {
				::exit(-14);
			}
		}
	}
	//Set Editor arguments.
	spEditor->set_arguments(file_out);

	/*
	 * Start the editor process.
	 */
	return spEditor->launch();
}

const VStrList &WShuffle::getFileOut() const {
    return file_out;
}

bool WShuffle::read_from(const std::string & f_name, StringList & w_list)
{
	namespace bt = boost::algorithm;
	if (!w_list.empty())
		w_list.clear();

	std::ifstream ifs(f_name);
	if (!ifs.is_open())
	{
		std::cout << "Error reading " << f_name << std::endl;
		return false;
	}
	std::cout << "Reading file " << f_name << '\n';

	std::string line;
	while (std::getline(ifs, line))
	{
		bt::trim(line);
		w_list.push_back(line);
	}
	return true;
}

bool WShuffle::read_from(const VStrList & file_list)
{
	return false;
}

std::string WShuffle::__str() const {
    std::ostringstream os;
    os << typeid(this).name() << '\n';
    os << "Input files are : " << file_in ;
    os << "Output files are : " << file_out;
    os << "Repetitions of Block are : " << repNum << '\n';
    os << "Lines between Blocks are : " << linesbtw << '\n';
    os << "List Of words are : " << w_list_ << '\n';
    os << std::endl;
    return os.str();
}

std::ostream &operator<<(std::ostream &os, const WShuffle &wShuffle) {
    os << wShuffle.__str();
    return os;
}



