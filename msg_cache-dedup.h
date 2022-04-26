//
//  duplicate.hpp
//  objc-zmq-reflector
//
//  Created by dev on 2021-10-28.
//  Copyright © 2021 Root Interface. All rights reserved.
//

#ifndef duplicate_h
#define duplicate_h

#include <string>
#include <unordered_map>
#include <future>
#include <iostream>
#include <chrono>

template<class Duration>
using t_time_point_ms= std::chrono::time_point<std::chrono::high_resolution_clock, Duration>;
using namespace std::chrono_literals;

const long kl_POLL_RES_MILLISECONDS= 1000L;

// https://en.cppreference.com/w/cpp/chrono/duration/duration_cast
// https://en.cppreference.com/w/cpp/chrono/time_point/time_point_cast

typedef std::string t_entry;
class t_entry_context {
	std::chrono::milliseconds ms_TTL_;
	t_time_point_ms<std::chrono::milliseconds> tp_instant_;
public:
	t_entry_context(std::chrono::milliseconds ms_TTL) : ms_TTL_(ms_TTL) {
		tp_instant_= std::chrono::time_point_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() );
	}
	bool expired() {
		std::chrono::time_point now = std::chrono::high_resolution_clock::now();
		auto duration_integral= std::chrono::duration_cast<std::chrono::milliseconds>(now - tp_instant_ - ms_TTL_);
		// std::cout << "Elapsed Time: " << duration_integral.count() << " milliseconds" << std::endl;
		
		return (duration_integral.count() > 0);
	}
};

typedef std::unordered_map<t_entry,t_entry_context> t_entries;

class msg_cache {
	t_entries cache_;
	
	std::future<void> task_;
	std::atomic<bool> execute_;
	std::thread _thd;
	
public:
	msg_cache() : msg_cache(kl_POLL_RES_MILLISECONDS) {}
	msg_cache(long l_poll_res_milliseconds);
	~msg_cache();
	void cache(t_entry, t_entry_context);
	bool contains(t_entry);
};

class duplicate : public msg_cache {
public:
	duplicate() : msg_cache() {}
	duplicate(long l_poll_res_milliseconds) : msg_cache(l_poll_res_milliseconds) {}
	bool exists(t_entry entry) {
		return this->contains(entry);
	}
};

#endif /* duplicate_h */