//
//  duplicate.cpp
//  objc-zmq-reflector
//
//  Created by dev on 2021-10-28.
//  Copyright © 2021 Root Interface. All rights reserved.
//

#include "duplicate.h"

msg_cache::msg_cache(long l_poll_res_milliseconds)
{
	// https://stackoverflow.com/questions/30425772/c-11-calling-a-c-function-periodically
	//
	execute_= true;
	_thd = std::thread([this, l_poll_res_milliseconds]()
		{
			while (execute_.load(std::memory_order_acquire))
			{
				t_entries entries_del;
				
//				printf("theard::run\n");

				for (auto it : cache_)
				{
					t_entry entry_del= it.first;
					t_entry_context ctx_del= it.second;
					
					if (ctx_del.expired())
						entries_del.insert( {entry_del, ctx_del} );

					// printf("ENTRY: %s\n", entry_del.c_str());
				}
				
				for (auto it : entries_del) {
					cache_.erase(it.first);
					// printf("PURGE: %s\n", it.first.c_str());
				}
				
				std::this_thread::sleep_for(std::chrono::milliseconds(l_poll_res_milliseconds));
			}
		});
}

msg_cache::~msg_cache()
{
	if ( execute_.load(std::memory_order_acquire) ) {
		execute_.store(false, std::memory_order_release);
		if ( _thd.joinable() )
			_thd.join();
	};
}

void msg_cache::cache(t_entry entry, t_entry_context ctx) {
	cache_.insert( {entry, ctx} ); // std:make_pair()
}

bool msg_cache::contains(t_entry entry) {
	//TODO: cache_.contains(entry); c++20
	return (cache_.end() != cache_.find(entry));
}


