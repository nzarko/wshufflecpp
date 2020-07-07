#ifndef SHUFFLERBASE_H
#define SHUFFLERBASE_H

#include "ishufflerbase.h"
#include "wineditor.h"
using namespace WShuffleUtils;


class WShuffle : public IShufflerBase
{
private:
    int linesbtw;
    ///TODO: Make file_in and file_out vector of strings for multiple files handling.
    VStrList    file_in;
    VStrList    file_out;
	std::shared_ptr<Editor> spEditor;
	std::map<std::string, StringList> file_word_list_map_;
	StringList  w_list_;
	SizeMap s_map_;
	StringList out_list_;
	int repNum;
	bool shuffle_;

	//Private Functions declarations
	void format_(const StringList& str_list, StringList& out_list);
	void format(const SizeMap &in_list, StringList &out_list);
	void get_size_map(const StringList &w_list, SizeMap &s_map);

public:
    int exec() override;

    virtual bool& shuffle() {
		return shuffle_;
	}

	virtual void set_shuffle(const bool shuffle) {
		shuffle_ = shuffle;
	}

	__declspec(property(get = shuffle, put = set_shuffle)) bool Shuffle;

    virtual std::shared_ptr<Editor>& sp_editor()     {
	    return spEditor;
    }

    virtual void set_sp_editor(const std::shared_ptr<Editor>& sp_editor) {
	    spEditor = sp_editor;
    }

    __declspec(property(get = sp_editor, put = set_sp_editor)) std::shared_ptr<Editor> SpEditor;

    const VStrList &getFileOut() const;
	
   

    explicit WShuffle(const VStrList & filenames, int repetitions=5, int linesbetween = 3, bool
	 shuffe=false);
    ~WShuffle() override;
    void setLinesBetween(int lines);
    void setRepetitions(int rep);
    bool write_to(const std::string &f_name, const StringList& w_list = {}) override ;
    bool write_to() override ;

    void setFilenames(const VStrList &file_list) override;

    bool write_to(const VStrList& files) override;


    bool read_from(const std::string& f_name, StringList& w_list) override;
    bool read_from(const VStrList& file_list) override;
    virtual  std::string __str() const ;
    friend std::ostream &operator<<(std::ostream &os, const WShuffle &wShuffle);
};


#endif