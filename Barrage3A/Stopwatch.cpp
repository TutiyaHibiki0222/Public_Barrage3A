#ifdef _DEBUG
#include "Stopwatch.hpp"

std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> DebugStopwatch::_startTimes;
#endif  // _DEBUG