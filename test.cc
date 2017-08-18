#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>

#include <condition_variable>
#include <thread>
#include <mutex>
#include <iostream>
#include <list>
#include <sys/inotify.h>
#include <dirent.h> 
#include <stdio.h> 
#include <iostream>
#include <limits.h> 
#include <thread>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <set>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include "myTimer.h"
#include <stdarg.h> 
#include <thread>
#include <mutex>
#include <iostream> 

using namespace std;

class Handle
{
    public:
        const std::vector<int>& get_data const & () { return *m_vec; }
        // non-const overloads, etc
        // deleted rvalue overloads!
        std::vector<int>& get_data const && () = delete;
        std::vector<int>& get_data && () = delete;
    private:
        std::vector<int>* m_vec;
        std::unique_lock<std::shared_mutex> m_lock;
}