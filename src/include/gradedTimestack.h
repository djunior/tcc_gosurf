#ifndef __GRADED_TIMESTACK__
#define __GRADED_TIMESTACK__

#include "timestack.h"

namespace tcc {

class GradedTimestack : public Timestack {
public:
	GradedTimestack(cv::VideoCapture&);
	void save(std::string);
private:
	int fps;
};

}

#endif